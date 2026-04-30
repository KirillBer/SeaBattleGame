#ifndef SEABATTLENETWORK_BM_CPP
#define SEABATTLENETWORK_BM_CPP



#ifdef  _WIN32
//#define _WIN32_WINNT 0x0A00
#define _WIN32_WINDOWS 0x0601
#endif
/*
#define ASIO_STADNALONE
*/

#include <asio.hpp>	//Для сетевого взаимодействия
#include <thread>		//Для ThreadSafeVector
#include <mutex>		//Для ThreadSafeVector
#include <atomic>	//Для atomic объектов
#include <memory>	//Для std::shared_ptr
#include <vector>


//Режимы разработчика
//#define SEABATTLENETWORK_BM_CPP_DEBUG_MODE_FUNC	//Вывод отладочной информации [DEV]
//#define SEABATTLENETWORK_BM_CPP_DEBUG_MODE_COUT	//Вывод отладочной информации [DEV]

#ifdef SEABATTLENETWORK_BM_CPP_DEBUG_MODE_FUNC
	#include <iostream>
#endif

#ifdef SEABATTLENETWORK_BM_CPP_DEBUG_MODE_COUT
	#include <iostream>
#endif

#include <iostream>
using asio::ip::tcp;


class P2PMessenger {
private:
	class ThreadSafeVector{	//Приватный класс для потокобезопасных векторов
		private:
		public:
			std::vector<std::string> data_;
			std::mutex mutex_;
		
		public:
			ThreadSafeVector() {}
			
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
		    
		    size_t find(std::string data){									//Узнать индекс первых встреченных искомых данных
				mutex_.lock();
				for(int i = 0; i < size(); i++)
		    		if (data_[i] == data){
		    			mutex_.unlock();
		    			return i;
					}
				mutex_.unlock();
		    	return SIZE_MAX;
			}
			
			size_t rfind(std::string data){									//Узнать индекс первых встреченных искомых данных (с конца)
				mutex_.lock();
				for(int i = size() - 1; i >= 0; i--)
		    		if (data_[i] == data){
		    			mutex_.unlock();
		    			return i;
					}
				mutex_.unlock();
		    	return SIZE_MAX;
			}
		    
