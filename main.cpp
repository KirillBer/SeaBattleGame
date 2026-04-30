

//Режим разработчика (для сетевой игры)
#define SEABATTLEGAMEMENU_DEV_MODE_FUNC	//Функционал [DEV]
//#define SEABATTLEGAMEMENU_DEV_MODE_COUT	//Вывод отладочной информации [DEV]



//Функционал [DEV]
#ifdef SEABATTLEGAMEMENU_DEV_MODE_FUNC
	#ifndef SEABATTLEFIELD_BM_H_DEV_MODE
		#define SEABATTLEFIELD_BM_H_DEV_MODE
	#endif
	#ifndef SEABATTLENETWORK_BM_CPP_DEBUG_MODE_FUNC
		#define SEABATTLENETWORK_BM_CPP_DEBUG_MODE_FUNC
	#endif
#endif

//Вывод отладочной информации [DEV]
#ifdef SEABATTLEGAMEMENU_DEV_MODE_COUT
	#ifndef SEABATTLENETWORK_BM_CPP_DEBUG_MODE_COUT
		#define SEABATTLENETWORK_BM_CPP_DEBUG_MODE_COUT
	#endif
#endif


//#include "seaBattleField.h"		//"Поле игры 'Морской бой'" (Включена в seaBattleGame.cpp)
#include "seaBattleGame.cpp"	//Контейнер для поля игры
#include "seaBattleBot.cpp"		//Бот для игры
#include "seaNetwork.cpp"		//"Работа с сетью" для игры 'Морской бой'
#include "ansiSequences.cpp"	//Escape-последовательности 

#include <iostream>	//Ввод и вывод
#include <conio.h>	//Для чтения ввода без прожатия ENTER
#include <thread>	//Для нескольких потоков
//#include <vector>	//Для сохранения и отката ходов
#include <cstring>	//Для работы с файлами
#include <fstream>	//Для работы с файлами
#include <windows.h> // для изменения цвета текста
#include <time.h> // для использования генератора случайных чисел


using namespace std;

