#ifndef SEABATTLENETWORK_BM_CPP
#define SEABATTLENETWORK_BM_CPP

//При start(), stop(), получается подключиться и отправлять сообщения, хоть получатель их и не увидит (не обработаются)
//#include <locale>
#include <iostream>
#include <asio.hpp>	//Для сетевого взаимодействия
#include <thread>		//Для ThreadSafeVector
#include <mutex>		//Для ThreadSafeVector
#include <queue>
#include <atomic>	//Для atomic объектов
#include <memory>	//Для std::shared_ptr
#include <vector>

using asio::ip::tcp;



class P2PMessenger {
private:
	class ThreadSafeVector{	//Приватный класс для потокобезопасных векторов
		private:
			std::vector<std::string> data_;
			std::mutex mutex_;
		
		public:
			ThreadSafeVector() {}											//Конструктор
			
			ThreadSafeVector(const ThreadSafeVector& other){				//Конструктор копирования
				*this = other;
			}
			
			ThreadSafeVector& operator=(const ThreadSafeVector& other) {	//Перегрузка =
				mutex_.lock();
		        data_ = other.data_;
		        mutex_.unlock();
				return *this;
		    }
			
			std::string& operator[](int index) {							//Перегрузка []
		        mutex_.lock();
		        std::string &text = data_[index];
		        mutex_.unlock();
				return text;
		    }
		    
		    bool is_locked(){									//Узнать, заблокирован ли mutex
		    	if (mutex_.try_lock()){
		    		mutex_.unlock();
		    		return false;
				}
				return true;
			}
		    
		    void lock(){										//Заблокировать mutex
		    	mutex_.lock();
			}
			
			void unlock(){										//Разблокировать mutex
				mutex_.unlock();
			}
	    
			size_t size(){										//size()
				return data_.size();
			}
			
			std::string first(){								//Получить значение первого элемента (без удаления)
				mutex_.lock();
				std::string text = data_[0];
				mutex_.unlock();
				return text;
			}
			
			std::string last(){									//Получить значение последнего элемента (без удаления)
				mutex_.lock();
				std::string text = data_[data_.size() - 1];
				mutex_.unlock();
				return text;
			}
			
			std::string pop_back(){								//pop_back(), который ещё и возвращает значение удалённого элемента
				mutex_.lock();
				std::string text = data_[data_.size() - 1];
				data_.pop_back();
				mutex_.unlock();
				return text;
			}
			
			void push_back(std::string text){					//push_back()
				mutex_.lock();
				data_.push_back(text);
				mutex_.unlock();
			}
			
			void clear(){										//clear()
				mutex_.lock();
				data_.clear();
				mutex_.unlock();
			}
			
			void erase(int index){								//erase()
				mutex_.lock();
				data_.erase((data_.begin() + index));
				mutex_.unlock();
			}
			
	};
public:
	typedef enum MessageType{
		MessageTypeStart,
		SYSTEM,
		SERVICE,
		MESSAGE,
		DATA,
		MessageTypeEnd
	}MessageType;
	bool first_start_ = true;
	bool ignore_messages = true;
    asio::io_context io_context_;
    tcp::acceptor acceptor_;
    std::shared_ptr<tcp::socket> connected_socket_;
    std::thread io_thread_;
    //std::mutex mutex_;
    //std::queue<std::string> message_queue_;
    
    std::string remote_ip_;						//IP удалённого устройства
    std::string local_ip_;						//IP текущего устройства (локальный)
    unsigned short remote_port_;				//Порт удалённого устройства
    unsigned short listen_port_;				//Порт текущего устройства (прослушиваемый)
    
    
    std::atomic<bool> running_{false};			//Работает ли класс с сетью
    std::atomic<bool> connected_{false};		//Подключён ли к другому устройству по сети
    std::atomic<bool> single_message_{true};	//В случае первого знакомства 			
    
    std::string required_message = "";
    std::string last_read_service_message = "last_read_service_message";
    std::atomic<bool> met_required_message{false};
    
    
    ThreadSafeVector service_;
    ThreadSafeVector message_;
    ThreadSafeVector data_;
    //std::mutex service_mutex_;
    //std::queue<std::string> service_;
    
