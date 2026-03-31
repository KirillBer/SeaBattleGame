#include <iostream>
#include <conio.h>



//#include <locale>

/*
#ifdef  _WIN32
#define _WIN32_WINNT 0x0A00
#endif
#define ASIO_STADNALONE
*/






//Из морского боя
#include <cstdlib>


//#include "network.cpp"
#include "seaBattleField.h"		//Библиотека "Поле игры 'Морской бой'"
#include "seaBattleGame.cpp"	//Контейнер для поля игры
#include "seaBattleBot.cpp"		//Бот для игры
#include "seaNetwork.cpp"		//Библиотека "Работа с сетью" для игры 'Морской бой'

#include <vector>	//Для сохранения и отката ходов
#include <cstring>	//Для работы с файлами
#include <fstream>	//Для работы с файлами
#include <windows.h> // для изменения цвета текста
#include <time.h> // для использования генератора случайных чисел


using namespace std;















int mainggg(){
	setlocale(LC_ALL, "Rus");
	
	asio::error_code ec;
	asio::io_context context;
	std::thread io_thread = std::thread([&]() { context.run();	});
	/*
	asio::ip::tcp::resolver resolver(context);	//Чтобы работать с URL вместо IP адреса
	auto endpoints = resolver.resolve("youtube.com", "80", ec);
	DisplayDomens(endpoints);
    */
	
	asio::ip::tcp::endpoint endpoint(asio::ip::make_address("64.233.164.93", ec), 80);
	//asio::ip::tcp::endpoint endpoint(asio::ip::make_address("93.184.216.34", ec), 80);
	asio::ip::tcp::socket socket(context);
	
	
	//Попытка подключиться ко всем полученным endpoint'ам
	cout << "Соединяем...\n";
    //asio::connect(socket, endpoints, ec);
	//socket.connect(endpoint, ec);
	
    
    // 4. Пытаемся подключиться
    socket.connect(endpoint, ec);
	
	
    /*
		if (!ec) {
            std::cout << "Соединение установлено!\n";
            
            std::string request =
                "GET / HTTP/1.1\r\n"
                "Host: example.com\r\n"
                "Connection: close\r\n"
                "\r\n";
            
            // Отправляем запрос
            asio::write(socket, asio::buffer(request), ec);
            
            if (ec) {
                std::cout << "Write failed: " << ec.message() << std::endl;
                return 1;
            }
            
            // Читаем ответ
            asio::streambuf response;
            asio::read(socket, response, asio::transfer_all(), ec);
            
            if (ec && ec != asio::error::eof) {
                std::cout << "Read failed: " << ec.message() << std::endl;
            } else {
                std::cout << "Ответ получен:\n";
                std::cout << &response << std::endl;
            }
            
            socket.close();
            
        } else {
            std::cout << "Ошибка соединения: " << ec.message() << std::endl;
        }
        
        
	
	*/
	
	
	/*
	if(!ec){
		std::cout << "Good!\n";
	}
	else{
		std::cout << "Bad\n" << ec.message() << std::endl;
	}
	*/
	std::cout << "Конец попытки соединения.\n";
	
	std::thread thread1 = std::thread([&]() { context.run();	});
	
	if (io_thread.joinable()) {
        //context.stop();
        io_thread.join();
    }
	
	system("pause");
	return 0;
}








	















class SeaBattleGameMenu {
private:
    SeaBattleGame* player1;
    SeaBattleBot* player2;
    int player1_color;
    int player2_color;
    bool IsModeVsComputer;
    bool player_turn;
    
    //Для сетевой игры
    typedef struct Player{
    	unsigned long long id = 0;
		std::string name = "name";
		SeaBattleGame field;
    	unsigned short color = 0;
    	std::string country = "country";
	}Player;
	bool host;
	Player MP_player1;
    Player MP_player2;
    P2PMessenger SeaNet;
    
    enum MENU {
        NEW_GAME = 0,
        MULTIPLAYER,
        CHANGE_SHIPS,
        RESET_SHIPS_COUNT,
        CHANGE_MAX_SHIP_LEN,
        LOAD,
        INSTRUCTION,
        EXIT,
        MENU_LEN
    };
    
    enum GAME_MODE {
        PLAYER_VS_PLAYER = 1,
        PLAYER_VS_COMPUTER = 2,
        BACK_TO_MENU = 3
    };

public:
    SeaBattleGameMenu(int cols_ = 10, int rows_ = 10) 
	{
		player1 = new SeaBattleGame(cols_, rows_);
		player2 = new SeaBattleBot(cols_, rows_);
		player1_color = 1;
		player2_color = 2;
		player_turn = true;  // true - ход игрока, false - ход компьютера / другого игрока
	}
    
    ~SeaBattleGameMenu() {
        if (player1) delete player1;
        if (player2) delete player2;
    }
    
    void Run() {
        cout << "Добро пожаловать в игру 'Морской бой'.\n";
        MainMenu();
    }

private:
    void MainMenu() {
        int choice = -1;
        
        while (true) {
            cout << "Вы находитесь в главном меню. Введите номер того действия, которое хотите выполнить.\n";
            PrintMenu();
            
            if (!(cin >> choice) || choice < 0 || choice >= MENU_LEN) {
                InputErrorMessage();
                continue;
            }
            
            system("cls");
            
            switch (choice) {
                case NEW_GAME:
                	player1->Reset();
                	player2->Reset();
                	player2->ResetBot();
                    NewGame();
                    
                    
                    break;
                case CHANGE_SHIPS:
                    ChangeShipsCount();
                    break;
                case MULTIPLAYER:
                	Multiplayer();
					break;
                case RESET_SHIPS_COUNT:
                    ResetShipsCount();
                    break;
                case CHANGE_MAX_SHIP_LEN:
                    ChangeMaxShipLen();
                    break;
                case LOAD:
                	if (!LoadDifferentGame(player1, player2))
                		break;

                	if(IsModeVsComputer)
                		StartGameWithComputer();
					else
					    StartGame();
                	break;
                case INSTRUCTION:
                    PrintInstruction();
                    break;
                case EXIT:
                    if (Exit()) return;
                    break;
            }
        }
    }
    
