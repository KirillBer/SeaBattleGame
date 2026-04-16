#include <iostream>
#include <conio.h>



//#include <locale>












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
	bool MyMoveTurn;
	Player MP_player1;
    Player MP_player2;
    P2PMessenger SeaNet;
    std::thread thread_;
    
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
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    bool MP_GameConfirmation(){	//Подтверждение готовности; Возвращает "готов": true - да, false - нет
    	//std::cout << "Ваш соперник: " << MP_player2.name << ".\nВы готовы?";
		
		//getch();	//Обработка для получения согласия или отказа от игры
    	
    	SeaNet.send_service("ГОТОВНОСТЬ_УСПЕШНО");	//Отправить ожидаемое сообщение
    	SeaNet.wait_for_certain_service_message("ГОТОВНОСТЬ_УСПЕШНО");	//Ждать получения сообщения
    	return true;
	}
	
    void MP_SettingShips(){	//Расстановка кораблей для игры
    	system("cls");
    	
    	MP_player1.field.Reset();	//Сброс изменений поля
    	//MP_player2.field = MP_player1.field;
    	
    	
    	//Установка кораблей
    	if (Confirm("Расставить корабли вручную?")) 
			SetShipsManually(MP_player1.field);	//Ручная установка
		else
			SetShipsRandomly(MP_player1.field);	//Случайная расстановка
    	
    	
    	//system("pause");
    	
    	
    	//MP_player1.field.DrawField();
    	//std::string temp = MP_player1.field.ToString();
    	//std::cout << "ToString: '" << temp << "'\n";
    	
    	//std::cout << "Итог: " << MP_player2.field.FromString(temp) << std::endl;
    	//MP_player2.field.DrawField();
    	
    	//std::cout << "MovesSize: " << MP_player1.field.GetMovesSize() << std::endl;
    	//for(int i = 0; i < MP_player1.field.GetMovesSize(); i++)
    	//	std::cout << "i=" << i << " " << MP_player1.field.MoveToString(i) << std::endl;
    	
    	SeaNet.send_service("УСТАНОВКА_КОРАБЛЕЙ_ЗАВЕРШЕНА");	//Отправить ожидаемое сообщение
    	SeaNet.wait_for_certain_service_message("УСТАНОВКА_КОРАБЛЕЙ_ЗАВЕРШЕНА");	//Ждать получения ожидаемого сообщения
	}
	
	void MP_FieldsSynchronization(bool first_time = true){	//Синхронизация полей
		if (first_time){
			//std::cout << "СИНХРОНИЗАЦИЯ начало\n";
			SeaNet.send_data(MP_player1.field.ToString());
			//std::cout << "СИНХРОНИЗАЦИЯ отправлено\n";
			while(SeaNet.data.size() == 0)
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			MP_player2.field.FromString(SeaNet.data.pop_back());
			//std::cout << "Итог восстановления из строки: "<<  MP_player2.field.FromString(SeaNet.data.pop_back()) << std::endl;
		}
		else{
			
			
		}
			
	}
	
	bool MP_CoinFlip(){	//Жеребьёвка. Возвращает "Моя очередь ходить первым?": true - да, false - нет
		bool coin_result, flag = false;
		
	    waiting(&flag, "Ожидание второго игрока");
	    
		if (host){	//Выполнение подброса монетки
    	
	    	srand(clock());
	    	coin_result = (rand() % 2);
	    	
	    	MyMoveTurn = coin_result;
	    	player_turn = MyMoveTurn;	//Костыль для MP_Game
	    	
	    	SeaNet.send_data(std::to_string(coin_result));
	    	
	    	SeaNet.send_service("РЕЗУЛЬТАТ_МОНЕТКИ_ОТПРАВЛЕН");	//Отправить ожидаемое сообщение
	    	
	    	
	    	SeaNet.wait_for_certain_service_message("РЕЗУЛЬТАТ_МОНЕТКИ_ПОЛУЧЕН");	//Ждать получения ожидаемого сообщения
		}
		else{	//Ожидание результата подброса от хоста
	    	
	    	SeaNet.wait_for_certain_service_message("РЕЗУЛЬТАТ_МОНЕТКИ_ОТПРАВЛЕН");	//Ждать получения ожидаемого сообщения
	    	
			std::string text = SeaNet.data.pop_back();
			coin_result = StrToInt(text);
	    	MyMoveTurn = !coin_result;
	    	player_turn = MyMoveTurn;	//Костыль для MP_Game
	    	
	    	SeaNet.send_service("РЕЗУЛЬТАТ_МОНЕТКИ_ПОЛУЧЕН");	//Отправить ожидаемое сообщение
		}
		flag = true;	//Завершить анимацию ожидания
		waiting_join();
		
		std::cout << "\nРезультат жеребьёвки:\n\tПервым ходить будет" << (MyMoveTurn ? std::string("е Вы\n") : (std::string(" ") + MP_player2.name + std::string("\n")));
	}
	
	void MP_DrawFields(){
		MP_player1.field.DrawFields(MP_player2.field);
	}
	
	bool MP_MoveTurn(bool& game_over) 
	{
	    bool extra_turn = true;
	    
	    while (extra_turn && !game_over) 
		{
	        system("cls");
	    
	    	//if(extra_turn)
	        //	current_player.DrawFields(enemy);
	        //else
	        //	enemy.DrawFields(current_player);
	        MP_DrawFields();
	        
	        
	        if(player_turn) // т.е. первый игрок
	        {
	        	cout << "Ваш ход.\n";
			}
			else{			// иначе второй игрок
				cout << "Ходит " << MP_player2.name << ".\n";
			}
	        
	        
			if (player_turn){
	        	cout << "Введите '0' для вызова меню, либо координаты для выстрела (например, A5): ";
	        }
	        else{
	        	cout << "Для вызова меню введите '0'\n";
			}
			
	        string input;
	        if (player_turn)
	        	cin >> input;
	        
	        // Проверяем, не хочет ли пользователь вызвать меню
	        /*
	        if (input == "0") {
	            InGameMenu(current_player, static_cast<SeaBattleBot&> (enemy), game_over);
	            if (game_over){
	            	system("pause");
	            	return false;
				} 
	            continue;
	        }
	        */
	        

	        // Обрабатываем обычный ход
	        int x, y, result;
	        if (input.length() >= 2 || !player_turn) 
			{
				if (player_turn){
					char input_x = input[0];
	            
		            try {
		                y = stoi(input.substr(1));
		            } catch (...) {
		                InputErrorMessage();
		                extra_turn = true;
		                continue;
		            }
		            
		            if (input_x >= 'A' && input_x <= 'Z') {
		                x = input_x - 'A';
		            } else if (input_x >= 'a' && input_x <= 'z') {
		                x = input_x - 'a';
		            } else {
		                InputErrorMessage();
		                extra_turn = true;
		                continue;
		            }
		            
		            if (x < 0 || x >= (player_turn ? MP_player2.field.GetCols() : MP_player1.field.GetCols()) || y < 0 || y >= (player_turn ? MP_player2.field.GetRows() : MP_player1.field.GetRows())) {
		                InputErrorMessage();
		                extra_turn = true;
		                continue;
		            }
		            result = MP_ShotTo(x, y);
				}
				else{
					result = MP_WaitShotResult();
				}
	            
	            //int result = enemy.ShotTo(x, y);
	            system("cls");
	            
	            switch (result) 
				{
	                case 0:
	                	if (player_turn)
	                    	cout << "Попадание! Вы получаете дополнительный ход.\n";
	                    else
	                    	cout << "По вашему кораблю попали!\n";
	                    	//cout << "По вашему кораблю попали! " << MP_player2.name << " получил дополнительный ход.\n";
	                    
	                    extra_turn = true;
	                    break;
	                case 1:
	                	if (player_turn)
	                    	cout << "Вражеский";
						else
							cout << "Ваш";
						cout << " корабль взорван! ";
						
						cout << "Оставшееся количество ";
						if (player_turn){
							int ships_remain_count = MP_player2.field.GetCountOfShipsRemain();
							if (ships_remain_count == 0)
								return true;
							cout << "вражеских кораблей: " << ships_remain_count << "\n";
						}
						else{
							int ships_remain_count = MP_player1.field.GetCountOfShipsRemain();
							if (ships_remain_count == 0)
								return true;
							cout << "ваших кораблей: " << ships_remain_count << "\n";
						}

	                    extra_turn = true;
	                    break;
	                case 2:
	                	if (player_turn)
	                		cout << "Промах!\n";
	                	else
	                		cout << MP_player2.name << " промахнулся!\n";
	                	
	                    extra_turn = false;
	                    break;
	                case 3:
	                	if (player_turn)
	                		cout << "Туда нет смысла стрелять!\n";
	                	else
	                		cout << MP_player2.name << " сделал бессмысленный ход!\n";
	                	
	                    extra_turn = true;
	                    break;
	                default:
	                    extra_turn = false;
	                    break;
	            }
	            
	            if (extra_turn) {
	            	if (player_turn)
	                	cout << "Продолжайте ваш ход.\n";
	                else
	                	cout << "Соперник получил дополнительный ход.\n";
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
	
	int MP_ShotTo(int x, int y){
		int result;
		result = MP_player2.field.ShotTo(x, y);
		//ShotTo возвращает: 0 - попадание по SHIP; 1 - подрыв последнего корабля; 2 - выстрел в пустую клетку;
		//3 - бессмысленный выстрел, попадание в SHOT, STRIKE, KILL; 4 - ошибка координаты; 5 - ошибка значения ячейки
		SeaNet.send_data(std::to_string(x) + " " + std::to_string(y));
		SeaNet.send_service("ДАННЫЕ_ХОДА_ОТПРАВЛЕНЫ");
		SeaNet.wait_for_certain_service_message("ДАННЫЕ_ХОДА_ПОЛУЧЕНЫ");
		return result;
	}
	
	int MP_WaitShotResult(){
		SeaNet.wait_for_certain_service_message("ДАННЫЕ_ХОДА_ОТПРАВЛЕНЫ");
		while(SeaNet.data.size() == 0);
		SeaNet.send_service("ДАННЫЕ_ХОДА_ПОЛУЧЕНЫ");
		std::string shot_message = SeaNet.data.pop_back();
		int x = StrToInt(shot_message);
		int y = StrToInt(shot_message);
		return MP_player1.field.ShotTo(x, y);
	    //SeaNet.send_service("РЕЗУЛЬТАТ_МОНЕТКИ_ПОЛУЧЕН");
	}
	
	void MP_Game(){		//Обработка сетевой игры
		cout << "Игра начинается. Приятной игры!\n";
        
        bool turn_result;
        bool game_over = false;
        while (!game_over) 
		{
            //if (player_turn) 
            //    cout << "Сейчас ваша очередь ходить.\n";
            //else
            //	cout << "Сейчас очередь ходить " << MP_player2.name << ".\n";
            //system("pause");
            
	        if (player_turn) {
	            turn_result = MP_MoveTurn(game_over); //turn_result = PlayerTurn(MP_player1.field, MP_player2.field, "Ваш", game_over);
	        } else {
	            turn_result = MP_MoveTurn(game_over); //turn_result = PlayerTurn(MP_player2.field, MP_player1.field, MP_player2.name, game_over);
	        }
	        
	        if (turn_result) {
	            game_over = true;
	        } 
			else if (!game_over) { // переход хода другому игроку
	            player_turn = !player_turn;
	        }
	        
	        system("cls");
	        if (!game_over) {
	        	cout << "\aСмена очереди хода.\n";
            	system("pause");
        	}
        }
        
        
        
        if (player_turn) {
        	cout << "\aВсе корабли ";
        	std::cout << MP_player2.name << " ";
        } else {
            std::cout << "\aВсе Ваши корабли ";
        }
        cout << "потоплены!\nПобеду одержал";
        if (player_turn) {
            std::cout << "и Вы";
        } else {
            std::cout << " " << MP_player2.name;
        }
        cout << ".\nСпасибо за игру!\n";
        system("pause");
        system("cls");
	}
	
	bool MP_GameEnded(){	//Опрос о дальнейшем действии пользователя по завершении сетевой игры (реванш или выйти); true - реванш; false - выйти
		return false;
	}
	
	void MP_GameStatesSwitch(bool have_an_unfinished_game = false){
		typedef enum MP_GameStates_{
			//Стандартные стадии начала игры
			GameAccepting,
			SettingShips,
			FirstSynchronization,
			CoinFlip,
			GameStart,
			GameEnded,
			//Специфичные
			ReconnectTry
		}MP_GameStates_;
		
		int temp;
		MP_GameStates_ state = (have_an_unfinished_game ? ReconnectTry : GameAccepting);
		
		while(true){
			switch(state){
				//Стандартные
				case GameAccepting:			//Подтверждение готовности начать игру
					if (MP_GameConfirmation())
						state = SettingShips;	//Готов
					else
						return;					//Не готов
					break;
				
				case SettingShips:			//Расстановка кораблей
					MP_SettingShips();
					
					state = FirstSynchronization;
					break;
				
				case FirstSynchronization:	//Получение чужого и отправка своего поля для игры
					MP_FieldsSynchronization();
					
					state = CoinFlip;
					break;
				
				case CoinFlip:				//Жеребьёвка
					MP_CoinFlip();
					
					state = GameStart;
					break;
				
				case GameStart:				//Отправка в процесс игры
					MP_Game();
					std::cout << "MP_GAME\n";
					state = GameEnded;
					break;
				
				case GameEnded:				//Отправка в процесс игры
					temp = MP_GameEnded();
					switch(temp){	//Что выбрал(и) игрок(и)
						case 0:	//Завершение игры
							return;
						
						case 1:	//Повторная игра
							state = SettingShips;
							break;
					}
					break;
				
				//Специфичные ситуации
				case ReconnectTry:			//Если процесс был завершён во время идущей игры
					
					break;
				
			}
		}
	}
	
	void MP_DrawFrame(unsigned int weight, unsigned int height, bool overwrite){
		if (weight < 2 || height < 2)
			return;
		
		CMDDrawLine(true, false, true, true, false);
    	for(int i = 0; i < (weight - 2); i++)
    		CMDDrawLine(true, false, true, false, true);
    	CMDDrawLine(true, false, false, true, true);
    	std::cout << std::endl;
    	
    	for(int n = 0; n < (height - 2); n++){
    		CMDDrawLine(true, true, false, true, false);
	    	if (overwrite){
	    		for(int i = 0; i < (weight - 2); i++)
	    			std::cout << " ";
			}
	    	else
    			CMDCursorMoveRight(true, (weight - 2));
	    	CMDDrawLine(true, true, false, true, false);
	    	//Вместо std::cout << std::endl;
			CMDCursorMoveLeft(true, weight);
	    	CMDCursorMoveBottom();
		}
    	
    	CMDDrawLine(true, true, true, false, false);
    	for(int i = 0; i < (weight - 2); i++)
    		CMDDrawLine(true, false, true, false, true);
    	CMDDrawLine(true, true, false, false, true);
    	//Вместо std::cout << std::endl;
		CMDCursorMoveLeft(true, weight);
	    CMDCursorMoveBottom();
	}
	
	void waiting(bool *stop, std::string text_before_waiting_points = ""){	//Отрисовка трёх точек ожидания в потоке thread_ пока флаг stop = false
		thread_ = std::thread([this, stop, text_before_waiting_points]() { waiting_points(stop, text_before_waiting_points); });
	}
	void waiting_join(){	//Остановить выполнение потока, пока не завершится обработка точек из waiting
		thread_.join();
	}
    void waiting_points(bool *stop, std::string text_before_waiting_points = ""){
    	CMDCursorOFF();	//Скрытие курсора
		int i;
		std::cout << text_before_waiting_points;
		while(!(*stop)){
        	for(i = 0; i < 3 && !(*stop); i++){
        		std::this_thread::sleep_for(std::chrono::milliseconds(500));
        		std::cout << ".";
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
			CMDRemNSymbols(true, i);
			//std::cout << "\033[" << i << "D";
			//for(int n = i; n > 0; n--)
			//	std::cout << " ";
			//std::cout << "\033[" << i << "D";
		}
		CMDCursorON();	//Отображение курсора
	}
	
	const int StrToInt(std::string& text, bool change_string = true){	//Вернуть первое число из начала строки (1 в 1 из SeaBattleField)
		size_t pos = text.find(' ');
		int temp;
		
		try{
			temp = std::stoi(text.substr(0, (pos == SIZE_MAX ? text.length() : pos)));
		}
		catch(...){
			throw std::string("String to int error");	//Чтобы вместо Ошибка="stoi" выбрасывать string="текст ошибки"
		}
		if (change_string)
			text = text.substr((pos == SIZE_MAX ? 0 : (pos + 1)), text.length());
		
		return temp;
	}
	//Функции Escape-последовательностей
	std::string EscapeCodeTemplate(bool out_to_cout_once, std::string code_text){
		if (out_to_cout_once){
			std::cout << code_text;
			return "";
		}
		return code_text;
	}
	std::string CMDStrClear(bool out_to_cout_once = true, bool set_cursor_at_string_start = true){	//Очистка строки, на которой находится курсор
		return EscapeCodeTemplate(out_to_cout_once, (set_cursor_at_string_start ? "\033[2K\r" : "\033[2K"));	//\033[2K - перезаписать все символы в строке пробелами; \r - перевести курсор в начало строки
	}
	std::string CMDCursorON(bool out_to_cout_once = true){	//Включение мигания курсора
		return EscapeCodeTemplate(out_to_cout_once, "\033[?25h");	//\033[?25h - включить мигание курсора
	}
	std::string CMDCursorOFF(bool out_to_cout_once = true){	//Выключение мигания курсора	
		return EscapeCodeTemplate(out_to_cout_once, "\033[?25l");	//\033[?25l - выключить мигание курсора
	}
	std::string CMDRemNSymbols(bool out_to_cout_once = true, int symbols_count_for_removing = 1){	//Удалить symbols_count_for_removing символов слева от курсора в текущей строке
		std::string code_text = "\033[" + std::to_string(symbols_count_for_removing) + "D", spaces_for_add = "";	//\033[{N}D - сдвиг курсора на N символов влево
		for(int i = 0; i < symbols_count_for_removing; i++)
			spaces_for_add += ' ';
		
		return EscapeCodeTemplate(out_to_cout_once, (code_text + spaces_for_add + code_text));
	}
	std::string CMDCursorMoveTop(bool out_to_cout_once = true, unsigned int symbols_count_for_move = 1){
		return EscapeCodeTemplate(out_to_cout_once, "\033[" + std::to_string(symbols_count_for_move) + "A");
	}
	std::string CMDCursorMoveRight(bool out_to_cout_once = true, unsigned int symbols_count_for_move = 1){
		return EscapeCodeTemplate(out_to_cout_once, "\033[" + std::to_string(symbols_count_for_move) + "C");
	}
	std::string CMDCursorMoveBottom(bool out_to_cout_once = true, unsigned int symbols_count_for_move = 1){
		return EscapeCodeTemplate(out_to_cout_once, "\033[" + std::to_string(symbols_count_for_move) + "B");
	}
	std::string CMDCursorMoveLeft(bool out_to_cout_once = true, unsigned int symbols_count_for_move = 1){
		return EscapeCodeTemplate(out_to_cout_once, "\033[" + std::to_string(symbols_count_for_move) + "D");
	}
	std::string CMDDrawLine(bool out_to_cout_once = true, bool top = false, bool right = false, bool bottom = false, bool left = false){
		//	Графические линии (все под прямым углом)
		//	Для отображения необходимо вывести: \033(0W,
		//		где W - код символа
		//
		//	Код | Направление, в котором смотрит линия
		//------+--------------------------------------
		//	j	|	Верх, 		,	 , Лево 					\033(0j
		//	k	|				, Низ, Лево						\033(0k
		//	l	|		, Право , Низ							\033(0l
		//	m	|	Верх, Право									\033(0m
		//	n	|	Верх, Право , Низ, Лево						\033(0n
		//	q	|		, Право , 	   Лево						\033(0q
		//	t	|	Верх, Право , Низ							\033(0t
		//	u	|	Верх, 		, Низ, Лево						\033(0u
		//	v	|	Верх, Право	, 	 , Лево						\033(0v
		//	w	|		, Право	, Низ, Лево 					\033(0w
		//	x	|	Верх, 		, Низ							\033(0x
		//------+--------------------------------------
		/*
		std::string result;
			 if (top 	&& !right 	&& !bottom 	&& left)
			result = "\033(0j";
		else if (!top 	&& !right 	&& bottom 	&& left)
			result = "\033(0k";
		else if (!top 	&& right 	&& bottom 	&& !left)
			result = "\033(0l";
		else if (top 	&& right 	&& !bottom 	&& !left)
			result = "\033(0m";
		else if (top 	&& right 	&& bottom 	&& left)
			result = "\033(0n";
		else if (!top 	&& right 	&& !bottom 	&& left)
			result = "\033(0q";
		else if (top 	&& right 	&& bottom 	&& !left)
			result = "\033(0t";
		else if (top 	&& !right 	&& bottom 	&& left)
			result = "\033(0u";
		else if (top 	&& right 	&& !bottom 	&& left)
			result = "\033(0v";
		else if (!top 	&& right 	&& bottom 	&& left)
			result = "\033(0w";
		else if (top 	&& !right 	&& bottom 	&& !left)
			result = "\033(0x";
		*/
		/*
		char symbol;
		if (top)
			if (right)
				if (bottom)
					if (left)
						symbol = 'n';
					else
						symbol = 't';
				else
					if (left)
						symbol = 'v';
					else
						symbol = 'm';
			else
				if (bottom)
					if (left)
						symbol = 'u';
					else
						symbol = 'x';
				else
					if (left)
						symbol = 'j';
					else
						symbol = ' ';	//Только верх
		else
			if (right)
				if (bottom)
					if (left)
						symbol = 'w';
					else
						symbol = 'l';
				else
					if (left)
						symbol = 'q';
					else
						symbol = ' ';	//Только право
			else
				if (bottom)
					if (left)
						symbol = 'k';
					else
						symbol = ' ';	//Только низ
				else
					if (left)
						symbol = ' ';	//Только лево
					else
						symbol = ' ';	//Ни одна сторона
		*/
		
		char symbol;
		if (top)	if (right)	if (bottom) if (left) 	symbol = 'n';
											else		symbol = 't';
								else		if (left)	symbol = 'v';
											else		symbol = 'm';
					else		if (bottom)	if (left)	symbol = 'u';
											else		symbol = 'x';
								else		if (left)	symbol = 'j';
											else		symbol = ' ';	//Только верх
		else		if (right)	if (bottom)	if (left)	symbol = 'w';
											else		symbol = 'l';
								else		if (left)	symbol = 'q';
											else		symbol = ' ';	//Только право
					else		if (bottom)	if (left)	symbol = 'k';
											else		symbol = ' ';	//Только низ
								else		if (left)	symbol = ' ';	//Только лево
											else		symbol = ' ';	//Ни одна сторона
		if (symbol == ' ')
			return "";
		
		/*
		//--------------------НЕ РАБОТАЕТ-------------------------
		char symbol;
		int index = ((top * 1000) + (right * 100) + (bottom * 10) + left);
		switch(index){
			case 1111: symbol = 'n'; break;
			case 1110: symbol = 't'; break;
			case 1101: symbol = 'v'; break;
			case 1100: symbol = 'm'; break;
			case 1011: symbol = 'u'; break;
			case 1010: symbol = 'x'; break;
			case 1001: symbol = 'j'; break;
		//	case 1000: symbol = ' '; break;	//Только верх
			case 0111: symbol = 'w'; break;
			case 0110: symbol = 'l'; break;
			case 0101: symbol = 'q'; break;
		//	case 0100: symbol = ' '; break;	//Только право
			case 0011: symbol = 'k'; break;
		//	case 0010: symbol = ' '; break;	//Только низ
		//	case 0001: symbol = ' '; break;	//Только лево
		//	case 0000: symbol = ' '; break;	//Ни одна сторона
			default:
				std::cout << "Default стороны: " << top << right << bottom << left << ", i=" << i << std::endl;
				return "";
				//break;
		}
		*/
		//std::cout << "Стороны: " << top << right << bottom << left << std::endl;
		//std::cout << "index=" << index << ", symbol='" << symbol << "', line='" << (std::string("\033(0") + symbol) << "'\n";
		//system("pause");
		return EscapeCodeTemplate(out_to_cout_once, std::string("\033(0") + symbol);
	}
	std::string CMDBlinkingTextON(bool out_to_cout_once = true){
		return EscapeCodeTemplate(out_to_cout_once, "\033[5m");	// \033[5m - сделать следующий выводимый текст мигающим
	}
	std::string CMDBlinkingTextOFF(bool out_to_cout_once = true){
		return EscapeCodeTemplate(out_to_cout_once, "\033[0m");	// \033[0m - сбросить атрибуты
	}
	std::string CMDBlinkingText(bool out_to_cout_once = true, std::string text = "", int text_color = 0, int text_background_color = 0){
		//Информация про атрибуты текста - https://blog.sedicomm.com/2026/03/24/kak-izmenyat-stili-vyvoda-komandy-echo-v-linux/
		// \033[5m - Сделать следующий выводимый текст мигающим. \033[0m - сбросить атрибуты
		return EscapeCodeTemplate(out_to_cout_once, "\033[5" +	(text_color == 0 ? "" : (";" + std::to_string(text_color))) +
								(text_background_color == 0 ? "" : (";" + std::to_string(text_background_color))) + "m" + text + "\033[0m");
	}
	
	
	
    void MP_CreateLobby(std::string lobby_name = ""){
    	host = true;
    	bool flag = false;
    	
		waiting(&flag, "Определение публичного IP");
		flag = true;
		waiting_join();
		
		std::cout << "\rПубличный код друга: " << SeaNet.get_public_code() << " и IP: " << SeaNet.get_public_ip() << "\n";
        std::cout << "Локальный код друга: " << SeaNet.get_local_code() << " и IP: " << SeaNet.get_local_ip() << "\n";
        
        
        flag = false;
        waiting(&flag, "Ожидание подключения второго игрока");
		SeaNet.wait_for_connect();
		flag = true;
		waiting_join();
		
		std::cout << CMDStrClear(false) << MP_player2.name << " подключился.\n";
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
					CMDRemNSymbols();	//std::cout << "\033[1D \033[1D";	//Убрать 1 символ из вывода и итоговой строки
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
			
			//std::cout << "\033[s\033[1B" << input.length() << "\033[u";
		}
		std::cout << std::endl;
		return input;
	}
	
	void MP_ConnectLobby(unsigned long long lobby_id = 0, std::string player_code = ""){	//Попытка подключения к пользователю/лобби
		host = false;
		if (lobby_id != 0){
			
		}
		else if (player_code != ""){
			SeaNet.decode_and_use_code(player_code);
			//SeaNet.handle_connect(SeaNet.remote_ip_ + " " + std::to_string(SeaNet.remote_port_));
			SeaNet.handle_connect();
			SeaNet.wait_for_connect();
		}
	}
	
	void MP_Statistics(unsigned long long player_id = 0){
		
	}
    
    void Multiplayer(){
    	MP_player1.field.FullReset();
    	MP_player2.field.FullReset();
    	
		
		/*
		//Тест всех возможных комбинаций линий (Всего доступно 11)
    	system("cls");
    	bool one, two, four, eight;
    	for(int i = 0, temp; i < 16; i++){
    		temp = i;
    		
    		eight = temp / 8;
    		temp -= (8 * eight);
    		
    		four = temp / 4;
    		temp -= (4 * four);
    		
    		two = temp / 2;
    		temp -= (2 * two);
    		
    		one = temp % 2;
    		
    		std::cout << "i=" << i << "\t" << eight << four << two << one << " '";
    		CMDBlinkingText(true, CMDDrawLine(false, eight, four, two, one));
    		std::cout << "'" << std::endl;
		}
		std::cout << "i=99 '-'\n"; 
		std::cout << "i=99 '\033(0q'\n";
    	std::cout << "i=99 '" << CMDDrawLine(false, false, true, false, true) << "'\n";
    	return;
    	*/
    	/*
    	//ДЛЯ ТЕСТОВ
    	std::cout << "НАЧАЛО ИЗМЕНЕНИЙ\n";
    	MP_player1.field.ChangeFieldSize(30, 30);
    	MP_player1.field.ChangeMaxShipLen(10);
    	MP_player1.field.ChangeShipLenCount(5, 2);
    	MP_player1.field.ChangeShipLenCount(6, 4);
    	MP_player1.field.ChangeShipLenCount(8, 3);
    	MP_player1.field.ChangeShipLenCount(10, 5);
    	std::cout << "КОНЕЦ ИЗМЕНЕНИЙ\n";
    	//Установка кораблей
		SetShipsRandomly(MP_player1.field);	//Случайная расстановка
    	
    	//MP_FieldsSynchronization();
    	
    	MP_player1.field.DrawField();
    	std::string temp = MP_player1.field.ToString();
    	std::cout << "ToString: '" << temp << "'\n";
    	
    	std::cout << "Начало тестов восстановления из строки:\n";
    	if (MP_player1.field == MP_player2.field)
    		std::cout << "\n\n\nПОЛЯ ПОЛНОСТЬЮ РАВНЫ!\n\n\n";
    	else
    		std::cout << "\n\n\nПОЛЯ РАЗНЫЕ!!!\n\n\n";
    	std::cout << "Итог: " << MP_player2.field.FromString(temp) << std::endl;
    	if (MP_player1.field == MP_player2.field)
    		std::cout << "\n\n\nПОЛЯ ПОЛНОСТЬЮ РАВНЫ!\n\n\n";
    	else
    		std::cout << "\n\n\nПОЛЯ РАЗНЫЕ!!!\n\n\n";
    	
    	MP_player1.field.ShotTo(5, 5);
    	if (MP_player1.field == MP_player2.field)
    		std::cout << "\n\n\nПОЛЯ ПОЛНОСТЬЮ РАВНЫ!\n\n\n";
    	else
    		std::cout << "\n\n\nПОЛЯ РАЗНЫЕ!!!\n\n\n";
    	//MP_player2.field.DrawField();
    	std::cout << "\n\n\nРазмер строки: " << temp.length() << "\nРезерв строки: " << temp.capacity() << std::endl;
    	return;
    	*/
    	
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
	    system("cls");
	    
    	while(true){
    		display = ' ', input_symbol = ' ';
    		
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
			            	
			            	ChoseGame = true;
			            	flag = false;
			            	try{
			            		MP_ConnectLobby(0, MP_InputCode("Введите код друга: "));
							}
							catch(std::string e){
								system("cls");
								std::cout << "Не удалось подключиться.\n";
								ChoseGame = false;
							}
			            	
			            	//SeaNet.stop();
			            	
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
				try{
					MP_GameStatesSwitch();
				}
				catch(std::string e){
					system("cls");
					std::cout << "Что-то пошло не так! Ошибка: " << e << std::endl;
					system("pause");
				}
				SeaNet.disconnect();
				//std::cout << "Ваш соперник: " << MP_player2.name << ".\nВы готовы?";
				//MP_GameConfirmation();
				//MP_SettingShips();
				//MP_FieldsSynchronization();
				ChoseGame = false;
				system("cls");
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