    //std::mutex data_mutex_;
    //std::vector<std::string> data_;

    
public:
    P2PMessenger() : acceptor_(io_context_)	{}
    
    ~P2PMessenger() {
    	stop();
    }
    
    void start(){	//Начать сетевое взаимодействие
    	if (running_)
    		return;
    	running_ = true;
    	ignore_messages = false;
    	
    	
	    if (first_start_){
	    	first_start_ = false;
	    	//"Попросить" ОС назначить любой свободный порт
	    	tcp::endpoint endpoint(tcp::v4(), 0);  //Создать точку подключения. 0 = "дай любой свободный порт"
			acceptor_.open(endpoint.protocol());	//Открыть сетевой порт
		    acceptor_.set_option(tcp::acceptor::reuse_address(true));	//Разрешить использовать адрес, даже если он использовался недавно
		    acceptor_.bind(endpoint);	//Привязать сокет к конкретному IP и порту
		    acceptor_.listen();	//Перевести acceptor в режим ожидания подключения (только с этого момента начнёт "слушать")
		}
		
		
        //local_ip_ = acceptor_.address().to_string();
        
        //Узнать, какой порт нам дала ОС
        listen_port_ = acceptor_.local_endpoint().port();
        
        //Начинаем слушать входящие подключения
        start_accept();
        
        //Запускаем поток для обработки сетевых событий
        io_thread_ = std::thread([this]() { io_context_.run(); });
	}
	
	void stop(){	//Завершить сетевое взаимодействие
		running_ = false;
		ignore_messages = true;
        if (io_thread_.joinable()) {
        	if (connected_){
        		_send_message("ЗАВЕРШЕНИЕ_СОЕДИНЕНИЯ");
        		close_connection();
			}
        	acceptor_.cancel();
            io_context_.stop();
            io_thread_.join();
            io_context_.restart();
        }
	}
	
	bool is_connected(){
		return connected_;
	}
    