    void PrintMenu() {
        cout << "\t" << NEW_GAME << " Начать новую игру\n";
        cout << "\t" << MULTIPLAYER << " Сетевая игра\n";
        cout << "\t" << CHANGE_SHIPS << " Изменить количество кораблей\n";
        cout << "\t" << RESET_SHIPS_COUNT << " Сбросить количество кораблей\n";
        cout << "\t" << CHANGE_MAX_SHIP_LEN << " Изменить максимальную длину корабля в игре\n";
        cout << "\t" << LOAD << " Загрузить игру\n";
        cout << "\t" << INSTRUCTION << " Инструкция по игре\n";
        cout << "\t" << EXIT << " Выйти\n";
    }
    
    void NewGame() {
        int choice = -1;
        
        while (true) {
            cout << "Выберите режим игры:\n";
            cout << "\t" << PLAYER_VS_PLAYER << " Человек с человеком\n";
            cout << "\t" << PLAYER_VS_COMPUTER << " Человек с компьютером\n";
            cout << "\t" << BACK_TO_MENU << " Назад в меню\n";
            
            if (!(cin >> choice) || choice < PLAYER_VS_PLAYER || choice > BACK_TO_MENU) {
                InputErrorMessage();
                continue;
            }
            
            system("cls");
            
            switch (choice) {
                case PLAYER_VS_PLAYER:
                	IsModeVsComputer = false;
                    PlayerVsPlayer();
                    return;
                case PLAYER_VS_COMPUTER:
                	IsModeVsComputer = true;
	                PlayerVsComputer();
	                return;
                case BACK_TO_MENU:
                    return;
            }
        }
    }
    
    void PlayerVsPlayer() 
	{
		player1_color = InputColorID("Первого игрока");
		player2_color = InputColorID("Второго игрока");
	
		cout << "Сейчас очередь ";
		PrintColored("Первого игрока ", player1_color);
		cout << "расставлять корабли.\n";
	
		if (Confirm("Расставить корабли вручную?")) {
			SetShipsManually(*player1);
		}
		else {
			SetShipsRandomly(*player1);
		}
	
		system("cls");
		cout << "Ваши корабли успешно установлены.\nТеперь очередь ";
		PrintColored("Второго игрока ", player2_color);
		cout << "расставлять корабли.\n";
	
		if (Confirm("Расставить корабли вручную?")) {
			SetShipsManually(*player2);
		}
		else {
			SetShipsRandomly(*player2);
		}
		
		system("cls");
        cout << "Все корабли успешно расставлены.\n";
		StartGame();
	}
    
    void ConfigureShipsCount(SeaBattleGame& game) {
        game.ResetShips();
    }
    
    void SetShipsManually(SeaBattleGame& game) {
        cout << "Режим ручной расстановки кораблей\n";
        game.DrawField();
        
        int ship_len = game.GetMaxShipLen();
        while (ship_len > 0) {
            int count = game.GetShipsRemainCountOfNLen(ship_len);
            for (int i = 0; i < count; i++) {
                system("cls");
                cout << "Расстановка " << ship_len << "-палубного корабля (" << (i+1) << "/" << count << ")\n";
                game.DrawField();
                
                if (ship_len == 1) {
                    int x, y;
                    cout << "Введите координаты для 1-палубного корабля: ";
                    while (!GetShotCoordinates(x, y) || !game.SetShip(1, 1, x, y)) {
                        InputErrorMessage();
                        cout << "Введите координаты для 1-палубного корабля: ";
                    }
                } 
				else 
				{
                    int x, y, side;
                    cout << "Введите координаты начальной точки " << ship_len << "-палубного корабля: ";
                    while (!GetShotCoordinates(x, y)) {
                        InputErrorMessage();
                        cout << "Введите координаты начальной точки " << ship_len << "-палубного корабля: ";
                    }
                    
                    int available_sides = game.AvailableSides(x, y, ship_len);
                    if (available_sides == 0) {
                        cout << "Из этой точки нельзя установить корабль. Попробуйте другую точку.\n";
                        i--;
                        continue;
                    }
                    
                    cout << "Доступные направления: ";
                    if ((available_sides / 1000) % 10) cout << "1(Вверх) ";
                    if ((available_sides / 100) % 10) cout << "2(Вправо) ";
                    if ((available_sides / 10) % 10) cout << "3(Вниз) ";
                    if (available_sides % 10) cout << "4(Влево) ";
                    cout << "\nВыберите направление: ";
                    
                    while (!(cin >> side) || side < 1 || side > 4 || game.IsSideAvailable(x, y, ship_len, side) != 0) 
					{
                        InputErrorMessage();
                        cout << "Выберите направление: ";
                    }
                    
                    if (game.SetShip(ship_len, side, x, y) != 0) {
                        cout << "Не удалось установить корабль. Попробуйте снова.\n";
                        i--;
                    }
                }
            }
            ship_len--;
        }
        
        system("cls");
        cout << "Все корабли успешно расставлены!\n";
        game.DrawField();
        system("pause");
    }
    
    void SetShipsRandomly(SeaBattleGame& game) {
        game.SetShipsRandomly(); // метод с таким же названием, но в классе SeaBattleGame
        cout << "Корабли расставлены случайным образом.\n";
        game.DrawField();
        
        if (!Confirm("Такое расположение подходит?")) {
            SetShipsRandomly(game);
        }
    }
    
    void PlayerVsComputer() 
	{
	    player1_color = InputColorID("Игрока");
	    
	    cout << "Сейчас ваша очередь расставлять корабли.\n";
	    if (Confirm("Расставить корабли вручную?")) {
	        SetShipsManually(*player1);
	    } else {
	        SetShipsRandomly(*player1);
	    }
	    
	    system("cls");
	    cout << "Ваши корабли успешно установлены.\n";
	    cout << "Компьютер расставляет свои корабли...\n";
	    
	    // Компьютер автоматически расставляет корабли
	    player2->SetShipsRandomly();
	    
	    
	    system("cls");
	    cout << "Компьютер завершил расстановку кораблей.\n";
	    cout << "Все корабли успешно расставлены.\n";
	    
	    StartGameWithComputer();
	}
	