		    bool is_exists(std::string data){								//Имеются ли в векторе искомые данные
		    	mutex_.lock();
				for(int i = 0; i < size(); i++)
		    		if (data_[i] == data){
		    			mutex_.unlock();
		    			return true;
					}
				mutex_.unlock();
		    	return false;
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
	    
			size_t size(){										//size() - получить количество элементов в векторе
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
			
			std::string pop_back(){								//pop_back() - получить значение последнего элемента (с удалением)
				mutex_.lock();
				std::string text = data_[data_.size() - 1];
				data_.pop_back();
				mutex_.unlock();
				return text;
			}
			
			void push_back(std::string text){					//push_back() - добавление в конец
				mutex_.lock();
				data_.push_back(text);
				mutex_.unlock();
			}
			
			void clear(){										//clear() - полная очистка вектора
				mutex_.lock();
				data_.clear();
				mutex_.unlock();
			}
			
			bool rem(std::string text){							//rem() - удаление объекта с переданным содержимым; true - успешно; false - объект не найден
				if (is_exists(text)){
					erase(find(text));
					return true;
				}
				return false;
			}
			
			bool rrem(std::string text){							//rrem() - удаление объекта с переданным содержимым (с конца); true - успешно; false - объект не найден
				if (is_exists(text)){
					erase(rfind(text));
					return true;
				}
				return false;
			}
						
			void erase(int index){								//erase() - удаление по индексу
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
	
	bool first_start_ = true;					//Первый ли раз происходит запуск
    asio::io_context io_context_;
    tcp::acceptor acceptor_;
    std::shared_ptr<tcp::socket> connected_socket_;
    std::thread io_thread_;
    
    std::string remote_ip_;						//IP удалённого устройства
    std::string local_ip_;						//IP текущего устройства (локальный)
    unsigned short remote_port_;				//Порт удалённого устройства
    unsigned short listen_port_;				//Порт текущего устройства (прослушиваемый)
    
    std::atomic<bool> running_{false};			//Запущена ли работа с сетью
    std::atomic<bool> connected_{false};		//Подключён ли к другому устройству по сети
    std::atomic<bool> single_message_{true};	//В случае первого знакомства
    std::atomic<bool> connection_end_{false};	//Идёт процесс завершения соединения
    
    ThreadSafeVector system_;
    ThreadSafeVector service;
    ThreadSafeVector message;
    ThreadSafeVector data;

public:
    P2PMessenger() : acceptor_(io_context_)	{}
    
    ~P2PMessenger() {
    	if (io_thread_.joinable()){
    		stop();				//std::this_thread::sleep_for(std::chrono::seconds(2));
	    	acceptor_.cancel();
	        io_context_.stop();	//io_context_.restart();
	        io_thread_.join();
		}
		system("pause");
    }
    
    void start(){	//Начать сетевое взаимодействие
    	if (running_)
    		return;
    	running_ = true;
    	connection_end_ = false;
    	
	    if (first_start_){
	    	first_start_ = false;
	    	tcp::endpoint endpoint(tcp::v4(), 0);  //"Попросить" ОС назначить любой свободный порт; Создать точку подключения. 0 = "дай любой свободный порт"
			acceptor_.open(endpoint.protocol());	//Открыть сетевой порт
		    acceptor_.set_option(tcp::acceptor::reuse_address(true));	//Разрешить использовать адрес, даже если он использовался недавно
		    acceptor_.bind(endpoint);	//Привязать сокет к конкретному IP и порту
		    acceptor_.listen();	//Перевести acceptor в режим ожидания подключения (только с этого момента начнёт "слушать")
        	listen_port_ = acceptor_.local_endpoint().port();	//Узнать, какой порт нам дала ОС
			start_accept();	//Начинаем слушать входящие подключения
			io_thread_ = std::thread([this]() { io_context_.run(); });	//Запускаем поток для обработки сетевых событий
		}
	}
	
	void stop(){	//Завершить сетевое взаимодействие
		if (!running_)
			return;
		disconnect();
		running_ = false;
	}
	
	void reset_all_data(){
		service.clear();
		message.clear();
		data.clear();
	}
	
	bool is_connected(){
		return connected_;
	}
    
    void wait_for_connect(){
		while(!connected_){
		    std::this_thread::yield();
		    std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
	
	void wait_for_certain_service_message(std::string text){
		for(int i = 0; i < 100 && !is_connected(); i++)
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		
		while(!service.rrem(text)){
			if (!is_connected())
				throw std::string("Can not wait for service message without active connection.");
			
			std::this_thread::yield();
    		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}
	
	#ifdef SEABATTLENETWORK_BM_CPP_DEBUG_MODE_FUNC
		void print_vector(ThreadSafeVector vec){
			for(int i = 0; i < vec.size(); i++)
				std::cout << i << " '" << vec.data_[i] << "'\n";
			std::cout << std::endl;
		}
	#endif
private:	
	void _wait_for_certain_system_message(std::string text, bool while_connected = false){
		size_t pos;
		for(int i = 0; (while_connected ? is_connected() : (i < 5)); i++){
			pos = system_.find(text);
			if (pos != SIZE_MAX){
				system_.erase(pos);
				return;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
	
private:
public:
	std::string get_time(){
	    try {
	        asio::io_context io_context;
	        tcp::resolver resolver(io_context);
	        
	        auto endpoints = resolver.resolve("timeapi.io", "80");	//Используем timeapi.io для получения времени
	        
	        tcp::socket socket(io_context);
	        asio::connect(socket, endpoints);
	        
	        std::string request = 
	            "GET /api/Time/current/zone?timeZone=UTC HTTP/1.1\r\n"
	            "Host: timeapi.io\r\n"
	            "Connection: close\r\n"
	            "\r\n";
	        
	        asio::write(socket, asio::buffer(request));
	        
	        asio::streambuf response;
	        asio::error_code ec;
	        
	        while (true) {														//HTTP/1.1 301 Moved Permanently
	            asio::read(socket, response, asio::transfer_at_least(1), ec);	//Server: nginx/1.29.4
	            if (ec == asio::error::eof) break;								//Date: Fri, 17 Apr 2026 16:40:51 GMT
	            else if (ec) throw asio::system_error(ec);						//Другие пункты ответа...
	        }
	        
	        std::string time = asio::buffer_cast<const char*>(response.data());
	        time = time.substr(time.find("Date"), time.find('\n'));
	        return time.substr(time.rfind(' ') + 1, time.find('\n'));
	    } catch (std::exception& e) {
	        #ifdef SEABATTLENETWORK_BM_CPP_DEBUG_MODE_COUT
	        std::cout << "Ошибка получения времени: " << e.what() << std::endl;
	        #endif
	        return "0:0:0";
	    }
	}
	std::string get_country(const std::string& ip) {
	    try {
	        asio::io_context io_context;
	        tcp::resolver resolver(io_context);
	        
	        auto endpoints = resolver.resolve("ip-api.com", "80");	//Резолвим ip-api.com
	        
	        tcp::socket socket(io_context);
	        asio::connect(socket, endpoints);
	        
	        std::string request = "GET /json/" + ip + "?fields=status,country,message HTTP/1.1\r\n"	//Формируем HTTP запрос
	                              "Host: ip-api.com\r\n"
	                              "Connection: close\r\n\r\n";
	        
	        asio::write(socket, asio::buffer(request));
	        
	        //Читаем ответ по частям, пока не получим EOF
	        asio::streambuf response;
	        asio::error_code ec;
	        
	        while (true) {
	            asio::read(socket, response, asio::transfer_at_least(1), ec);
	            if (ec == asio::error::eof) break;	//EOF - это нормально
	            else if (ec) throw asio::system_error(ec);	//Другая ошибка 
	        }
	        
	        
	        const char* resp_data = asio::buffer_cast<const char*>(response.data());	//Получаем указатель на данные
	        size_t size = response.size();
	        
	        
	        const char* body_start = nullptr;
	        for (size_t i = 0; i < size - 3; ++i){	//Ищем начало тела ответа (после \r\n\r\n)
	            if (resp_data[i] == '\r' && resp_data[i + 1] == '\n' && 
	                resp_data[i + 2] == '\r' && resp_data[i + 3] == '\n') {
	                body_start = resp_data + i + 4;
	                break;
	            }
	        }
	        if (!body_start)	return "Unknown";
	        
	        const char* status_pos = strstr(body_start, "\"status\"");	//Ищем поле "status"
	        if (!status_pos)	return "Unknown";
	        
	        const char* colon = strchr(status_pos, ':');	//Ищем значение статуса
	        if (!colon)			return "Unknown";
	        
	        const char* value_start = strchr(colon, '"');	//Ищем начало строки значения
	        if (!value_start)	return "Unknown";
	        value_start++; //Пропускаем кавычку
	        const char* value_end = strchr(value_start, '"');	//Ищем конец строки значения
	        if (!value_end)		return "Unknown";
	        
	        std::string status(value_start, value_end - value_start);	//Проверяем статус
	        if (status != "success") 	return "Unknown";
	        
	        const char* country_pos = strstr(body_start, "\"country\"");	//Ищем поле "country"
	        if (!country_pos) 	return "Unknown";
	        
	        //Ищем значение страны
	        colon = strchr(country_pos, ':');
	        if (!colon) 		return "Unknown";
	        value_start = strchr(colon, '"');
	        if (!value_start) 	return "Unknown";
	        value_start++;
	        value_end = strchr(value_start, '"');
	        if (!value_end) 	return "Unknown";
	        
	        return std::string(value_start, value_end - value_start);	//Возвращаем название страны
	    } catch (std::exception& e) {
	    	#ifdef SEABATTLENETWORK_BM_CPP_DEBUG_MODE_COUT
	        std::cout << "Ошибка геолокации: " << e.what() << std::endl;
	        #endif
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
		catch (...) { return "Exception"; }
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
	        
	        std::istream response_stream(&response);	//Преобразуем в строку
	        
	        std::string line;
	        while (std::getline(response_stream, line) && line != "\r"); //Пропускаем HTTP заголовки (до пустой строки)
	        
	        std::getline(response_stream, ip);	//Читаем IP (это первая строка после заголовков)
	        
	        if (!ip.empty() && ip.back() == '\r')	//Убираем возможные \r в конце
	            ip.pop_back();
	        
	        socket.close();
	        
	    }
		catch (...) { return "Exception"; }
	    
	    return ip;
	}
	unsigned short get_listening_port(){
		return listen_port_;
	}
	unsigned short get_remote_port(){
		return remote_port_;
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
		std::string decoded_ip = "", temp_str = "";
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
		return decoded_ip;
	}

    void start_accept(){
        auto socket = std::make_shared<tcp::socket>(io_context_);
        
        acceptor_.async_accept(*socket,
            [this, socket](const asio::error_code& error) {
                if (!error) {
        			#ifdef SEABATTLENETWORK_BM_CPP_DEBUG_MODE_COUT
                    std::cout << "\nВходящее подключение от: " << socket->remote_endpoint() << std::endl;
                    #endif
                    handle_incoming_connection(socket);	//Принимаем подключение
                }
                //if (running_)
                start_accept();	//Продолжаем слушать новые подключения
            });
    }
    
    void handle_incoming_connection(std::shared_ptr<tcp::socket> socket) {
    	bool reject_connect = false;
    	#ifdef SEABATTLENETWORK_BM_CPP_DEBUG_MODE_COUT
    	std::string cout_message;
    	#endif
    	std::string net_message;
    	
    	if (!running_){	//Если сетевое взаимодействие отключено
    		#ifdef SEABATTLENETWORK_BM_CPP_DEBUG_MODE_COUT
    		cout_message = "Сетевое взаимодействие отключено. Отклоняю запрос от: ";
    		#endif
    		net_message = std::to_string(SYSTEM) + " ПОДКЛЮЧЕНИЕ_ОТКАЗАНО: Сетевое взаимодействие отключено\n";
    		reject_connect = true;
		}
        else if (connected_socket_ && connected_socket_->is_open()) {	//Если уже есть активное соединение
        	#ifdef SEABATTLENETWORK_BM_CPP_DEBUG_MODE_COUT
        	cout_message = "Уже есть активное подключение. Отклоняю запрос от: ";
        	#endif
    		net_message = std::to_string(SYSTEM) + " ПОДКЛЮЧЕНИЕ_ОТКАЗАНО: Уже имеется подключение\n";
    		reject_connect = true;
        }
        
        if (reject_connect){
        	#ifdef SEABATTLENETWORK_BM_CPP_DEBUG_MODE_COUT
        	std::cout << cout_message << socket->remote_endpoint() << std::endl;
        	#endif
        	try { asio::write(*socket, asio::buffer(net_message)); }
			catch (...) {}	//Игнорирование ошибок отправки
	        
	        socket->close();
			return;
		}
        
        //Подтверждение подключения
        connected_socket_ = socket;
        remote_ip_ = socket->remote_endpoint().address().to_string();
        remote_port_ = socket->remote_endpoint().port();
	    try {
	        std::string accept_msg = std::to_string(SYSTEM) + " ПОДКЛЮЧЕНИЕ_ПРИНЯТО\n";
	        asio::write(*connected_socket_, asio::buffer(accept_msg));
	    } catch (std::exception& e) {
	    	#ifdef SEABATTLENETWORK_BM_CPP_DEBUG_MODE_COUT
	        std::cout << "Не удалось отправить подтверждение: " << e.what() << std::endl;
	        #endif
	        connected_socket_.reset();
	        return;
	    }
	    
	    connected_ = true;
	    single_message_ = false;
	    #ifdef SEABATTLENETWORK_BM_CPP_DEBUG_MODE_COUT
	    std::cout << "Подключён к " << remote_ip_ << ":" << remote_port_ << std::endl;
	    #endif
	    start_read();
    }
    
    void handle_connect(std::string command = "") {
    	if (!running_)
        	return;
        
        std::string ip = remote_ip_;
        unsigned short port = remote_port_;
        if (command != ""){
        	ip = command.substr(0, command.find(' '));
        	port = std::stoi(command.substr(command.find(' '), command.length() - 1));
		}
        
        try {
            if (connected_socket_ && connected_socket_->is_open())	//Если уже есть соединение, закрываем его
                connected_socket_->close();
            
            connected_socket_ = std::make_shared<tcp::socket>(io_context_);	//Создаем новый сокет
            tcp::endpoint endpoint(asio::ip::make_address(ip), port);	//Подключаемся к удаленному пользователю
            
            asio::error_code ec;
            connected_socket_->connect(endpoint, ec);
            
            if (!ec) {
                remote_ip_ = ip;
                remote_port_ = port;

                single_message_ = false;
                start_read();	//Читаем 1 следующее сообщение (Если подключение подтвердится, то и все последующие от него)
            } else {
            	#ifdef SEABATTLENETWORK_BM_CPP_DEBUG_MODE_COUT
                std::cout << "Не удалось подключиться: " << ec.message() << std::endl;
                #endif
                connected_socket_.reset();
            }
            
        } catch (std::exception& e) {
        	#ifdef SEABATTLENETWORK_BM_CPP_DEBUG_MODE_COUT
            std::cout << "Ошибка: " << e.what() << std::endl;
            #endif
            throw std::string("Failed to connect to another user error.");
        }
    }
    
    void disconnect(){
    	if (connected_){
    		connection_end_ = true;
	    	_send_message("ЗАВЕРШЕНИЕ_СОЕДИНЕНИЯ");
	    	_wait_for_certain_system_message("ЗАВЕРШЕНИЕ_СОЕДИНЕНИЯ: ПОДТВЕРЖДЕНИЕ", true);
		}
    	close_connection();
	}
	
	void connect(){
		
	}
    
	void start_read() {
        if (!connected_socket_ || !connected_socket_->is_open())
            return;
        
        auto buffer = std::make_shared<std::vector<char>>(1024);
        
        connected_socket_->async_read_some(asio::buffer(*buffer),
            [this, buffer](const asio::error_code& error, size_t bytes_transferred) {
                if (!error && bytes_transferred > 0){	//Получили сообщение
                    if (!running_)
						start_read();
                    
					#ifdef SEABATTLENETWORK_BM_CPP_DEBUG_MODE_COUT
                    	std::cout << "(Получено сообщение): " << std::endl;
                    #endif
                    std::string message_buf(buffer->data(), bytes_transferred);
                    
                    size_t pos, reject_message_pos;
                    
                    while ((pos = message_buf.find('\n')) != std::string::npos){	//Обработка на случай, если несколько сообщений "склеились" в один пакет
	                    std::string sub_message = message_buf.substr(0, pos);	//Извлекаем одно сообщение
	                    char sm_char = sub_message[0] - '0';	//Код типа полученного сообщения
	                    std::string msg = sub_message.substr(2, pos);	//Полученное сообщение
	                    #ifdef SEABATTLENETWORK_BM_CPP_DEBUG_MODE_COUT
	                    	std::cout << "\n[" << remote_ip_ << "]: " << "'" << msg << "'" << std::endl;	//Выводим полученное сообщение
	                    #endif
						
	                    switch(sm_char){
	                    	case SYSTEM:
	                    		system_.push_back(msg);
	                    		reject_message_pos = msg.find("ПОДКЛЮЧЕНИЕ_ОТКАЗАНО");
	                    		
	                    		if (msg == "ПОДКЛЮЧЕНИЕ_ПРИНЯТО"){
	                    			connected_ = true;
	                    			single_message_ = false;
								}
								else if (reject_message_pos != std::string::npos){	//Действия на случай, если пришёл отказ на подключение (с каким либо комментарием)
									close_connection();	
								}
								else if (msg == "ЗАВЕРШЕНИЕ_СОЕДИНЕНИЯ"){
									_send_message("ЗАВЕРШЕНИЕ_СОЕДИНЕНИЯ: ПОДТВЕРЖДЕНИЕ");
									connection_end_ = true;
									//close_connection();
								}
								else if (msg == "ЗАВЕРШЕНИЕ_СОЕДИНЕНИЯ: ПОДТВЕРЖДЕНИЕ"){
									//close_connection();
								}
								#ifdef SEABATTLENETWORK_BM_CPP_DEBUG_MODE_COUT
	                    			std::cout << "Пакет системы!" << std::endl;
	                    		#endif
	                    		break;
	                    	
	                    	case SERVICE:
	                    		service.push_back(msg);
	                    		
	                    		#ifdef SEABATTLENETWORK_BM_CPP_DEBUG_MODE_COUT
	                    			std::cout << "Пакет сервиса!" << std::endl;
	                    		#endif
	                    		break;
	                    		
	                    	case MESSAGE:
	                    		message.push_back(msg);
	                    		
	                    		#ifdef SEABATTLENETWORK_BM_CPP_DEBUG_MODE_COUT
	                    			std::cout << "Пакет сообщения!" << std::endl;
	                    		#endif
	                    		break;
	                    		
	                    	case DATA:
	                    		data.push_back(msg);
	                    		
	                    		#ifdef SEABATTLENETWORK_BM_CPP_DEBUG_MODE_COUT
	                    			std::cout << "Пакет данных!" << std::endl;
	                    		#endif
	                    		break;
						}
	                    message_buf.erase(0, pos + 1);	//Удаление обработанного сообщения из буфера
                    }
                    
                    // Продолжаем чтение
                    if (!single_message_)
                    	start_read();
                }
				else if (!running_ || connection_end_){	//Соединение завершено (отключено сетевое взаимодействие)
					#ifdef SEABATTLENETWORK_BM_CPP_DEBUG_MODE_COUT
						std::cout << "\nЗавершено соединение с " << remote_ip_ << std::endl;
					#endif
					connection_end_ = false;
                	close_connection();
				}
				else if (!connected_){	//Подключиться не удалось
					#ifdef SEABATTLENETWORK_BM_CPP_DEBUG_MODE_COUT
						std::cout << "\nНе удалось подключиться к " << remote_ip_ << std::endl;
					#endif
				}
				else{	//Соединение разорвано
					#ifdef SEABATTLENETWORK_BM_CPP_DEBUG_MODE_COUT
                    	std::cout << "\nПотеряно соединение с " << remote_ip_ << std::endl;
                    #endif
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
	void send_service(std::string text){
		_send_message(text, SERVICE);
	}
	
	void send_message(std::string text){
		_send_message(text, MESSAGE);
	}
	
	void send_data(std::string text){
		_send_message(text, DATA);
    }
    
//private:
	void _send_message(std::string text, MessageType type = SYSTEM){
		if (!running_){
			#ifdef SEABATTLENETWORK_BM_CPP_DEBUG_MODE_COUT
			std::cout << "Сетевое взаимодействие отключено, а хотел отправить сообщение: " << text << std::endl;
			#endif
            return;
		}
		
		if (!connected_socket_ || !connected_socket_->is_open()) {
			#ifdef SEABATTLENETWORK_BM_CPP_DEBUG_MODE_COUT
            std::cout << "Ни к кому не подключён, а хотел отправить сообщение: " << text << std::endl;
            #endif
            return;
        }
        if (text == ""){
        	#ifdef SEABATTLENETWORK_BM_CPP_DEBUG_MODE_COUT
        	std::cout << "Попытка отправить пустое сообщение: " << text << std::endl;
        	#endif
            return;
		}
        try {
            // Добавляем символ новой строки для разделения сообщений
            //asio::write(*connected_socket_, asio::buffer(std::to_string(type) + " " + std::to_string(nnn) + " " + message + "\n"));
            asio::write(*connected_socket_, asio::buffer(std::to_string(type) + " " + text + "\n"));
            
            //Эхо своего сообщения
            #ifdef SEABATTLENETWORK_BM_CPP_DEBUG_MODE_COUT
            std::cout << "You: '" << text << "'" << std::endl;
            #endif
            
        } catch (std::exception& e) {
        	#ifdef SEABATTLENETWORK_BM_CPP_DEBUG_MODE_COUT
            std::cout << "Ошибка отправки сообщения: " << e.what() << std::endl;
            #endif
        }
    }
    
//public:
    /*
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
    */
};



#endif