    void wait_for_connect(){
    	while(!connected_){
    		std::this_thread::yield();
    		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}
	
	void set_certain_message(std::string text){
		required_message = text;
	}
	
	void wait_for_certain_message(){
		int i = 0;
		while(!met_required_message){
    		std::this_thread::yield();
    		std::this_thread::sleep_for(std::chrono::milliseconds(10));
    		if (i % 100 == 0)
    			std::cout << "Ожидание слова.\n";
    		i++;
		}
		met_required_message = false;
	}
	
private:
public:
	std::string get_country(const std::string& ip) {
	    try {
	        asio::io_context io_context;
	        tcp::resolver resolver(io_context);
	        
	        // Резолвим ip-api.com
	        auto endpoints = resolver.resolve("ip-api.com", "80");
	        
	        tcp::socket socket(io_context);
	        asio::connect(socket, endpoints);
	        
	        // Формируем HTTP запрос
	        std::string request = "GET /json/" + ip + "?fields=status,country,message HTTP/1.1\r\n"
	                              "Host: ip-api.com\r\n"
	                              "Connection: close\r\n\r\n";
	        
	        asio::write(socket, asio::buffer(request));
	        
	        // Читаем ответ по частям, пока не получим EOF
	        asio::streambuf response;
	        asio::error_code ec;
	        
	        while (true) {
	            asio::read(socket, response, asio::transfer_at_least(1), ec);
	            if (ec == asio::error::eof) {
	                // EOF - это нормально, выходим из цикла
	                break;
	            } else if (ec) {
	                // Другая ошибка
	                throw asio::system_error(ec);
	            }
	        }
	        
	        // Получаем указатель на данные
	        const char* data = asio::buffer_cast<const char*>(response.data());
	        size_t size = response.size();
	        
	        // Ищем начало тела ответа (после \r\n\r\n)
	        const char* body_start = nullptr;
	        for (size_t i = 0; i < size - 3; ++i) {
	            if (data[i] == '\r' && data[i+1] == '\n' && 
	                data[i+2] == '\r' && data[i+3] == '\n') {
	                body_start = data + i + 4;
	                break;
	            }
	        }
	        
	        if (!body_start) {
	            return "Unknown";
	        }
	        
	        // Ищем поле "status"
	        const char* status_pos = strstr(body_start, "\"status\"");
	        if (!status_pos) {
	            return "Unknown";
	        }
	        
	        // Ищем значение статуса
	        const char* colon = strchr(status_pos, ':');
	        if (!colon) {
	            return "Unknown";
	        }
	        
	        // Ищем начало строки значения
	        const char* value_start = strchr(colon, '"');
	        if (!value_start) {
	            return "Unknown";
	        }
	        value_start++; // Пропускаем кавычку
	        
	        // Ищем конец строки значения
	        const char* value_end = strchr(value_start, '"');
	        if (!value_end) {
	            return "Unknown";
	        }
	        
	        // Проверяем статус
	        std::string status(value_start, value_end - value_start);
	        if (status != "success") {
	            return "Unknown";
	        }
	        
	        // Ищем поле "country"
	        const char* country_pos = strstr(body_start, "\"country\"");
	        if (!country_pos) {
	            return "Unknown";
	        }
	        
	        // Ищем значение страны
	        colon = strchr(country_pos, ':');
	        if (!colon) {
	            return "Unknown";
	        }
	        
	        value_start = strchr(colon, '"');
	        if (!value_start) {
	            return "Unknown";
	        }
	        value_start++;
	        
	        value_end = strchr(value_start, '"');
	        if (!value_end) {
	            return "Unknown";
	        }
	        
	        // Возвращаем название страны
	        return std::string(value_start, value_end - value_start);
	        
	    } catch (std::exception& e) {
	        std::cout << "Ошибка геолокации: " << e.what() << std::endl;
	        return "Unknown";
	    }
	}
	
	std::string get_local_ip(){
		try {
            asio::io_context tmp_io_context;
            tcp::resolver resolver(tmp_io_context);
            
            auto endpoints = resolver.resolve(asio::ip::host_name(), "");	//Получаем локальный hostname и резолвим его
            for (const auto& endpoint : endpoints) {
                auto addr = endpoint.endpoint().address();
                if (addr.is_v4() && !addr.is_loopback())	//Проверяем, что это IPv4 и не loopback адрес (не 127.X.X.X)
                    return addr.to_string();
            }
        }
		catch (...) {
	        return "Exception";
	    }
	}
	std::string get_public_ip(){
        asio::error_code ec;
	    std::string ip = "Unknown";
	    try {
	        tcp::resolver resolver(io_context_);
	        auto endpoints = resolver.resolve("api.ipify.org", "80", ec);
	        
	        if (ec) return "Resolve error";
	        
	        //Подключаемся
	        tcp::socket socket(io_context_);
	        asio::connect(socket, endpoints, ec);
	        
	        if (ec) return "Connect error";

	        std::string request =
	            "GET / HTTP/1.1\r\n"
	            "Host: api.ipify.org\r\n"
	            "Connection: close\r\n"
	            "\r\n";
	        
	        asio::write(socket, asio::buffer(request), ec);
	        if (ec) return "Write error";
	        
	        //Читаем весь ответ
	        asio::streambuf response;
	        asio::read(socket, response, asio::transfer_all(), ec);
	        
	        if (ec && ec != asio::error::eof) return "Read error";
	        
	        //Преобразуем в строку
	        std::istream response_stream(&response);
	        
	        //Пропускаем HTTP заголовки (до пустой строки)
	        std::string line;
	        while (std::getline(response_stream, line) && line != "\r") {
	            // ничего не делаем, просто пропускаем
	        }
	        
	        //Читаем IP (это первая строка после заголовков)
	        std::getline(response_stream, ip);
	        
	        //Убираем возможные \r в конце
	        if (!ip.empty() && ip.back() == '\r')
	            ip.pop_back();
	        
	        socket.close();
	        
	    }
		catch (...) {
	        return "Exception";
	    }
	    
	    return ip;
	}
	
	std::string get_local_code(){
		return encode_address(get_local_ip(), listen_port_);
	}
	std::string get_public_code(){
		return encode_address(get_public_ip(), listen_port_);
	}
	
	std::string encode_address(std::string IP, unsigned short int port){	//Закодировать IP и порт в виде одной строки
		const std::string CODE_ALPHABET = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
		std::string code = "";
		//std::cout << "Передали в кодер:\n\tIP: " << IP << "\n\tПорт: " << port << std::endl;
		int temp;
		unsigned char num[4];
		unsigned short int pos = IP.find('.');
		bool flag = false;
	
		for(int i = 0; i < 4 && (pos < 10 || flag); i++){
			temp = std::stoi(IP.substr(0, pos));
			num[i] = temp;
			if (!flag) IP = IP.substr(pos + 1, IP.length() - 1);
			pos = IP.find('.');
			if (pos >= 10)
				flag = true;
		}
		
		temp = CODE_ALPHABET.length();
		//При IP = 192.20.140.235
		//192 (0-й элемент) / 62 = 3 и 140 (2-й элемент) / 62 = 2
		//3 * 10 + 2 = 32 => будет закодировано первым символом
		//(массив_символов_для_кодирования[32]) в итоговом коде. Для 1-го и
		//3-го элементов аналогично, но это будет вторым символом.
		for(int i = 0; i < 2; i++)
			code += CODE_ALPHABET[10 * (num[i] / temp) + (num[i + 2] / temp)];
		//Запись остатков от деления каждого байта IP адреса на количество символов доступных для кодирования
		for(int i = 0; i < 4; i++)
			code += CODE_ALPHABET[num[i] % temp];
		//Запись закодированного порта в итоговый код
		IP = "";
	    for(;port > 0; port = port / temp)
	    	IP += CODE_ALPHABET[port % temp];
		
		char ch_;
		for(int i = 0; i < IP.length() / 2; i++){
			ch_ = IP[i];
			IP[i] = IP[IP.length() - i - 1];
			IP[IP.length() - i - 1] = ch_;
		}
		
		return (code + IP);
	}
	
	void decode_and_use_code(std::string code){
		remote_ip_ = decode_address(code, &remote_port_);
	}
	
	std::string decode_address(std::string code, unsigned short int *port){	//Декодировать из строки IP и порт
		const std::string CODE_ALPHABET = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
		std::string decoded_ip = "";
		std::string temp_str = "";
		//std::cout << "Передали в декодер: " << code << std::endl;
		int temp;
		
	
		for(int i = 2; i < 4; i++){
			temp = CODE_ALPHABET.find(code[i - 2]);
			decoded_ip += std::to_string((temp / 10) * CODE_ALPHABET.length() + CODE_ALPHABET.find(code[i])) + ".";
			decoded_ip += temp_str;
			temp_str = std::to_string((temp % 10) * CODE_ALPHABET.length() + CODE_ALPHABET.find(code[i + 2])) + (i == 2 ? "." : "");
		}
		decoded_ip += temp_str;
		
		auto pow_ = [](int x, int y){	//Лямбда функция для возведения x в степень y
		    int result = 1;
			for(; y > 0; y--)
				result *= x;
			return result;
		};
		
		*port = 0;
		for(int i = 6; i < code.length(); i++)
			*port += CODE_ALPHABET.find(code[i]) * pow_(CODE_ALPHABET.length(), (code.length() - i - 1));
	
		//std::cout << "Итоговый декодированный адрес: " << decoded_ip << std::endl;
		return decoded_ip;
	}




    void start_accept(){
        auto socket = std::make_shared<tcp::socket>(io_context_);
        
        acceptor_.async_accept(*socket,
            [this, socket](const asio::error_code& error) {
                if (!error) {
                    std::cout << "\nВходящее подключение от: "
                              << socket->remote_endpoint() << std::endl;
                    
                    //Принимаем подключение
                    handle_incoming_connection(socket);
                }
                
                //Продолжаем слушать новые подключения
                if (running_)
                    start_accept();
            });
    }
    
    void handle_incoming_connection(std::shared_ptr<tcp::socket> socket) {
        // Если уже есть активное соединение, закрываем его
        if (connected_socket_ && connected_socket_->is_open()) {
            std::cout << "Уже есть активное подключение. Отклоняю запрос от: "
                  << socket->remote_endpoint() << std::endl;
	        
	        //Сообщение об отказе
	        try {
	            std::string reject_msg = std::to_string(SERVICE) + " ПОДКЛЮЧЕНИЕ_ОТКАЗАНО: Уже имеется подключение\n";
	            asio::write(*socket, asio::buffer(reject_msg));
	        } catch (...) {
	            //Игнорирование ошибок отправки
	        }
	        
	        socket->close();
	        return;
        }
        
        connected_socket_ = socket;
        remote_ip_ = socket->remote_endpoint().address().to_string();
        remote_port_ = socket->remote_endpoint().port();
        
        // Подтверждаем подключение
	    try {
	        std::string accept_msg = std::to_string(SERVICE) + " ПОДКЛЮЧЕНИЕ_ПРИНЯТО\n";
	        asio::write(*connected_socket_, asio::buffer(accept_msg));
	    } catch (std::exception& e) {
	        std::cout << "Не удалось отправить подтверждение: " << e.what() << std::endl;
	        connected_socket_.reset();
	        return;
	    }
	    
	    connected_ = true;
	    single_message_ = false;
	    std::cout << "Подключён к " << remote_ip_ << ":" << remote_port_ << std::endl;
	    start_read();
    }
    
    void handle_connect(std::string command) {
        std::string ip = command.substr(0, command.find(' '));
        unsigned short port = std::stoi(command.substr(command.find(' '), command.length() - 1));
        
        try {
            // Если уже есть соединение, закрываем его
            if (connected_socket_ && connected_socket_->is_open()) {
                connected_socket_->close();
            }
            
            // Создаем новый сокет
            connected_socket_ = std::make_shared<tcp::socket>(io_context_);
            
            // Подключаемся к удаленному пользователю
            tcp::endpoint endpoint(asio::ip::make_address(ip), port);
            
            asio::error_code ec;
            connected_socket_->connect(endpoint, ec);
            
            if (!ec) {
                remote_ip_ = ip;
                remote_port_ = port;
                
                //std::cout << "Успешно подключён к " << ip << ":" << port << std::endl;
                
                // Начинаем читать сообщения
                single_message_ = false;
                start_read();
            } else {
                std::cout << "Не удалось подключиться: " << ec.message() << std::endl;
                connected_socket_.reset();
            }
            
        } catch (std::exception& e) {
            std::cout << "Ошибка: " << e.what() << std::endl;
        }
    }
    
    /*
	asio::streambuf read_buffer_;
	
	void start_reads() {
    if (!connected_socket_ || !connected_socket_->is_open()) {
        return;
    }

    
    asio::async_read_until(*connected_socket_, read_buffer_, '\n',	//Читаем до символа новой строки
        [this](const asio::error_code& error, size_t bytes_transferred) {
            if (!error) {
                //Извлекаем все полные сообщения из буфера
                std::istream is(&read_buffer_);
                std::string line;
                
                while (std::getline(is, line)) {
                    if (!line.empty()) {
                        //Убираем \r если есть
                        while (!line.empty() && (line.back() == '\r' || line.back() == '\n'))
                            line.pop_back();
                        
                        // ???????????? ?????????
                        std::cout << "\n[" << remote_ip_ << "]: " << line << std::endl;
                    
						if (line[0] - '0' == MESSAGE){
							std::cout << "Пакет сообщение!" << std::endl;
						}
						else if (line[0] - '0' == SERVICE){
							std::cout << "Пакет сервиса!" << std::endl;
						}
						else if (line[0] - '0' == MOVE){
							std::cout << "Пакет с ходом!" << std::endl;
						}
                    }
                }
                
                //Продолжаем чтение
                start_read();
            }
			else{
            	// Соединение разорвано
                std::cout << "\nПотеряно соединение с " << remote_ip_ << std::endl;
                std::cout << "Вы: " << std::flush;
                    
                if (connected_socket_) {
                	connected_socket_->close();
                	connected_socket_.reset();
            	}
            }
        });
	}
	*/
	
	void start_read() {
        if (!connected_socket_ || !connected_socket_->is_open()) {
            return;
        }
        
        auto buffer = std::make_shared<std::vector<char>>(1024);
        
        connected_socket_->async_read_some(asio::buffer(*buffer),
            [this, buffer](const asio::error_code& error, size_t bytes_transferred) {
                if (!error && bytes_transferred > 0) {
                	if (ignore_messages){
                		std::cout << "Сообщение получено, но я должен их игнорировать.\n";
                		if (!single_message_)
                    		start_read();
                    	return;
					}
                    // Получили сообщение
                    std::cout << "(Получено сообщение): " << std::endl;
                    std::string message(buffer->data(), bytes_transferred);
                    
                    
                    /*
                    // Убираем лишние символы новой строки
                    while (!message.empty() && (message.back() == '\n' || message.back() == '\r')){
                    	std::cout << "(Удаление символа из строки)" << std::endl;
						message.pop_back();
					}
					*/
					
                    size_t pos;
                    while ((pos = message.find('\n')) != std::string::npos){	//Обработка на случай, если несколько сообщений "склеились" в один пакет
	                    std::string sub_message = message.substr(0, pos);	//Извлекаем одно сообщение
	                    //Выводим полученное сообщение
	                    std::cout << "\n[" << remote_ip_ << "]: " << sub_message << std::endl;
	                    
	                    char sm_char = sub_message[0] - '0';
	                    std::string msg = sub_message.substr(2, pos);
	                    if (sm_char != SYSTEM && (msg == required_message || msg == last_read_service_message)){
	                    	met_required_message = true;
	                    	std::cout << "Встретил требуемое сообщение.\n";
						}
	                    switch(sm_char){
	                    	case SYSTEM:
	                    		if (msg == "ПОДКЛЮЧЕНИЕ_ПРИНЯТО"){
	                    			connected_ = true;
	                    			single_message_ = false;
								}
								else if (msg == "ПОДКЛЮЧЕНИЕ_ОТКАЗАНО: Уже имеется подключение"){
									
								}
								else if (msg == "ЗАВЕРШЕНИЕ_СОЕДИНЕНИЯ"){
									close_connection();
								}
	                    		std::cout << "Пакет системы!" << std::endl;
	                    		last_read_service_message = msg;
	                    		break;
	                    	case SERVICE:
	                    		std::cout << "Пакет сервиса!" << std::endl;
	                    		break;
	                    	case MESSAGE:
	                    		std::cout << "Пакет сообщения!" << std::endl;
	                    		break;
	                    	case DATA:
	                    		std::cout << "Пакет данных!" << std::endl;
	                    		break;
						}

						//Удаление обработанного сообщения из буфера
	                    message.erase(0, pos + 1);
                    }
                    
                    // Продолжаем чтение
                    if (!single_message_)
                    	start_read();
                } else {
                    // Соединение разорвано
                    std::cout << "\nПотеряно соединение с " << remote_ip_ << std::endl;
                    
                    close_connection();
                }
            });
    }
    
    void close_connection(){
    	connected_ = false;
		single_message_ = true;
    	if (connected_socket_) {
            connected_socket_->close();
            connected_socket_.reset();
        }
	}
//public:
	void send_service(std::string message){
		_send_message(message, SERVICE);
	}
	
	void send_message(std::string message){
		_send_message(message, MESSAGE);
	}
	
	void send_data(std::string message){
		_send_message(message, DATA);
    }
    
//private:
	void _send_message(std::string message, MessageType type = SYSTEM){
		if (!connected_socket_ || !connected_socket_->is_open()) {
            std::cout << "Ни к кому не подключён.\n";
            return;
        }
        if (message == "")
        	return;
        try {
            // Добавляем символ новой строки для разделения сообщений
            //asio::write(*connected_socket_, asio::buffer(std::to_string(type) + " " + std::to_string(nnn) + " " + message + "\n"));
            asio::write(*connected_socket_, asio::buffer(std::to_string(type) + " " + message + "\n"));
            
            //Эхо своего сообщения
            std::cout << "You: " << message << std::endl;
            
        } catch (std::exception& e) {
            std::cout << "Ошибка отправки сообщения: " << e.what() << std::endl;
        }
    }
    
//public:
    void print_status() {
        std::cout << "\n=== Статус ===" << std::endl;
        std::cout << "Слушаю на порту: " << listen_port_ << std::endl;
        
        if (connected_socket_ && connected_socket_->is_open()) {
            std::cout << "Подключён к: " << remote_ip_ << ":" << remote_port_ << std::endl;
            std::cout << "Статус: ОНЛАЙН" << std::endl;
        } else {
            std::cout << "Подключён к: Никто" << std::endl;
            std::cout << "Статус: ОФЛАЙН" << std::endl;
        }
        std::cout << "==============" << std::endl;
    }
};




#endif