	void StartGameWithComputer() 
	{
	    cout << "Начинается Морской бой против компьютера. Приятной игры!\n";
	    system("pause");
	    
	    bool game_over = false;
	    
	    
	    while (!game_over) {
	        if (player_turn) {
	            // Ход игрока
	            PlayerTurnVsComputer(*player1, *player2, game_over);
	        } 
			else {
	            // Ход компьютера
	            ComputerTurn(*player2, *player1, game_over);
	        }
	        
	        if (!game_over) {
	            system("pause");
	        }
	    }
	    
	    system("cls");
	    cout << "\a";
	    if (player_turn) {
	        cout << "Все корабли компьютера потоплены!\n";
	        PrintColored("Вы победили!", player1_color);
	    } else {
	        cout << "Все ваши корабли потоплены!\n";
	        PrintColored("Компьютер победил!", 7);  // Белый цвет для компьютера
	    }
	    cout << "\nСпасибо за игру!\n";
	    system("pause");
	    system("cls");
	}
	
	void PlayerTurnVsComputer(SeaBattleGame& current_player, SeaBattleBot& enemy, bool& game_over) 
	{
	    bool extra_turn = true;
	    
	    while (extra_turn && !game_over) 
		{
	        system("cls");
	        current_player.DrawFields(enemy);
	        
	        cout << "Сейчас очередь ";
			PrintColored("Первого игрока ", player1_color);
	        cout << "\n";
	        
	        cout << "Введите '0' для вызова меню или координаты для выстрела (например, A5): ";
	        
	        string input;
	        cin >> input;
	        
	        // Проверяем, не хочет ли пользователь вызвать меню
	        if (input == "0") {
	            InGameMenu(current_player, enemy, game_over);
	            continue;
	        }
	        
	        // Обрабатываем обычный ход
	        if (input.length() >= 2) {
	            char input_x = input[0];
	            int y;
	            try {
	                y = stoi(input.substr(1));
	            } catch (...) {
	                InputErrorMessage();
	                extra_turn = true;
	                continue;
	            }
	            
	            int x;
	            if (input_x >= 'A' && input_x <= 'Z') {
	                x = input_x - 'A';
	            } else if (input_x >= 'a' && input_x <= 'z') {
	                x = input_x - 'a';
	            } else {
	                InputErrorMessage();
	                extra_turn = true;
	                continue;
	            }
	            
	            if (x < 0 || x >= current_player.GetCols() || y < 0 || y >= current_player.GetRows()) {
	                InputErrorMessage();
	                extra_turn = true;
	                continue;
	            }
	            
	            int result = enemy.ShotTo(x, y);
	            
	            system("cls");
	            current_player.DrawFields(enemy);
	            
	            switch (result) {
	                case 0:
	                    cout << "Попадание! Вы получаете дополнительный ход.\n";
	                    extra_turn = true;
	                    break;
	                case 1:
	                    cout << "Корабль компьютера взорван! ";
	                    if (enemy.GetCountOfShipsRemain() == 0) {
	                        game_over = true;
	                    }
	                    cout << "Оставшееся количество кораблей компьютера: " 
	                         << enemy.GetCountOfShipsRemain() << "\n";
	                    extra_turn = true;
	                    break;
	                case 2:
	                    cout << "Промах! Теперь ход компьютера.\n";
	                    extra_turn = false;
	                    player_turn = false;  // Передаем ход компьютеру
	                    break;
	                case 3:
	                    cout << "Туда нет смысла стрелять! Попробуйте ещё раз.\n";
	                    extra_turn = true;
	                    break;
	                default:
	                    extra_turn = false;
	                    player_turn = false;
	                    break;
	            }
	            
	            if (extra_turn) {
	                cout << "Продолжайте ваш ход.\n";
	                system("pause");
	            }
	        } else {
	            InputErrorMessage();
	            extra_turn = true;
	        }
	    }
	}
	
	void ComputerTurn(SeaBattleBot& bot, SeaBattleGame& current_player, bool& game_over) 
	{
	    system("cls");
	    cout << "Ход компьютера...\n";
	    
	    // Небольшая задержка
	    Sleep(600);
	    
	    int x, y;
	    bot.ShotByBot(current_player, &x, &y);
	    
	    cout << "Компьютер стреляет в " << char('A' + x) << y << "...\n";
	    
	    int result = current_player.ShotTo(x, y); 
	    
	    // Показываем результат выстрела компьютера
	    system("cls");
	    current_player.DrawFields(bot);
	    
	    
		PrintColored("Компьютер ", player2_color);
		cout << "стрелял в " << char('A' + x) << y << endl;
	    
	    switch (result) {
	        case 0:
	            cout << "Компьютер попал в ваш корабль! Он получает дополнительный ход.\n";
	            player_turn = false;  // Компьютер продолжает ход
	            break;
	        case 1:
	            cout << "Компьютер взорвал ваш корабль! ";
	            if (current_player.GetCountOfShipsRemain() == 0) {
	                game_over = true;
	            } else {
	                cout << "Оставшееся количество ваших кораблей: " 
	                     << current_player.GetCountOfShipsRemain() << "\n";
	                player_turn = false;  // Компьютер продолжает ход
	            }
	            break;
	        case 2:
	            cout << "Компьютер промахнулся! Теперь ваша очередь.\n";
	            player_turn = true;  // Передаем ход игроку
	            break;
	        case 3:
	            cout << "Компьютер выстрелил в уже стреляную клетку. Он продолжает ход.\n";
	            player_turn = false;  // Компьютер продолжает ход
	            break;
	        default:
	            player_turn = true;
	            break;
	    }
	}
    
    void StartGame() {
        cout << "Начинается Морской бой человек против человека. Приятной игры!\n";
        system("pause");
        
        bool turn_result;
        bool game_over = false;
        while (!game_over) 
		{
            if (!player_turn) {
                cout << "\aСмена очереди хода!\n";
                cout << "Сейчас будет ходить ";
                PrintColored("Второй игрок", player2_color);
                cout << ".\n";
                system("pause");
                system("cls");
            }
            
	        if (player_turn) {
	            turn_result = PlayerTurn(*player1, *player2, "Первого игрока", game_over);
	        } else {
	            turn_result = PlayerTurn(*player2, *player1, "Второго игрока", game_over);
	        }
	        
	        if (turn_result) {
	            game_over = true;
	        } 
			else if (!game_over) { // переход хода другому игроку
	            player_turn = !player_turn;
	        }
	        
	        if (!game_over) {
            	system("pause");
        	}
        }
        
        system("cls");
        cout << "\aВсе корабли ";
        if (player_turn) {
            PrintColored("Второго игрока ", player2_color);
        } else {
            PrintColored("Первого игрока ", player1_color);
        }
        cout << "потоплены!\nПобеду одержал ";
        if (player_turn) {
            PrintColored("Первый игрок", player1_color);
        } else {
            PrintColored("Второй игрок", player2_color);
        }
        cout << ".\nСпасибо за игру!\n";
        system("pause");
        system("cls");
    }
    