using CMD = AnsiSequences;	//Создание псевдонима


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
    	//unsigned short color = 0;
    	//std::string country = "country";
	}Player;
	bool host;
	bool MyMoveTurn;
	bool SubmenuActive = false;
	int MP_last_move_index_;
	Player MP_player1;
    Player MP_player2;
    P2PMessenger SeaNet;
    std::string temp_string_ = "";
    //std::vector<std::string> messages;
    
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
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
	//Игровое подменю
	bool MP_IngameMenu(){
		SubmenuActive = true;
		while(1){
			system("cls");
			std::cout 	<< "\t1. Чат\n"
						<< "\t2. История ходов\n"
						#ifdef SEABATTLEGAMEMENU_DEV_MODE_FUNC
							<< "\t3. [DEV] Сдаться\n"
							<< "\t4. [DEV] Содержимое векторов\n"
							<< "\t5. [DEV] Уничтожение всех кораблей ботом\n"
						#endif
						<< "\t0. Закрыть меню\n";
			
			#ifdef SEABATTLEGAMEMENU_DEV_MODE_FUNC
				std::string str = MP_MenuOrShotInput(true, '0', '5');
			#else
				std::string str = MP_MenuOrShotInput(true, '0', '2');
			#endif
			
			char menu = (str == "ESC" ? '0' : str[0]);
			switch(menu){
				case '1':
					MP_Chat();
					break;
				case '2':
					MP_MovesHistory();
					break;
				#ifdef SEABATTLEGAMEMENU_DEV_MODE_FUNC
					case '3':
						if (Confirm("Вы уверены, что хотите сдаться? Вам будет засчитано поражение.\n")){
							system("cls");
							SubmenuActive = false;
							return true;
						}
						break;
					case '4':
						MP_PrintVectors();
						break;
					case '5':
						MP_DestroyByBot();
						SubmenuActive = false;
						return true;
				#endif
				case '0':
					system("cls");
					SubmenuActive = false;
					return false;
			}
		}
	}
		//Методы подменю
	void MP_Chat(){
		std::string str = "";
		size_t message_count;
		char ch;
		while(1){
			message_count = SeaNet.message.size();
			system("cls");
			std::cout << "Чат с игроком " << MP_player2.name << ". Чтобы закрыть чат нажмите ESC\n";
			for(int i = 0; i < message_count; i++)
				std::cout << "\t" << SeaNet.message[i] << std::endl;
			
			std::cout << str;
			while(message_count == SeaNet.message.size()){
				if (!GetKeyNonblock(&ch)){
					std::this_thread::sleep_for(std::chrono::milliseconds(100));
					continue;
				}
				
				if (ch == 8){			//Кнопка Backspace
		        	if (str.length() > 0){
		        		CMD::CursorOFF();
		        		str.pop_back();
		        		//CMDRemNSymbols();
		        		CMD::StrClear();
		        		CMD::CursorMoveBottom();
		        		CMD::StrClear();
						CMD::CursorMoveTop();
						std::cout << str;
						CMD::CursorON();
					}
				}
		        else if (ch == 27){	//Кнопка Escape
		        	system("cls");
					return;
				}
				else if (ch == 13){	//Перевод строки
					if (str.length() > 0){
						MP_SendMessage(str);
						str = "";
					}
					break;
				}
				else if (str.length() > 499){
					CMD::CursorMoveBottom();
					std::string str_temp = "Достигнут лимит в 500 символов";
					std::cout << str_temp;
					CMD::CursorMoveTop();
					CMD::CursorMoveLeft(true, str_temp.length());	//Вернуться на то же место
				}
				else{
					str.push_back(ch);
					std::cout << ch;
				}
			}
		}
	}
			//Методы внутриигрового чата
	void MP_SendMessage(std::string text){
		std::string str = (SeaNet.get_time() + " " + MP_player1.name +  " " + text);
		if (SeaNet.is_connected()){
			SeaNet.send_message(str);
			SeaNet.message.push_back(str);
		}
	}
		//Продолжение Методы подменю
	void MP_MovesHistory(){
		int start_size = MP_player1.field.GetMovesSize();
		std::vector<int> move_history;
		SeaBattleGame temp = MP_player1.field;
		std::string temp_str = "";
		system("cls");
		std::cout << "Для просмотра истории нажимайте: 'A' и 'D'\n";
		while(1){
			std::cout << CMD::CursorOFF(false) << "Для завершения просмотра нажмите ESC или '0'\n";
			std::cout << "Номер выполненного хода: " << temp.GetMovesSize() << std::endl;
			temp.DrawField();
			_getch();
    		for(bool flag = true; GetKeyState('A') & 0x8000; )	//Кнопка нажата и удерживается в данный момент
				if (flag){
					flag = false;
					if (temp.CheckLastMove() != 5 && temp.GetMovesSize() > 0){	//Не была установка корабля
						move_history.push_back(temp.GetLastMoveIndex());
						temp.CancelLastMove();
						if (move_history[move_history.size() - 1] != -1)
							std::cout << "Ход: " << (char)((move_history[move_history.size() - 1] % temp.GetCols()) + 'A') << move_history[move_history.size() - 1] / temp.GetCols() << std::endl;
						else
							std::cout << "Был сделан бессмысленный ход.\n";
					}
				}
				else
					std::this_thread::sleep_for(std::chrono::milliseconds(20));
			for(bool flag = true; GetKeyState('D') & 0x8000;)	//Кнопка нажата и удерживается в данный момент
				if (flag){
					if (temp.GetMovesSize() < start_size){
						int temp1 = move_history[move_history.size() - 1];
						move_history.pop_back();
						if (temp1 != -1){
							temp.ShotTo(temp1 % temp.GetCols(), temp1 / temp.GetCols());
							std::cout << "Координаты выстрела: " << (char)((temp1 % temp.GetCols()) + 'A') << temp1 / temp.GetCols() << std::endl;
						}
						else{
							for(int i = 0; i < temp.GetCols() * temp.GetRows(); i++)
								if (temp.LookAtCellByIndex(i) == temp.GetValueOfValueMean(1)){
									temp.ShotTo(i % temp.GetCols(), i / temp.GetCols());
									break;
								}
							std::cout << "Был выполнен бесмесленный ход\n";
						}
					}
					else{
						std::cout << "Больше ходов не было\n";
					}
					flag = false;
				}
				else
					std::this_thread::sleep_for(std::chrono::milliseconds(20));
			CMD::CursorON(true);
			if (GetKeyState(VK_ESCAPE) & 0x8000 || GetKeyState('0') & 0x8000)	//Кнопка ESC или '0' нажата и удерживается в данный момент	
				return;
			
			//std::this_thread::sleep_for(std::chrono::milliseconds(20));
			system("cls");
		}
	}
	#ifdef SEABATTLEGAMEMENU_DEV_MODE_FUNC
		void MP_PrintVectors(){
			system("cls");
			std::cout << "System:\n";
			SeaNet.print_vector(SeaNet.system_);
			std::cout << "Service:\n";
			SeaNet.print_vector(SeaNet.service);
			std::cout << "Message:\n";
			SeaNet.print_vector(SeaNet.message);
			std::cout << "Data:\n";
			SeaNet.print_vector(SeaNet.data);
			system("pause");
		}
		void MP_DestroyByBot(){
			for(int i = 0; i < MP_player2.field.GetCols() * MP_player2.field.GetRows() - 1; i++)
				if (MP_player2.field.GetValueOfCellByIndex(i) == MP_player2.field.GetValueOfValueMean(4))
					MP_ShotTo(i % MP_player2.field.GetCols(), i / MP_player2.field.GetCols());
		}
	#endif
	
	
	//Игра
	void MP_Game(){		//Обработка сетевой игры
		cout << "Игра начинается. Приятной игры!\n";
        
        bool turn_result;
        bool game_over = false;
        int move_count = 0;
        while (!game_over){
			/*
	        try{
		        if (player_turn)
		        	turn_result = MP_MoveExecutor(game_over);
		        else
		        	turn_result = MP_MoveWaiting(game_over);
			}
			catch(std::exception& e){
				std::cout << "Ошибка_error: " << e.what() << std::endl;
				system("pause");
			}
			catch(std::string str){
				std::cout << "Ошибка_str: " << str << std::endl;
				system("pause");
			}
			catch(const char* e){
				std::cout << "Ошибка_char: " << e << std::endl;
				system("pause");
			}
	        catch(...){
	        	std::cout << "Ошибка" << std::endl;
	        	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	        	system("pause");
			}
			*/
			if (player_turn)
		        turn_result = MP_MoveExecutor(game_over);
		    else
		        turn_result = MP_MoveWaiting(game_over);
	        
	        if (turn_result) {
	            game_over = true;
	        } 
			else if (!game_over) { // переход хода другому игроку
	            player_turn = !player_turn;
	        }
	        
	        system("cls");
	        
	        if (!game_over) {
	        	std::cout << "\a";
	        	cout << "Смена очереди хода.\n";
	        	std::this_thread::sleep_for(std::chrono::seconds(1));
        	}
        }
	}
	bool MP_GameEnded(){	//Опрос о дальнейшем действии пользователя по завершении сетевой игры (реванш или выйти); true - реванш; false - выйти
		std::cout << (player_turn ? ("\aВсе корабли " + MP_player2.name + " ") : "\aВсе Ваши корабли ") << "потоплены!\nПобеду одержал";
        std::cout << (player_turn ? "и Вы" : (" " + MP_player2.name)) << ".\nСпасибо за игру!\n";
        MP_SaveMatchResult(player_turn);
		system("pause");
		return false;
	}
		//Делающий ход
	bool MP_MoveExecutor(bool& game_over){
		bool extra_turn = true, flag = false, first_create = true;
	    string input;
	    int result;
	    
	    while (extra_turn && !game_over){
			input = "";
			flag = false;
			while(true){
				system("cls");
				MP_DrawFields();
				cout << "Ваш ход.\nВведите '0' для вызова меню, либо координаты для выстрела (например, A5):\n";
				
	        	input = MP_MenuOrShotInput(true, 'A', ('A' + MP_player2.field.GetCols() - 1), '0', ('0' + MP_player2.field.GetRows() - 1));
	        	if (input != "")
	        		if (input[0] == '0' || input == "ESC"){
	        			if (MP_IngameMenu())
	        				return true;
	        			input = "";
					}
					else
						break;
			}
			
	        // Обрабатываем обычный ход
	        int x, y;
	        char input_x = input[0];
	            
		    try {
		        y = stoi(input.substr(1));
		    } catch (...) {
		        InputErrorMessage();
		        extra_turn = true;
		        continue;
		    }
		            
		    if (input_x >= 'A' && input_x <= 'Z')
		        x = input_x - 'A';
		    else if (input_x >= 'a' && input_x <= 'z')
		        x = input_x - 'a';
		    else {
		        InputErrorMessage();
		        extra_turn = true;
		        continue;
		    }
		            
		    if ((x < 0 || x >= MP_player2.field.GetCols()) || y < 0 || y >= MP_player2.field.GetRows()) {
		        InputErrorMessage();
		        extra_turn = true;
		        continue;
		    }
		    result = MP_ShotTo(x, y);
	            
	        //system("cls");
	        switch (result){
	            case 0:
	                cout << "Попадание! Вы получаете дополнительный ход.\n";
	                    
	                extra_turn = true;
	                break;
	            case 1:
	                cout << "Вражеский корабль взорван! Оставшееся количество ";
					result = MP_player2.field.GetCountOfShipsRemain();
					if (result == 0)
						return true;
					cout << "Оставшееся количество вражеских кораблей: " << result << "\n";

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
	    //std::cout << "Выход из метода\n";
	    return false;
	}
	int MP_ShotTo(int x, int y){
		int result;
		result = MP_player2.field.ShotTo(x, y);
		//ShotTo возвращает: 0 - попадание по SHIP; 1 - подрыв последнего корабля; 2 - выстрел в пустую клетку;
		//3 - бессмысленный выстрел, попадание в SHOT, STRIKE, KILL; 4 - ошибка координаты; 5 - ошибка значения ячейки
		SeaNet.send_data(std::to_string(x) + " " + std::to_string(y));
		SeaNet.send_service("ДАННЫЕ_ХОДА_ОТПРАВЛЕНЫ");
		//SeaNet.wait_for_certain_service_message("ДАННЫЕ_ХОДА_ПОЛУЧЕНЫ");
		return result;
	}
		//Ожидающий хода
	bool MP_MoveWaiting(bool& game_over){
		bool extra_turn = true, flag = false, draw_field;
	    string input;
	    int result;
	    
	    while (extra_turn && !game_over){
			input = "";
			flag = false;
			draw_field = true;
			
	        temp_string_ = "";
			thread_ = std::thread([this, &flag]() { MP_MenuOrShotInput(false, '0', '0', ' ', ' ', &flag); });
			while(true){
				if (draw_field && !SeaNet.service.is_exists("ДАННЫЕ_ХОДА_ОТПРАВЛЕНЫ")){
					system("cls");
	        		MP_DrawFields();
					cout << "Ходит " << MP_player2.name << ".\nДля вызова меню введите '0'\n";
					draw_field = false;
				}
				else
					std::this_thread::sleep_for(std::chrono::milliseconds(10));
				
				
	        	if (SeaNet.service.is_exists("ДАННЫЕ_ХОДА_ОТПРАВЛЕНЫ")){
	        		flag = true;
					thread_.join();
					break;
				}
				
				
	        	input = temp_string_;
	        	if (input != "" && !SubmenuActive){
	        		if (input[0] == '0' || (input.length() == 3 && input == "ESC")){
						bool temp_flag = flag;
						flag = true;
						thread_.join();
	        			if (MP_IngameMenu())
	        				return true;
	        			
	        			flag = temp_flag;
	        			
	        			temp_string_ = "";
						thread_ = std::thread([this, &flag]() { MP_MenuOrShotInput(false, '0', '0', ' ', ' ', &flag); });
	        			draw_field = true;
					}
					else
						break;
				}
			}

	        // Обрабатываем обычный ход
			result = MP_WaitShotResult();
	        
	        //int result = enemy.ShotTo(x, y);
	        system("cls");
	        switch (result){
	            case 0:
	                cout << "По вашему кораблю (" << (char)((MP_last_move_index_ % MP_player1.field.GetCols()) + 'A') << (char)(MP_last_move_index_ / MP_player1.field.GetCols() + '0') << ") попали!\n";
	                    
	                extra_turn = true;
	                break;
	            case 1:
					cout << "Ваш корабль взорван (" << (char)((MP_last_move_index_ % MP_player1.field.GetCols()) + 'A') << (char)(MP_last_move_index_ / MP_player1.field.GetCols() + '0') << ")! ";
						
					result = MP_player1.field.GetCountOfShipsRemain();
					if (result == 0){
						if (thread_.joinable())
							thread_.join();
						return true;
					}
					cout << "Оставшееся количество ваших кораблей: " << result << "\n";

	                extra_turn = true;
	                break;
	            case 2:
	                cout << MP_player2.name << " промахнулся (" << (char)((MP_last_move_index_ % MP_player1.field.GetCols()) + 'A') << (char)(MP_last_move_index_ / MP_player1.field.GetCols() + '0') << ")!\n";
	                	
	                extra_turn = false;
	                break;
	            case 3:
	                cout << MP_player2.name << " сделал бессмысленный ход (" << (char)((MP_last_move_index_ % MP_player1.field.GetCols()) + 'A') << (char)(MP_last_move_index_ / MP_player1.field.GetCols() + '0') << ")!\n";
	                	
	                extra_turn = true;
	                break;
	            default:
	                extra_turn = false;
	                break;
	        }
	            
	        if (extra_turn && !SubmenuActive) {
	            cout << MP_player2.name << " получил дополнительный ход.\n";
	            system("pause");
	        }
	    }
	    if (thread_.joinable())
			thread_.join();
	    return false;
	}
	int MP_WaitShotResult(){
		SeaNet.wait_for_certain_service_message("ДАННЫЕ_ХОДА_ОТПРАВЛЕНЫ");
		while(SeaNet.data.size() == 0);
		//SeaNet.send_service("ДАННЫЕ_ХОДА_ПОЛУЧЕНЫ");
		//std::string shot_message = SeaNet.data.pop_back();
		std::string shot_message = SeaNet.data.first();
		SeaNet.data.erase(0);
		
		int x = StrToInt(shot_message);
		int y = StrToInt(shot_message);
		MP_last_move_index_ = (y * MP_player1.field.GetCols() + x);
		return MP_player1.field.ShotTo(x, y);
	    //SeaNet.send_service("РЕЗУЛЬТАТ_МОНЕТКИ_ПОЛУЧЕН");
	}
	
	
	//Состояния перед игрой
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
    	//Состояния подготовки к игре
	bool MP_GameConfirmation(){	//Подтверждение готовности; Возвращает "готов": true - да, false - нет
		SeaNet.send_data(std::to_string(MP_player1.id) + " " + MP_player1.name);
		SeaNet.send_service("ДАННЫЕ_АККАУНТА_ОТПРАВЛЕНЫ");
		SeaNet.wait_for_certain_service_message("ДАННЫЕ_АККАУНТА_ОТПРАВЛЕНЫ");
		std::string str = SeaNet.data.pop_back();
		MP_player2.id = StrToUll(str);
		MP_player2.name = str;
		
		bool cancel_input = false, stop_waiting_result = false;
		
		auto lambda = [this, &cancel_input, &stop_waiting_result]() {
		    while (SeaNet.is_connected() && !stop_waiting_result) {
		        if (SeaNet.service.is_exists("ГОТОВНОСТЬ_УСПЕШНО"))
		            return;
		        if (SeaNet.service.is_exists("ГОТОВНОСТЬ_ОТКАЗАНО"))
		            break;
		        std::this_thread::sleep_for(std::chrono::milliseconds(10));
		    }
		    cancel_input = true;
		    return;
		};
		
		thread_ = std::thread(lambda);
		std::cout << "Ваш соперник: " << MP_player2.name << ".\nПринять матч?\n1 - да\n0 - нет\n";
		str = MP_MenuOrShotInput(false, '0', '1', ' ', ' ', &cancel_input);
		char ch = (str.length() == 3 ? '0' : (str.length() > 0 ? str[0] : '0'));	//При нажатии ESC, вводе '0' или завершении из-за отказа соперника ch = 0
		switch(ch){
			case '0':
				SeaNet.send_service("ГОТОВНОСТЬ_ОТКАЗАНО");
				stop_waiting_result = true;
				if (thread_.joinable())
					thread_.join();
				return false;
			case '1':
				std::cout << "\nОжидание второго игрока.";
				SeaNet.send_service("ГОТОВНОСТЬ_УСПЕШНО");	//Отправить ожидаемое сообщение
				if (thread_.joinable())
					thread_.join();
				return (cancel_input ? false : true);
		}
		
    	std::cout << "Ошибка, метод прошёл за switch() каким-то чудом.\nchar = '" << ch << "'\ncancel_input=" << cancel_input << std::endl;
    	system("pause");
    	return false;
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
			std::string str = SeaNet.data.pop_back();
			if (!MP_player2.field.FromString(str)){
				std::cout << "Ошибка синхронизации поля. Исходная строка поля: '" << str << "'\n";
				system("pause");
			}
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
	
	
	//Работа с аккаунтами
	bool MP_CreateAccount(){
		system("cls");
		char input_symbol;
		std::string result = "";
		while(true){
			std::cout << "Создание профиля.\nВведите имя пользователя (от 3 до 20 символов):\n";
			std::cout << result;
			input_symbol = _getch();
			if (input_symbol == 8){			//Кнопка Backspace (без подтверждения Enter'ом)
				if (result.length() > 0)
					result.pop_back();
		    }
		    else if (input_symbol == 27)	//Escape (без подтверждения Enter'ом)
				return false;
			else if (input_symbol == 13){	//Перевод строки
				if (result.length() > 3)
					if (!MP_AccountExists(result)){
						MP_player1.name = result;
						MP_player1.id = GenerateID();
						MP_SaveAccountData();
						MP_SaveLastUsedAccount();
						return true;
					}
					else{
						std::cout << "\nАккаунт с таким именем уже существует!\n";
						system("pause");
					}
			}
			else if(result.length() < 20 &&
					((input_symbol >= 'a' && input_symbol <= 'z') ||
					(input_symbol >= 'A' && input_symbol <= 'Z') ||
					(input_symbol >= 'а' && input_symbol <= 'я') ||
					(input_symbol >= 'А' && input_symbol <= 'Я') ||
					(input_symbol >= '0' && input_symbol <= '9')))
				result.push_back(input_symbol);
			system("cls");
		}
	}
	bool MP_AccountExists(std::string account_name){
		std::ifstream file(account_name + ".txt");	//Открываем файл для чтения
		return (file.is_open() ? true : false);
	}
	std::string MP_ReadLastUsedAccount(){
		std::ifstream file("last_user.txt");	//Открываем файл для чтения
		if (!file.is_open())
			return "";
		
		std::string result = "";
		if	(!ReadString(file, result)){	//Ошибка чтения
				file.clear();	//Сбросить состояние чтения
				file.close();
				return "";
		}
		file.close();
		return result;
	}
	bool MP_LoadLastUsedAccount(){	//Загрузка из файла; true - успешно; false - некорректное название/ошибка открытия файл/ошибка чтения
		std::string result = MP_ReadLastUsedAccount();
		if (result == "")
			return false;
		MP_player1.name = result;
		return true;
	}
		//Запись в файл
	int MP_SaveAccountData(){	//Сохранение в файл; 0 - успешно; 1 - некорректное название/ошибка открытия файла; 2 - ошибка записи; 3 - ошибка сохранения
		std::ofstream file(MP_player1.name + ".txt", std::ios::trunc);	//Открытие файла для записи с очисткой содержимого файла и записью с начала
		if (!file.is_open())
			return 1;
		
		if	(!RecordElementULL(file, "id", MP_player1.id)){	//Ошибка записи
				file.clear();	//Сбросить состояние записи
				return 2;
		}
		file.close();
		return (file.fail() ? 3 : 0);	//Ошибка при финальной записи/закрытии?
	}
	int MP_SaveLastUsedAccount(){ //Сохранение в файл; 0 - успешно; 1 - некорректное название/ошибка открытия файла; 2 - ошибка записи; 3 - ошибка сохранения
		//std::fstream file("test.txt", std::ios::in | std::ios::out);
		std::ofstream file("last_user.txt", std::ios::trunc);	//Открытие файла для записи с очисткой содержимого файла и записью с начала
		if (!file.is_open())
			return 1;
		
		if	(!RecordString(file, MP_player1.name + "\n")){	//Ошибка записи
				file.clear();	//Сбросить состояние записи
				return 2;
		}
		file.close();
		return (file.fail() ? 3 : 0);	//Ошибка при финальной записи/закрытии?
	}
	void MP_SaveMatchResult(bool won){	//Записать результат матча
    	std::fstream file(MP_player1.name + ".txt", std::ios::in | std::ios::out);
    	
    	//MP_player2.id = 1234;
    	std::string str, player2_str_id = std::to_string(MP_player2.id), loses_str, wins_str, all_file_str = "", file_str, needed_player_str;
    	int loses = 0, wins = 0;
    	/*
    	size_t pos = 0, current_pos = 0;
    	
		//while (getline(flow_name, str)){
		while(getline(file, str))
			all_file_str += (str + '\n');
		
		
		file_str = all_file_str;
		while(pos != SIZE_MAX && file_str.length() > 0){
			file_str = file_str.substr(pos, all_file_str.length() - 1);
			current_pos = file_str.find('\n');
			if (current_pos == SIZE_MAX)
				break;
			if (current_pos > file_str.find("\n" + player2_str_id))
				break;
			std::cout << "Цикл. Позиция: " << pos << ", cur: " << current_pos << ", '" << file_str << "'" << std::endl;
			pos = current_pos;
			system("pause");
		}
		std::cout << "Позиция id: " << pos << ", cur: " << current_pos << std::endl;
		system("pause");
		*/
		
		while(true){
			file >> str;
			if (file.fail() && !file.eof()){
				file << MP_player2.id << ' ' << (won ? 0 : 1) << ':' << (won ? 1 : 0) << std::endl;
				return;
			}
            if (str == player2_str_id){
				break;
			}
            getline(file, str);
        }
        
		file >> str;
		file >> loses;
		file.get();
		file >> wins;
		
		loses_str = std::to_string(loses);
		wins_str = std::to_string(wins);
		
		if (won)
			wins++;
		else
			loses++;
		
		file.seekg((loses_str.length() + 2 + wins_str.length()) * -1, std::ios::cur);
		file << loses << ':' << wins;
		file.close();
	}
	
	
	//Визуальные методы
	void waiting(bool *stop, std::string text_before_waiting_points = ""){	//Отрисовка трёх точек ожидания в потоке thread_ пока флаг stop = false
		thread_ = std::thread([this, stop, text_before_waiting_points]() { waiting_points(stop, text_before_waiting_points); });
	}
	void waiting_join(){	//Остановить выполнение потока, пока не завершится обработка точек из waiting
		thread_.join();
	}
    void waiting_points(bool *stop, std::string text_before_waiting_points = ""){
    	CMD::CursorOFF();	//Скрытие курсора
		std::cout << text_before_waiting_points;
		for(int i; !(*stop); ){
        	for(i = 0; i < 3 && !(*stop); i++){
        		std::this_thread::sleep_for(std::chrono::milliseconds(400));
        		std::cout << ".";
			}
			if (!(*stop))
				std::this_thread::sleep_for(std::chrono::milliseconds(400));
			CMD::RemNSymbols(true, i);
		}
		CMD::CursorON();	//Отображение курсора
	}
	void TopOfTable(std::string first_column_name, std::string second_column_name, std::string third_column_name){
		bool second_draw = false;
		for(int count = 0; count < 2; count++){
			for(int i = 0; i < first_column_name.length(); i++)
				CMD::DrawLine(true, false, true, false, true);
			CMD::DrawLine(true, second_draw, true, true, true);
			for(int i = 0; i < second_column_name.length(); i++)
				CMD::DrawLine(true, false, true, false, true);	
			CMD::DrawLine(true, second_draw, true, true, true);
			for(int i = 0; i < third_column_name.length(); i++)
				CMD::DrawLine(true, false, true, false, true);
			CMD::DrawLine(true, second_draw, false, true, true);
			std::cout << std::endl;
			if (!second_draw)
				std::cout 	<< first_column_name << CMD::DrawLine(false, true, false, true, false)
							<< second_column_name << CMD::DrawLine(false, true, false, true, false)
							<< third_column_name << CMD::DrawLine(false, true, false, true, false) << std::endl;
			second_draw = true;
		}
	}
	void CenterOfTable(std::string first_column_name, std::string second_column_name, std::string third_column_name, std::string first_column_object, std::string second_column_object, std::string third_column_object){
		int temp = (first_column_name.length() - first_column_object.length());
		std::cout << first_column_object;
		if (temp > 0) CMD::CursorMoveRight(true, temp);
		
		temp = (second_column_name.length() - second_column_object.length());
		std::cout << CMD::DrawLine(false, true, false, true, false) << second_column_object;
		if (temp > 0) CMD::CursorMoveRight(true, temp);
		std::cout << CMD::DrawLine(false, true, false, true, false) << third_column_object;
		
		temp = (third_column_name.length() - third_column_object.length());
		if (temp > 0) CMD::CursorMoveRight(true, temp);
		std::cout << CMD::DrawLine(false, true, false, true, false) << std::endl;
	}
	void BottomOfTable(std::string first_column_name, std::string second_column_name, std::string third_column_name){
		for(int i = 0; i < first_column_name.length(); i++)
			CMD::DrawLine(true, false, true, false, true);
		CMD::DrawLine(true, true, true, false, true);
		for(int i = 0; i < second_column_name.length(); i++)
			CMD::DrawLine(true, false, true, false, true);
		CMD::DrawLine(true, true, true, false, true);
		for(int i = 0; i < third_column_name.length(); i++)
			CMD::DrawLine(true, false, true, false, true);
		CMD::DrawLine(true, true, false, false, true);
		std::cout << std::endl;
	}
	void MP_DrawFrame(unsigned int weight, unsigned int height, bool overwrite){	//Нарисовать рамку
		if (weight < 2 || height < 2)
			return;
		
		CMD::DrawLine(true, false, true, true, false);
    	for(int i = 0; i < (weight - 2); i++)
    		CMD::DrawLine(true, false, true, false, true);
    	CMD::DrawLine(true, false, false, true, true);
    	std::cout << std::endl;
    	
    	for(int n = 0; n < (height - 2); n++){
    		CMD::DrawLine(true, true, false, true, false);
	    	if (overwrite){
	    		for(int i = 0; i < (weight - 2); i++)
	    			std::cout << " ";
			}
	    	else
    			CMD::CursorMoveRight(true, (weight - 2));
	    	CMD::DrawLine(true, true, false, true, false);
			CMD::CursorMoveLeft(true, weight);
	    	CMD::CursorMoveBottom();
		}
    	
    	CMD::DrawLine(true, true, true, false, false);
    	for(int i = 0; i < (weight - 2); i++)
    		CMD::DrawLine(true, false, true, false, true);
    	CMD::DrawLine(true, true, false, false, true);
		CMD::CursorMoveLeft(true, weight);
	    CMD::CursorMoveBottom();
	}
	void MP_DrawFields(){	//Отрисовка двух полей
		MP_player1.field.DrawFields(MP_player2.field);
	}
	
	
	//Функциональные методы
	std::string MP_MenuOrShotInput(bool with_thread_blocking_mode, char first_symbol_left_border, char first_symbol_right_border, char second_symbol_left_border = ' ', char second_symbol_right_border = ' ', bool *stop_flag = 0){	//Ввод для 
			char input_symbol, display1 = ' ', display2 = ' ';
			std::string str = "";
			bool two_symbols_mode = ((second_symbol_left_border == ' ' || second_symbol_right_border == ' ') ? false : true);
			while (true) {
				CMD::StrClear();
				if (display1 != ' ')
					std::cout << display1;
				if (display2 != ' ')
					std::cout << display2;
					
		        if (with_thread_blocking_mode)
					input_symbol = _getch();	//Это getch(), но без эха символа
		        else
					while (!GetKeyNonblock(&input_symbol)){
						std::this_thread::sleep_for(std::chrono::milliseconds(20));
						if (stop_flag != 0)
							if (*stop_flag)
								return "";
					}
		        
		        if (input_symbol == ')' || ((display1 == ' ' || display1 == '0') && input_symbol == '0')){
		        	display1 = '0';
					display2 = ' ';
		        	continue;
				}

				if (display1 != '0' && (second_symbol_left_border <= input_symbol && input_symbol <= second_symbol_right_border) && two_symbols_mode)
		        	display2 = input_symbol;
	        	else if (first_symbol_left_border <= input_symbol && input_symbol <= first_symbol_right_border)
		        	display1 = input_symbol;
	            
				if (input_symbol == 8){			//Кнопка Backspace (без подтверждения Enter'ом)
					if (display2 == ' ')
	        			display1 = ' ';
					else
						display2 = ' ';
	        	}
	            else if (input_symbol == 27){	//Escape (без подтверждения Enter'ом)
				    temp_string_ = "ESC";
					return "ESC";
				}
				else if (input_symbol == 13)	//Перевод строки
						if ((display1 != ' ' && (two_symbols_mode ? (display2 != ' ') : true)) || display1 == '0'){
							str.push_back(display1);
							str.push_back(display2);
							CMD::StrClear();
							temp_string_ = str;
							return str;
						}	
			}
	}
	unsigned long long GenerateID(){	//Сгенерировать ID
		std::random_device rd;
	    std::mt19937 gen(rd());
	    std::uniform_int_distribution<> distrib;
	    auto now = std::chrono::system_clock::now();
	    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
	    return ((((unsigned long long)distrib(gen)) << (sizeof(long long) * 8 / 2)) ^ ms);
	}
	bool GetKeyNonblock(char *char_for_input){	//Неблокирующий ввод с записью в char_for_input, если прожата клавиша (без эха символа). true - успешно, false - неудачно
		if (_kbhit() != 0){	//Прожата ли клавиша
			*char_for_input = _getch();	//Получить символ без эхо символа
			return true;
		}
		return false;
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
	const unsigned long long StrToUll(std::string& text, bool change_string = true){	//Вернуть первое число из начала строки (1 в 1 из SeaBattleField)
		size_t pos = text.find(' ');
		unsigned long long temp;
		
		try{
			temp = std::stoull(text.substr(0, (pos == SIZE_MAX ? text.length() : pos)));
		}
		catch(...){
			throw std::string("String to int error");	//Чтобы вместо Ошибка="stoi" выбрасывать string="текст ошибки"
		}
		if (change_string)
			text = text.substr((pos == SIZE_MAX ? 0 : (pos + 1)), text.length());
		
		return temp;
	}
	
	
	//Основное меню сетевой игры
    void Multiplayer(){
    	if (!MP_LoadLastUsedAccount())
    		while(!MP_CreateAccount());
    	std::string temp;
    	char result;
	    bool flag, ChoseGame = false;
	    system("cls");
	    
    	while(true){
    		MP_player1.field.FullReset();
	    	MP_player2.field.FullReset();
	    	SeaNet.reset_all_data();
	    	
    		flag = true;
    			
	    	cout 	<< "Сетевая игра"
			    	#ifdef SEABATTLEGAMEMENU_DEV_MODE_FUNC
		    			<< " [DEV: FUNC"
		    			#ifdef SEABATTLEGAMEMENU_DEV_MODE_COUT
		    				<< ", COUT]\n"
		    			#else
		    				<< "]\n"
		    			#endif
		    		#else
		    			#ifdef SEABATTLEGAMEMENU_DEV_MODE_COUT
							<< " [DEV: COUT]\n"
		    			#else
		    				<< "\n"
		    			#endif
		    		#endif
	        		<< "\t1. Создать игру\n"
	            	<< "\t2. Присоединиться к игре\n"
	            	<< "\t3. Статистика\n"
	            	<< "\t0. Вернуться в главное меню\n";
	        
	        temp = MP_MenuOrShotInput(true, '0', '3');
	        result = (temp.length() == 3 ? '0' : temp[0]);
	        system("cls");
			switch (result) {
			    case '1':
			        SeaNet.start();
			        MP_CreateLobby();
			        ChoseGame = true;
			        flag = false;
			        break;
			    case '2':
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
			        break;
			    case '3':
			        MP_Statistics();
			        system("pause");
			        flag = false;
			        break;
			    case '0':
			        SeaNet.stop();
					return;
			}
			
			if (ChoseGame){
				try{
					MP_GameStatesSwitch();
				}
				catch(std::string e){
					std::cout << "Что-то пошло не так! Ошибка_str: " << e << std::endl;
					system("pause");
				}
				catch(std::exception& e){
					std::cout << "Что-то пошло не так! Ошибка_err: " << e.what() << std::endl;
					system("pause");
				}
				catch(...){
					std::cout << "Что-то пошло не так!" << std::endl;
					system("pause");
				}
				SeaNet.disconnect();
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
				<< "8. SeaNet.stop();\n"
				<< "9. Chat\n";
            
            
            if (!(cin >> choice) || choice < 0 || choice > 9) {
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
				case 9:
					MP_Chat();
					break;
            }
        }
	}
		//Пункты меню сетевой игры
    void MP_CreateLobby(std::string lobby_name = ""){
    	host = true;
    	bool flag = false;
    	
		waiting(&flag, "Определение публичного IP");
		flag = true;
		waiting_join();
		
		//std::cout << "\rПубличный код друга: " << SeaNet.get_public_code() << " и IP: " << SeaNet.get_public_ip() << "\n";
		//std::cout << "Локальный код друга: " << SeaNet.get_local_code() << " и IP: " << SeaNet.get_local_ip() << "\n";
		
        std::cout << "\rЛокальный код друга: " << SeaNet.get_local_code() << " и IP: " << SeaNet.get_local_ip() << "\n";
        
        
        flag = false;
        waiting(&flag, "Ожидание подключения второго игрока");
		SeaNet.wait_for_connect();
		flag = true;
		waiting_join();
		
		std::cout << CMD::StrClear(false) << MP_player2.name << " подключился.\n";
	}
	void MP_ConnectLobby(unsigned long long lobby_id = 0, std::string player_code = ""){	//Попытка подключения к пользователю/лобби
		host = false;
		if (lobby_id != 0){
			
		}
		else if (player_code != ""){
			SeaNet.decode_and_use_code(player_code);
			SeaNet.handle_connect();	//SeaNet.handle_connect(SeaNet.remote_ip_ + " " + std::to_string(SeaNet.remote_port_));
			SeaNet.wait_for_connect();
		}
	}
			//Методы подключения к лобби
	std::string MP_InputCode(std::string message_befor_input = ""){
		std::string input;
		char input_symbol;
		bool flag = true;
		
		std::cout << message_befor_input;
		
		while (flag) {
	        input_symbol = getch();

	        if (input_symbol == 8){			//Кнопка Backspace (Стереть)
				if (input.length() > 0){
					CMD::RemNSymbols();
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
		}
		std::cout << std::endl;
		return input;
	}
		//Продолжение Пункты меню сетевой игры
	void MP_Statistics(){	//Отображение статистики игр
		std::ifstream file(MP_player1.name + ".txt");	//Открываем файл для чтения
		if (!file.is_open())
			return;
		
		std::string str, name, loses_str, wins_str;
		getline(file, str);
		system("cls");
		std::cout << "\t  Статистика игрока " << MP_player1.name << std::endl;
		std::string table_player = "Игрок               ", table_wins = "Победы", table_loses = "Поражения";
		TopOfTable(table_player, table_wins, table_loses);
		
		while(file.fail() || !file.eof()){
			getline(file, str, ' ');		//ID
			getline(file, name, ' ');		//Имя
			getline(file, loses_str, ':');	//Поражения
			getline(file, wins_str);		//Победы
			
			if (file.fail() && !file.eof()){
				return;
			}
			CenterOfTable(table_player, table_wins, table_loses, name, wins_str, loses_str);
        }
        BottomOfTable(table_player, table_wins, table_loses);
		file.close();
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
	bool RecordElementULL(ofstream &flow_name, string element_name, unsigned long long number){	//Записать в файл элемент в кавычки; true - успешно; false - ошибка записи
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
    bool ReadString(std::ifstream &flow_name, std::string &element){	//Считать из файла символ; true - успешно; false - ошибка чтения
		flow_name >> element;
		return !(flow_name.fail() || flow_name.eof());
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
	bool ReadElementULL(ifstream &flow_name, string element_name, unsigned long long *element){	//Считать из файла элемент в кавычках; true - успешно; false - ошибка чтения
		if (!ReadIsStringEquals(flow_name, element_name + "=\"") || !ReadNumberULL(flow_name, element) || !ReadIsCharEqualsReaded(flow_name, '"') || !ReadIsCharEqualsReaded(flow_name, '\n'))
			return false;
		return true;
	}
	bool ReadNumberULL(ifstream &flow_name, unsigned long long *element){	//Считать из файла элемент в кавычках; true - успешно; false - ошибка чтения
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

	SeaBattleGameMenu menu;
    menu.Run();
    
	return 0;
}