    bool GetShotCoordinates(int& x, int& y) {
        char input_x;
        cout << "Введите координаты для выстрела (например, A5): ";
        
        if (!(cin >> input_x >> y)) {
            return false;
        }
        
        if (input_x >= 'A' && input_x <= 'Z') {
            x = input_x - 'A';
        } else if (input_x >= 'a' && input_x <= 'z') {
            x = input_x - 'a';
        } else {
            return false;
        }
        
        if (player1 && (x < 0 || x >= player1->GetCols() || y < 0 || y >= player1->GetRows())) {
            return false;
        }
        
        return true;
    }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    void MP_GameConfirmation(){
    	SeaNet.set_certain_message("ГОТОВНОСТЬ_УСПЕШНО");	//Что ожидать
    	
    	getch();
    	
    	SeaNet.send_message("ГОТОВНОСТЬ_УСПЕШНО");	//Отправить ожидаемое сообщение
    	SeaNet.wait_for_certain_message();	//Ждать получения ожидаемого сообщения
	}
	
	void MP_CoinFlip(){
		if (host){	//Подброс монетки
			SeaNet.set_certain_message("РЕЗУЛЬТАТ_МОНЕТКИ_ПОЛУЧЕН");	//Что ожидать
    	
	    	srand(clock());
	    	bool coin_result = (rand() % 2);
	    	
	    	SeaNet.send_message("РЕЗУЛЬТАТ_МОНЕТКИ_ОТПРАВЛЕН");	//Отправить ожидаемое сообщение
	    	SeaNet.wait_for_certain_message();	//Ждать получения ожидаемого сообщения
		}
		else{	//Ожидание результата подброса от хоста
			SeaNet.set_certain_message("РЕЗУЛЬТАТ_МОНЕТКИ_ОТПРАВЛЕН");	//Что ожидать
			
	    	getch();
	    	
	    	SeaNet.wait_for_certain_message();	//Ждать получения ожидаемого сообщения
	    	SeaNet.send_message("РЕЗУЛЬТАТ_МОНЕТКИ_ПОЛУЧЕН");	//Отправить ожидаемое сообщение
		}
	}
    
    void MP_PrepareForGame(){
    	system("cls");
    	SeaNet.set_certain_message("УСТАНОВКА_КОРАБЛЕЙ_ЗАВЕРШЕНА");	//Что ожидать
    	
    	MP_player1.field.Reset();	//Сброс всех настроек поля
    	
    	//Установка кораблей
    	if (Confirm("Расставить корабли вручную?"))
			SetShipsManually(MP_player1.field);	//Ручная установка
		else
			SetShipsRandomly(MP_player1.field);	//Случайная расстановка
    	
    	//MP_player1.field.DrawField();
    	
    	SeaNet.send_message("УСТАНОВКА_КОРАБЛЕЙ_ЗАВЕРШЕНА");	//Отправить ожидаемое сообщение
    	SeaNet.wait_for_certain_message();	//Ждать получения ожидаемого сообщения
    	
    	//MP_player2.field.RegenerateFieldByMoves();	//private
    	
    	SeaNet.wait_for_certain_message();	//Ждать получения ожидаемого сообщения
	}
    
    void waiting_points(bool *stop){
    	std::cout << "\033[?25l";	//Скрытие курсора
		int i;
		while(!(*stop)){
        	for(i = 0; i < 3 && !(*stop); i++){
        		std::this_thread::sleep_for(std::chrono::milliseconds(500));
        		std::cout << ".";
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
			
			std::cout << "\033[" << i << "D";
			for(int n = i; n > 0; n--)
				std::cout << " ";
			std::cout << "\033[" << i << "D";
		}
		std::cout << "\033[?25h";	//Отображение курсора
	}
	
	
    
    void MP_CreateLobby(std::string lobby_name = ""){
    	host = true;
    	bool flag = false;
    	std::thread thr_;
		std::cout << "Определение публичного IP";
		thr_ = std::thread([this, &flag]() { waiting_points(&flag); });
		flag = true;
		thr_.join();
		
		std::cout << "\rПубличный код друга: " << SeaNet.get_public_code() << " и IP: " << SeaNet.get_public_ip() << "\n";
        std::cout << "Локальный код друга: " << SeaNet.get_local_code() << " и IP: " << SeaNet.get_local_ip() << "\n";
        
        
        flag = false;
        std::cout << "Ожидание подключения второго игрока";
        thr_ = std::thread([this, &flag]() { waiting_points(&flag); });
		SeaNet.wait_for_connect();
		flag = true;
		thr_.join();
		
		std::cout << "\033[?25h\033[2K\r" << MP_player2.name << " подключился.\n";
	}
	
	std::string MP_InputCode(std::string message_befor_input = ""){
		std::string input;
		char input_symbol;
		bool flag = true;
		
		std::cout << message_befor_input;
		
		while (flag) {
	        input_symbol = getch();

	        if (input_symbol == 8){			//Кнопка Backspace (Стереть)
				if (input.length() > 0){
					std::cout << "\033[1D \033[1D";	//Убрать 1 символ из вывода и итоговой строки
					input.pop_back();
				}
			}
			else if (input_symbol == 13 && input.length() >= 7)	//Перевод строки (Ввод/Enter). 7 - минимальная длина кода
				break;
			else if (input.length() >= 9)	//9 - максимальная длина кода
				continue;
			else if (('0' <= input_symbol && input_symbol <= '9') || ('a' <= input_symbol && input_symbol <= 'z') || ('A' <= input_symbol && input_symbol <= 'Z')){	//Ввод символов из кодировки
				input += input_symbol;
				std::cout << input_symbol;
			}
			std::cout << "\033[s\033[1B" << input.length() << "\033[u";
		}
		return input;
	}
	
	void MP_ConnectLobby(unsigned long long lobby_id = 0, std::string player_code = ""){
		host = false;
		if (lobby_id != 0){
			
		}
		else if (player_code != ""){
			SeaNet.decode_and_use_code(player_code);
			SeaNet.handle_connect(SeaNet.remote_ip_ + " " + std::to_string(SeaNet.remote_port_));
			SeaNet.wait_for_connect();
		}
	}
	
	void MP_Statistics(unsigned long long player_id = 0){
		
	}
    
    void Multiplayer(){
    	//SeaNet.start();
    	
    	
    	//cout << "Создание сетевого\n";
	    //P2PMessenger SeaNet;
	    //SeaNet.run();
    	//SeaBattleGameNetwork SeaNet;
    	//std::cout << "Мой публичный IP: " << SeaNet.get_public_ip() << std::endl;
		//std::cout << "Мой код друга: " << SeaNet.get_my_code() << std::endl;
		
    	//cout << "Создание сетевого закончено\n";
    	
    	char display, input_symbol;
	    bool flag;
	    bool ChoseGame = false;
	    
    	while(true){
    		display = ' ', input_symbol = ' ';
    		system("cls");
    		
    		flag = true;
	    	cout	<< "Сетевая игра\n"
	        		<< "\t1. Создать игру\n"
	            	<< "\t2. Присоединиться к игре\n"
	            	<< "\t3. Статистика\n"
	            	<< "\t0. Вернуться в главное меню\n";
	        
	    	while (flag) {
	            input_symbol = getch();
	            
	            if (input_symbol == 8){			//Кнопка Backspace
	        		display = ' ';
				    std::cout << "\033[2K\r";	//Очистить строку и вернуться в начало строки
				}
	            else if (input_symbol == 27){	//Кнопка Escape == Пункту меню '0'
	        		SeaNet.stop();
				    system("cls");
				    return;
				}
	            else if ('0' <= input_symbol && input_symbol <= '3'){	//Пункты меню
		        	display = input_symbol;
		        	std::cout << "\033[2K\r" << display;
				}
				else if (input_symbol == 13)	//Перевод строки
					switch (display) {
			        	case '1':
			        		system("cls");
			        		SeaNet.start();
			        		MP_CreateLobby();
			        		//SeaNet.stop();
			        		ChoseGame = true;
			        		flag = false;
			        		break;
			            case '2':
			            	system("cls");
			            	SeaNet.start();
			            	MP_ConnectLobby(0, MP_InputCode("Введите код друга: "));
			            	//SeaNet.stop();
			            	ChoseGame = true;
			            	flag = false;
			            	break;
			            case '3':
			            	system("cls");
			            	MP_Statistics();
			            	flag = false;
			            	break;
			            case '0':
			            	system("cls");
			             	SeaNet.stop();
							return;
			        }
			}
			if (ChoseGame){
				std::cout << "Ваш соперник: " << MP_player2.name << ".\nВы готовы?";
				MP_GameConfirmation();
				MP_PrepareForGame();
				
				ChoseGame = false;
			}
		}
		
		
		std::string message;
    	std::string connect_ip = "127.0.0.1";
    	unsigned int rem_p = SeaNet.remote_port_;
		int choice = -1;
		while (true) {
            cout << "Вы находитесь в меню сетевой игры. Введите номер того действия, которое хотите выполнить.\n";
            
            cout << "Публичный код друга: " << SeaNet.get_public_code() << " и IP: " << SeaNet.get_public_ip() << "\n";
            cout << "Локальный код друга: " << SeaNet.get_local_code() << " и IP: " << SeaNet.get_local_ip() << "\n";
            cout << "0. Подключиться\n"
            	<< "1. Отправить сообщение\n"
				<< "2. К какому порту подключаться? (Текущий: " << rem_p << ")\n"
				<< "\tОжидает подключения на: (" << SeaNet.listen_port_ << ")\n"
				<< "3. Применить код друга\n" 
				<< "4. К какому IP подключаться (Текущий: " << connect_ip << ")\n"
				<< "5. SeaNet.start()\n"
				<< "6. MP_CreateLobby();\n"
				<< "7. TEST MP_ConnectLobby();\n"
				<< "8. SeaNet.stop();\n";
            
            
            if (!(cin >> choice) || choice < 0 || choice > 8) {
                InputErrorMessage();
                continue;
            }
            
            system("cls");
            
            switch (choice) {
                case 0:
		            SeaNet.handle_connect(connect_ip + " " + std::to_string(rem_p));
		            cout << "TEST Подключение\n";
                    break;
                case 1:
                    if (!(cin >> message)){
		                InputErrorMessage();
		                break;
		            }
		            SeaNet.send_message(message);
		            cout << "TEST Сообщение отправлено.\n";
                    break;
                case 2:
                    if (!(cin >> rem_p)){
		                InputErrorMessage();
		                break;
		            }
		            SeaNet.remote_port_ = rem_p;
		            cout << "TEST К какому порту подключаться изменено.\n";
                    break;
                case 3:
                	cout << "Введите код друга: ";
                	if (!(cin >> message)){
		                InputErrorMessage();
		                break;
		            }
		            
                	SeaNet.decode_and_use_code(message);
                	rem_p = SeaNet.remote_port_;
		            connect_ip = SeaNet.remote_ip_;
		            cout << "Буду подключаться к IP: " << connect_ip << ", с портом: " << rem_p << endl;
                    break;
                case 4:
                	cout << "Введите новый IP для подключения: ";
                	if (!(cin >> connect_ip)){
		                InputErrorMessage();
		                break;
		            }
                	break;
                case 5:
                	SeaNet.start();
                	std::cout << "Теперь может принять подключение.\n";
					break;
				case 6:
					MP_CreateLobby();
					break;
				case 7:
					cout << "Введите код друга: "; 
                	if (!(cin >> message)){
		                InputErrorMessage();
		                break;
		            }
		            
					MP_ConnectLobby(0, message);
					break;
				case 8:
					SeaNet.stop();
					break;
            }
        }
	}
	
	void ChangeShipsCount() 
	{
	    if (!player1) {
	        // Создаем временный объект для отображения текущих настроек
	        SeaBattleGame temp;
	        cout << "Изменение количества кораблей:\n";
	        PrintShipsCount(temp);
	    }
	    else {
	        cout << "Изменение количества кораблей:\n";
	        PrintShipsCount(*player1);
	    }
	
	    int ship_len, new_count;
	    cout << "Введите длину корабля для изменения: ";
	
	    // Используем актуальную максимальную длину из игровых объектов
	    int max_ship_len;
	    if (player1) {
	        max_ship_len = player1->GetMaxShipLen();
	    } else {
	        SeaBattleGame temp;
	        max_ship_len = temp.GetMaxShipLen();
	    }
	
	    while (!(cin >> ship_len) || ship_len < 1 || ship_len > max_ship_len) {
	        InputErrorMessage();
	        cout << "Введите длину корабля для изменения (1-" << max_ship_len << "): ";
	    }
	
	    // Получаем текущее количество
	    int current_count;
	    if (player1) {
	        current_count = player1->GetShipsRemainCountOfNLen(ship_len);
	    }
	    else {
	        SeaBattleGame temp;
	        current_count = temp.GetShipsRemainCountOfNLen(ship_len);
	    }
	
	    cout << "Текущее количество " << ship_len << "-палубных кораблей: " << current_count << "\n";
	    cout << "Введите новое количество: ";
	
	    while (!(cin >> new_count) || new_count < 0 || new_count > 9) {
	        InputErrorMessage();
	        cout << "Введите новое количество: ";
	    }
	
	    // Применяем изменения к существующим объектам
	    if (player1) player1->ChangeShipLenCount(ship_len, new_count);
	    if (player2) player2->ChangeShipLenCount(ship_len, new_count);
	
	    cout << "Количество кораблей изменено.\n";
	    if (player1) {
	        PrintShipsCount(*player1);
	    }
	    else {
	        SeaBattleGame temp;
	        PrintShipsCount(temp);
	    }
	    system("pause");
	    system("cls");
	}
    
    void ResetShipsCount() 
	{
		if (Confirm("Сбросить количество кораблей к стандартным значениям?")) {
			// Если объекты уже созданы - сбрасываем их настройки
			if (player1) player1->ResetShips();
			if (player2) player2->ResetShips();
	
			cout << "Значения сброшены к стандартным.\n";
		}
		else {
			cout << "Значения не были изменены.\n";
		}
		system("pause");
		system("cls");
	}
    
    void ChangeMaxShipLen() 
	{
	    cout << "Текущая максимальная длина корабля: " << player1->GetMaxShipLen() << "\n";
	    cout << "Введите новую максимальную длину (1-9): ";
	    
	    int new_max_len;
	    while (!(cin >> new_max_len) || new_max_len < 1 || new_max_len > 9) {
	        InputErrorMessage();
	        cout << "Введите новую максимальную длину (1-9): ";
	    }
	    
	    if (Confirm("Сохранить изменение? Все текущие настройки кораблей будут сброшены.")) {
	        // Применяем изменения
	        player1->ChangeMaxShipLen(new_max_len);
	        player2->ChangeMaxShipLen(new_max_len);
	        
	    cout << "Максимальная длина изменена на " << new_max_len << ".\n";
	    } 
		else 
	        cout << "Изменения отменены.\n";
	    
	    
	    system("pause");
	    system("cls");
	}
    
    void PrintInstruction() {
        cout << "///////////////////////////////////ИНСТРУКЦИЯ///////////////////////////////////\n\n";
        cout << "Игра 'Морской бой'\n\n";
        cout << "Цель игры: первым уничтожить все корабли противника.\n\n";
        cout << "Правила:\n";
        cout << "- Игровое поле размером 10x10 клеток\n";
        cout << "- Корабли расставляются вертикально или горизонтально\n";
        cout << "- Корабли не могут соприкасаться друг с другом\n";
        cout << "- При попадании в корабль дается дополнительный ход\n";
        cout << "- При уничтожении корабля помечаются клетки вокруг него\n\n";
        cout << "Символы на поле:\n";
        cout << "'-' - пустая клетка\n";
        cout << "'*' - промах\n"; 
        cout << "'X' - раненый корабль\n";
        cout << "'#' - уничтоженный корабль\n";
        cout << "'S' - ваш корабль (виден только на вашем поле)\n\n";
        cout << "Управление:\n";
        cout << "- Для выстрела вводите букву и цифру (например: A5)\n";
        cout << "- Буквы от A до J - горизонтальные координаты\n";
        cout << "- Цифры от 0 до 9 - вертикальные координаты\n";
        cout << "////////////////////////////////КОНЕЦ ИНСТРУКЦИИ////////////////////////////////\n\n";
        system("pause");
        system("cls");
    }
    
    bool Exit() {
        if (Confirm("Вы точно хотите выйти?")) {
            return true;
        }
        system("cls");
        return false;
    }
    
    void InputErrorMessage() {
        system("cls");
        cout << "Неправильный ввод! Попробуйте ещё раз.\n\n";
        cin.clear();
        cin.ignore(10000, '\n');
    }
    
    bool Confirm(const string& message) {
        int choice;
        while (true) {
            cout << message << "\n'0' - если да.\n'1' - если нет.\n";
            if (cin >> choice && (choice == 0 || choice == 1)) {
                return choice == 0;
            }
            InputErrorMessage();
        }
    }
    
    void PrintColored(const string& text, int color) 
	{
	    cout << "\033[1;3" << color << "m" << text << "\033[0m";
	}
    
    void PrintAvailableColors() {    //вывести доступные цвета (ColorID)
                                //ColorID:
    PrintColored("0 — Серый\n", 0);        //0
    PrintColored("1 — Красный\n", 1);      //1
    PrintColored("2 — Зеленый\n", 2);      //2 
    PrintColored("3 — Желтый\n", 3);       //3
    PrintColored("4 — Синий\n", 4);        //4
    PrintColored("5 — Фиолетовый\n", 5);   //5
    PrintColored("6 — Голубой\n", 6);      //6
    PrintColored("7 — Белый\n", 7);        //7
}

int InputColorID(const string& player_name) {
    int color;
    while (true) {
        cout << "Выбор цвета для " << player_name << ":\n";
        PrintAvailableColors();
        cout << "Введите код цвета (0-7): ";

        if (!(cin >> color) || color < 0 || color > 7) {
            InputErrorMessage();
            continue;
        }

        cout << "Пример отображения: ";
        PrintColored(player_name, color);
        cout << "\n";

        if (Confirm("Сохранить этот цвет?")) {
            return color;
        }
        system("cls");
    }
}
    
    void PrintShipsCount(SeaBattleGame& game) 
	{
		cout << "Кораблей в игре\n";
		for (int i = game.GetMaxShipLen(); i >= 1; i--) {
			int count = game.GetShipsRemainCountOfNLen(i);
			if (count > 0) {
				cout << "  " << count << " x " << i << "-палубных\n";
			}
		}
		cout << "\n";
	}
    
private:
    void InGameMenu(SeaBattleGame& current_player, SeaBattleBot& enemy, bool& game_over) 
	{
        int choice = -1;
        
        while (true) {
            system("cls");
            current_player.DrawFields(enemy);
            
            cout << "\n=== ПОДМЕНЮ ИГРЫ ===\n";
            cout << "1 - Продолжить игру\n";
            cout << "2 - Сохранить игру\n";
            cout << "3 - Загрузить другую игру\n";
            cout << "4 - Выйти в главное меню\n";
            cout << "5 - Отменить ход\n";
            cout << "6 - Завершить программу\n";
            cout << "Выберите действие: ";
            
            if (!(cin >> choice) || choice < 1 || choice > 6) {
                InputErrorMessage();
                continue;
            }
            
            switch (choice) {
                case 1:
                    // Продолжить игру
                    return;
                    
                case 2:
                    // Сохранить игру
                    if (SaveCurrentGame(current_player, enemy))
					{
                    	game_over = false;
                    	return;
					}
                    break;
                    
                case 3:
                    // Загрузить другую игру
                    if (LoadDifferentGame(&current_player, &enemy)) {
                        game_over = false;
                        return;
                    }
                    break;
                    
                case 4:
                    // Выйти в главное меню
                    if (Confirm("Вы точно хотите выйти в главное меню? Текущий прогресс будет потерян.")) {
                        game_over = true;
                        return;
                    }
                    break;

                case 5:
                	// Отменить ход
                	choice = player2->CheckLastMove();
                	if (!(choice == 2 || choice == 3)){
                		choice = player1->CancelLastMove();
                		do{
                			choice = player1->CancelLastMove();
						}
	                	while (choice == 2 || choice == 3);	//У бота был ход, дающий дополнительный ход
					}
                	player2->CancelLastMove();
                	return;
                	break;
                
                case 6:
                    // Завершить программу
                    if (Confirm("Вы точно хотите завершить программу?")) {
                        exit(0);
                    }
                    break;
            }
        }
    }
    bool RecordString(ofstream &flow_name, string your_string){	//Записать в файл строку; true - успешно; false - ошибка записи
		return (flow_name << your_string).good();
	}
	bool RecordElement(ofstream &flow_name, string element_name, int number){	//Записать в файл элемент в кавычки; true - успешно; false - ошибка записи
		return (flow_name << element_name << "=\"" << number << "\"\n").good();
	}
    bool SaveCurrentGame(SeaBattleGame& player1, SeaBattleBot& player2) 
	{
        string filename;
        cout << "Введите имя файла для сохранения (без пробелов): ";
        cin >> filename;
        
        // Добавляем расширение если нужно
        if (filename.length() < 4 || filename.substr(filename.length() - 4) != ".txt") 
		{
		    filename += ".txt";
		}
		ofstream testopen_file(filename, ios::app);
        if (!testopen_file.is_open()) {
            testopen_file.close();
            cout << "Не удалось открыть файл!" << endl;
            return false;
        }
        testopen_file.close();
        
        if(player_turn)
        {
        	// Открываем файл для записи (перезаписываем содержимое)
	        if (player1.SaveToFile(filename) || player2.SaveToFile(filename, true))
			{
				cout << "Ошибка при сохранении игры!" << endl;
		       	return false;
			}
		}
		else
		{
			if (player2.SaveToFile(filename) || player1.SaveToFile(filename, true))
			{
				cout << "Ошибка при сохранении игры!" << endl;
		       	return false;
			}
		}
		
        if (IsModeVsComputer)
        	if (player2.BotSaveToFile(filename)){
        		cout << "Ошибка при сохранении игры!" << endl;
				return false;
			}

		ofstream info_file(filename, ios::app);
        if (!info_file.is_open()) {
            info_file.close();
            cout << "Не удалось открыть файл!" << endl;
            return false;
        }
		if (
			!RecordString(info_file, "Menu\n") ||
			!RecordElement(info_file, "mt", ((int)player_turn)) ||
			!RecordElement(info_file, "pc1", player1_color) ||
			!RecordElement(info_file, "pc2", player2_color)
			){
				cout << "Ошибка при сохранении игры!" << endl;
				return false;
			}
		
		
        cout << "Игра успешно сохранена в файл: " << filename << endl;
        info_file.close();
        system("pause");
        return true;
    }
    
	bool ReadFindString(ifstream &flow_name, string your_string){	//Найти строку в файле; true - удалось; false - не удалось
		string str;
		while (getline(flow_name, str)){
			if (flow_name.fail() && !flow_name.eof())
                return false;
            if (str.find(your_string) != string::npos)
                return true;
        }
    	return false;
	}
	bool ReadIsCharEqualsReaded(ifstream &flow_name, char your_char){	//Равен ли считанный символ переданному; true - равны; false - не равны
		return (flow_name.get() == your_char);
	}
	bool ReadIsStringEquals(ifstream &flow_name, string your_string){	//Совпадает ли ожидаемое имя элемента со считанным; true - совпадает; false - не совпадает
		for(int i = 0; i < your_string.length(); i++){
			if (your_string[i] != flow_name.get())
				return false;
		}
		return true;
	}
	bool ReadElement(ifstream &flow_name, string element_name, int *element){	//Считать из файла элемент в кавычках; true - успешно; false - ошибка чтения
		if (!ReadIsStringEquals(flow_name, element_name + "=\"") || !ReadNumber(flow_name, element) || !ReadIsCharEqualsReaded(flow_name, '"') || !ReadIsCharEqualsReaded(flow_name, '\n'))
			return false;
		return true;
	}
	bool ReadNumber(ifstream &flow_name, int *element){	//Считать из файла элемент в кавычках; true - успешно; false - ошибка чтения
		return (flow_name >> *element).good();
	}
	
    bool LoadDifferentGame(SeaBattleGame* player1, SeaBattleBot* player2) {
        string filename;
        cout << "Введите имя файла для загрузки: ";
        cin >> filename;
        player1->Reset();
        player2->Reset();
        player2->ResetBot();
        
        // Добавляем расширение если нужно
        if (filename.find(".txt") == string::npos) {
            filename += ".txt";
        }
        ifstream testopen_file(filename);
        if (!testopen_file.is_open()) {
            testopen_file.close();
            cout << "Не удалось открыть файл!" << endl;
            return false;
        }
        testopen_file.close();
        
        if (player1->LoadFromFile(filename) || player2->LoadFromFile(filename, true)){
        	cout << "Ошибка при загрузке игры!" << endl;
            system("pause");
			return false;
		}
		
		ifstream bot_file(filename);
        if (!bot_file.is_open()) {
            bot_file.close();
            cout << "Не удалось открыть файл!" << endl;
            return false;
        }
        if (!ReadFindString(bot_file, "SeaBattleBot"))
			IsModeVsComputer = false;	//Не нашёл
		else
			IsModeVsComputer = true;
		bot_file.close();
		
		if (IsModeVsComputer)
		{
			if(player2->BotLoadFromFile(filename))
			{
				cout << "Ошибка при загрузке игры!" << endl;
            	system("pause");
				return false;
			}
		}
		
		 int temp;
		ifstream info_file(filename);
        if (!info_file.is_open()) {
            info_file.close();
            cout << "Не удалось открыть файл!" << endl;
            return false;
        }
		if (
			!ReadFindString(info_file, "Menu") ||
			!ReadElement(info_file, "mt", &temp) ||
			!ReadElement(info_file, "pc1", &player1_color) ||
			!ReadElement(info_file, "pc2", &player2_color)
			){
				cout << "Ошибка при загрузке игры!" << endl;
            	system("pause");
				return false;
			}
			
		player_turn = (bool)temp;
		
			
        cout << "Игра успешно загружена из файла: " << filename << endl;
        return true;
    }

	bool PlayerTurn(SeaBattleGame& current_player, SeaBattleGame& enemy, const string& player_name, bool& game_over) 
	{
	    bool extra_turn = true;
	    
	    while (extra_turn && !game_over) 
		{
	        system("cls");
	    
	    	if(extra_turn)
	        	current_player.DrawFields(enemy);
	        else
	        	enemy.DrawFields(current_player);
	        
	        
	        cout << "Ход ";
	        if(player_turn) // т.е. первый игрок
	        {
	        	PrintColored(player_name, player1_color);
			}
			else{
				PrintColored(player_name, player2_color); // иначе второй игрок
			}
	        cout << "\n";
	        
	      
	        cout << "Введите '0' для вызова меню или координаты для выстрела (например, A5): ";
	        
	        string input;
	        cin >> input;
	        
	        // Проверяем, не хочет ли пользователь вызвать меню
	        if (input == "0") {
	            InGameMenu(current_player, static_cast<SeaBattleBot&> (enemy), game_over);
	            if (game_over){
	            	system("pause");
	            	return false;
				} 
	            continue;
	        }
	        
	        // Обрабатываем обычный ход
	        if (input.length() >= 2) 
			{
	            char input_x = input[0];
	            int y;
	            try {
	                y = stoi(input.substr(1));
	            } catch (...) {
	                InputErrorMessage();
	                extra_turn = true;
	                continue;
	            }
	            
	            int x;
	            if (input_x >= 'A' && input_x <= 'Z') {
	                x = input_x - 'A';
	            } else if (input_x >= 'a' && input_x <= 'z') {
	                x = input_x - 'a';
	            } else {
	                InputErrorMessage();
	                extra_turn = true;
	                continue;
	            }
	            
	            if (x < 0 || x >= current_player.GetCols() || y < 0 || y >= current_player.GetRows()) {
	                InputErrorMessage();
	                extra_turn = true;
	                continue;
	            }
	            
	            int result = enemy.ShotTo(x, y);
	            
	            system("cls");
	            
	            switch (result) 
				{
	                case 0:
	                    cout << "Попадание! Вы получаете дополнительный ход.\n";
	                    extra_turn = true;
	                    break;
	                case 1:
	                    cout << "Корабль взорван! ";
	                    if (enemy.GetCountOfShipsRemain() == 0) {
	                        return true;
	                    }
	                    cout << "Оставшееся количество вражеских кораблей: " 
	                         << enemy.GetCountOfShipsRemain() << "\n";
	                    extra_turn = true;
	                    break;
	                case 2:
	                    cout << "Промах!\n";
	                    extra_turn = false;
	                    break;
	                case 3:
	                    cout << "Туда нет смысла стрелять!\n";
	                    extra_turn = true;
	                    break;
	                default:
	                    extra_turn = false;
	                    break;
	            }
	            
	            if (extra_turn) {
	                cout << "Продолжайте ваш ход.\n";
	                system("pause");
	            }
	        } 
			else 
			{
	            InputErrorMessage();
	            extra_turn = true;
	        }
	    }
	    
	    return false;
	}
};








int main() {
	system("TITLE SeaBattle");	//Переименовать окно консоли в "SeaBattle"
	system("CHCP 1251");	//Для русского языка в консоли
	system("cls");
	//setlocale(LC_ALL, "Rus");
	//std::thread th1(func, 1);

	
	//AnotherClass ancl;
	SeaBattleGameMenu menu;
	//SeaBattleGameNetwork netw;
	//std::thread th1([&netw]() { netw.network_check_run(); });
	//cout << "(После создания сетевого объекта)\n";
	cout << "test22";
    menu.Run();
    //std::thread th2(menu.Run());	//Не будет работать
    //std::thread th2([&menu]() { menu.Run(); });	//Будет работать
    
	//th1.join(); //Основной поток будет здесь ожидать завершения выполнения потока th1 
	//mainggg();
	return 0;
}
/*
int main() {
	setlocale(LC_ALL, "Rus");
	
	SeaBattleGameMenu menu;
    menu.Run();
    
	return 0;
}
*/
