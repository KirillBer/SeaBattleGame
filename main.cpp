#include <iostream>




//#include <locale>
// Метод мультиплеера ~1250 строка










/*
#ifdef  _WIN32
#define _WIN32_WINNT 0x0A00
#endif
#define ASIO_STADNALONE
*/






//Из морского боя
#include <cstdlib>		

#include "seaBattleField.h"	//Библиотека "Поле игры 'Морской бой'"
#include "seaNetwork.cpp"	//Библиотека "Работа с сетью" для игры 'Морской бой'
//#include "network.cpp"


#include <vector>	//Для сохранения и отката ходов
#include <cstring>	//Для работы с файлами
#include <fstream>	//Для работы с файлами
#include <iomanip> //Для форматирования текста
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









class SeaBattleGame : public SeaBattleField{ //Интерфейс и реализация игры
	private:
		char *FieldSymbol;	//символы отображения клеток во время игры
	public:
		SeaBattleGame(int cols_ = 10, int rows_ = 10) : SeaBattleField(cols_, rows_) {
			FieldSymbol = 0;
			FieldSymbol = new char[5];
			if (!FieldSymbol)
				exit(1);
			FieldSymbol[0] = '-';	//Пустая клетка
			FieldSymbol[1] = '*';	//Стреленая клетка
			FieldSymbol[2] = 'X';	//Раненая клетка корабля
			FieldSymbol[3] = '#';	//Клетка взорванного корабля
			FieldSymbol[4] = 'S';	//Целая клетка корабля
		}
		~SeaBattleGame() {
			if (FieldSymbol)
				delete[] FieldSymbol;
			FieldSymbol = 0;
		}
		SeaBattleGame(const SeaBattleGame& other){
			FieldSymbol = 0;
			FieldSymbol = new char[5];
			if (!FieldSymbol)
				exit(1);
			FieldSymbol[0] = other.FieldSymbol[0];	//Пустая клетка
			FieldSymbol[1] = other.FieldSymbol[1];	//Стреленая клетка
			FieldSymbol[2] = other.FieldSymbol[2];	//Раненая клетка корабля
			FieldSymbol[3] = other.FieldSymbol[3];	//Клетка взорванного корабля
			FieldSymbol[4] = other.FieldSymbol[4];	//Целая клетка корабля
		}
		
		void PrintColored(const string& text, int color) 
		{
		    cout << "\033[1;3" << color << "m" << text << "\033[0m";
		}
		
		void DrawSymbol(int index){	//Отрисовка элемента index поля, в виде игрового символа
			unsigned char value = GetValueOfCellByIndex(index);
			string myString(1, FieldSymbol[0]);
			if (value == GetValueOfValueMean(0))
				PrintColored(myString, 5);
				//cout << FieldSymbol[0];
			else if (value == GetValueOfValueMean(1))
				cout << FieldSymbol[1];
			else if (value == GetValueOfValueMean(2))
				cout << FieldSymbol[2];
			else if (value == GetValueOfValueMean(3))
				cout << FieldSymbol[3];
			else if (value == GetValueOfValueMean(4))
				cout << FieldSymbol[4];
		}
		void DrawFields(SeaBattleGame &enemy_field) 
		{
	        int y;
	        
	        PrintWordNTimes(" ", GetCols() - 1);
	        cout << "Ваше поле ";
	        PrintWordNTimes(" ", 2 * GetCols() + 3);
	        cout << "Поле врага" << endl;
	        
	        draw_top_letters();
	        cout << "          ";
	        enemy_field.draw_top_letters();
	        
	        for(cout << endl, y = 0; y < GetRows(); y++, cout << " |\n") {
	            draw_row(y, 1);
	            cout << " |        ";
	            enemy_field.draw_row(y, 0);
	        }
	        cout << endl;
	    }
		
	    void draw_row(int y, int owner) //строки с учётом владения полем
		{ 
	        int x, value;
	        for(cout << setfill(' ') << setw(2) << y << "|", x = 0; x < GetCols(); x++) {
	            value = GetValueOfCell(x, y);
	            if (value == GetValueOfValueMean(0)) 
				{
	                cout << " " << FieldSymbol[0];
	            } 
				else if (value == GetValueOfValueMean(4)) 
				{
	                if (owner)
	                    cout << " " << FieldSymbol[4];
	                else
	                    cout << " " << FieldSymbol[0];
	            } 
				else 
				{
	                for (int i = 0; i < 5; i++) {
	                    if (value == GetValueOfValueMean(i)) {
	                        cout << " " << FieldSymbol[i];
	                        break;
	                    }
	                }
	            }
	        }
	    }
		
		void DrawField(){	//Отрисовать 1 поле игровыми символами
			draw_top_letters();
			cout << endl;
			for(int y = 0; y < GetRows(); y++, cout << " |\n")
	            draw_row(y, 1);
			cout << endl;
		}
		
		void draw_top_letters()
		{						//буквенная полоса горизонтальных координат
			short i;
			for(cout << "   ", i = 0; i < this->GetCols(); i++)
				//printf(" %c", i + 'A');
				cout << " " << (char)(i + 'A');
		}
		
		void PrintWordNTimes(string word, int n){ 				//вывести строку n раз
			for(n; n > 0; n--)
				cout << word;
		}
		
		void PrintFieldValues(){	//Отрисовать 1 поле хранимыми значениями
			for(int index = 0; index < GetCols() * GetRows(); index++){
				cout << GetValueOfCellByIndex(index);
				if (((index + 1) % GetCols() == 0) && (index > 0))
					cout << endl;
				else
					cout << " ";
			}
			cout << endl;
		}
		void PrintFieldIndexs(){	//Отрисовать индексы 1 поля
			for(int index = 0; index < GetCols() * GetRows(); index++){
				cout << index;
				if (((index + 1) % GetCols() == 0) && (index > 0))
					cout << endl;
				else
					cout << " ";
			}
		}
		void PrintFieldValueMean(){	//Отобразить значения, которыми обозначаются EMPTY, SHOT, ..., SHIP
			cout << "Обозначения:\nEMPTY - " << GetValueOfValueMean(0) << "\nSHOT - " << GetValueOfValueMean(1)
			<< "\nSTRIKE - " << GetValueOfValueMean(2) << "\nKILL - " << GetValueOfValueMean(3)
			<< "\nSHIP - " << GetValueOfValueMean(4) << "\n";
		}
		
		void SetShipsRandomly() 
		{
	        ClearField();
	        ResetShipsRemain();
	        srand(clock()); // инициализация генератора
	        
	        int max_len = GetMaxShipLen();
	        for (int len = max_len; len >= 1; len--) 
			{
	            int count = GetShipsRemainCountOfNLen(len);
	            for (int i = 0; i < count; i++) 
				{
	                if (!PlaceRandomShip(len)) {
	                    i--;
	                }
	            }
	        }
    	}
    
	    bool PlaceRandomShip(int len) 
		{
		    int attempts = 0;
		    while (attempts < 1000) {
		        int x = rand() % GetCols();
		        int y = rand() % GetRows();
		        
		        if (len == 1) {
		            if (SetShip(1, 1, x, y) == 0) {
		                return true;
		            }
		        } else {
		            // Пробуем все возможные направления
		            for (int side = 1; side <= 4; side++) {
		                if (IsSideAvailable(x, y, len, side - 1) != 0) {
		                    if (SetShip(len, side, x, y) == 0) {
		                        return true;
		                    }
		                }
		            }
		        }
		        attempts++;
		    }
		    return false;
		}
};
	


class SeaBattleBot : public SeaBattleGame{ //Класс со всей логикой бота для игры
	private:
		typedef enum BotState{	//Состояние бота
			Searching,	//В поиске корабля
			Destruction	//Уничтожение корабля
		}BotState;
		typedef enum ShipRotation{	//Ориентация найденного корабля
			Unknown,	//Неизвестный
			Horizontal,	//Горизонтальный
			Vertical	//Dертикальный
		}ShipRotation;
		int ef_cols, ef_rows;	//Высота и ширина поля, в которое будет происходить выстрел
		BotState State;	//Текущее состояние бота
		int LastShotIndex;	//Индекс последнего его хода
		int LastShotResult;	//Результат последнего его хода
		int solution; //Решение, куда он будет стрелять
		ShipRotation Rotation;	//Ориентация атакуемого корабля
		int FirstHitIndex;	//Индекс клетки с первым попаданием по кораблю
		bool ShootingRight; //Бот для уничтожения движется вправо (вниз), иначе влево (вверх)
		int *EmptyCells;
		int EmptyCellsSize;
				
	public:
		SeaBattleBot(int enemy_field_cols = 10, int enemy_field_rows = 10) : SeaBattleGame(enemy_field_cols, enemy_field_rows), ef_cols(GetCols()), ef_rows(GetRows()){
			State = Searching;
			LastShotIndex = -1;
			LastShotResult = -1;
			solution = -1;
			Rotation = Unknown;
			FirstHitIndex = -1;
			ShootingRight = true;
			EmptyCellsSize = -1;
			EmptyCells = 0;
		}
		~SeaBattleBot(){
			if (EmptyCells != 0)
				delete[] EmptyCells;
		}
	//Изменение данных бота
	private:
		void ResetAboutShipInfo(){	//Сбросить всю информацию по найденному кораблю
			Rotation = Unknown;
			ShootingRight = true;
			FirstHitIndex = -1;
		}
	public:
		void SetBotFieldSize(int new_cols, int new_rows){	//Установить размер поля, в котором бот будет стрелять
			if (new_cols > 0 && new_cols < 2000000000)
				ef_cols = new_cols;
			else
				ef_cols = GetCols();
			if (new_rows > 0 && new_rows < 2000000000)
				ef_rows = new_rows;
			else
				ef_rows = GetRows();
			EmptyCellsSize = -1;
		}
		void ResetBot(int cols_ = -1, int rows_ = -1){	//Сбросить всю игровую информацию бота
			SetBotFieldSize(cols_, rows_);
			LastShotIndex = -1;
			LastShotResult = -1;
			solution = -1;
			State = Searching;
			ResetAboutShipInfo();
			FirstHitIndex = -1;
			EmptyCellsSize = -1;
		}
	
	//Работа с клетками/значениями клеток поля
	private:
		bool IsInField(int index){	//В пределах ли поля для стрельбы координата; true - да; false - нет
			return !(index < 0 || index >= (ef_cols * ef_rows));
		}
		bool IsInFieldHorizontal(int x, int y){	//В пределах ли поля для стрельбы координата; true - да; false - нет
			return !((x < 0 || x >= ef_cols) || (y < 0 || y >= ef_rows));
		}
		bool IsMayShotTo(const SeaBattleField &enemy_field, int index){	//Имеет ли смысл выстрела по данному индексу; true - да; false - нет
			return (IsInField(index) && (CheckCell(enemy_field, index) == 0));
		}
		bool IsMayShotToHorizontal(const SeaBattleField &enemy_field, int x, int y){	//Имеет ли смысл выстрела по данной координате; true - да; false - нет
			return CheckCellHorizontal(enemy_field, x, y) == 0;
		}
		int CheckCell(const SeaBattleField &enemy_field, int index){	//-1 - за пределами поля; 0 - пустота (или, возможно, корабль); 1 - стреленая клетка; 2 - раненый корабль; 3 - взорванный корабль
			int value = enemy_field.LookAtCellByIndex(index);
			if (enemy_field.GetValueOfValueMean(0) == value)
				return 0;
			else if (enemy_field.GetValueOfValueMean(1) == value)
				return 1;
			else if (enemy_field.GetValueOfValueMean(2) == value)
				return 2;
			else if (enemy_field.GetValueOfValueMean(3) == value)
				return 3;
			else
				return -1;
		}
		int CheckCellHorizontal(const SeaBattleField &enemy_field, int x, int y){	//-1 - за пределами поля; 0 - пустота (или, возможно, корабль); 1 - стреленая клетка; 2 - раненый корабль; 3 - взорванный корабль
			if (!IsInFieldHorizontal(x, y))
				return -1;
			int index = y * ef_cols + x;
			int value = enemy_field.LookAtCellByIndex(index);
			if (enemy_field.GetValueOfValueMean(0) == value)
				return 0;
			else if (enemy_field.GetValueOfValueMean(1) == value)
				return 1;
			else if (enemy_field.GetValueOfValueMean(2) == value)
				return 2;
			else if (enemy_field.GetValueOfValueMean(3) == value)
				return 3;
			else
				return -1;
		}
	
	//Определение координаты для уничтожения корабля
	private:
		void InitializeEmptyCells(const SeaBattleField &enemy_field){
			int sum = 0;
			if (EmptyCells != 0)
				delete[] EmptyCells;
			EmptyCells = new int[ef_cols * ef_rows];
			for(int i = 0; i < ef_cols * ef_rows; i++){
				if (CheckCell(enemy_field, i) == 0){
					EmptyCells[sum] = i;
					sum++;
				}
			}
			EmptyCellsSize = sum;
		}
		void DeleteEmptyCellsByIndex(int index_of_empty_cell){
			int i = 0;
			for(; i < index_of_empty_cell + 1; i++){
				if (EmptyCells[i] == index_of_empty_cell)
					break;
			}
			for(; i < EmptyCellsSize - 1; i++)
				EmptyCells[i] = EmptyCells[i + 1];
			EmptyCellsSize--;
		}
		void ReverseShootingSide(){	//Развернуть уничтожение корабля на противоположную сторону
			if (ShootingRight)
				solution = (Rotation == Vertical ? FirstHitIndex - ef_cols : FirstHitIndex - 1);	//Выстрелить левее (выше) первой найденной клетки
			else
				solution = (Rotation == Vertical ? FirstHitIndex + ef_cols : FirstHitIndex + 1);	//Выстрелить правее (ниже) первой найденной клетки
			ShootingRight = !ShootingRight;
		}
		int FindOutShipRotation(const SeaBattleField &enemy_field){	//Узнать ориентацию найденного корабля; 1 - узнал ориентацию; 2 в процессе определения
			int  Up = FirstHitIndex - ef_cols, Down = FirstHitIndex + ef_cols, Right = (FirstHitIndex % ef_cols) + 1, Left = (FirstHitIndex % ef_cols) - 1, y = FirstHitIndex / ef_cols;	//Следующие ближайшие клетки в каждом направлении
			if (CheckCell(enemy_field, Up) == 2 || CheckCell(enemy_field, Down) == 2){	//Если выше или ниже был ранен корабль
				Rotation = Vertical;
				//cout << "Выше или ниже есть раненый корабль\n";
				return 1;
			}
			else
				if (!(IsMayShotTo(enemy_field, Up) || IsMayShotTo(enemy_field, Down))){	//Может ли выстрелить выше/ниже
					Rotation = Horizontal;
					//cout << "Не могу выстрелить выше/ниже\nЗначения верха и низа: ";
					//cout << CheckCell(enemy_field, Up) << CheckCell(enemy_field, Down) << endl;
					return 1;
				}
		
			if (CheckCellHorizontal(enemy_field, Left, y) == 2 || CheckCellHorizontal(enemy_field, Right, y) == 2){	//Если левее или правее был ранен корабль
				Rotation = Horizontal;
				//cout << "Левее или правее есть раненый корабль\n";
				return 1;	
			}	
			else
				if (!(IsMayShotToHorizontal(enemy_field, Left, y) || IsMayShotToHorizontal(enemy_field, Right, y))){	//Может ли выстрелить левее/правее
					Rotation = Vertical;
					//cout << "Не могу выстрелить левее/правее\n";
					return 1;
				}
			
			if (IsMayShotTo(enemy_field, Down)){
				ShootingRight = true;
				solution = Down;
			}
			else if (IsMayShotTo(enemy_field, Up)){
				ShootingRight = false;
				solution = Up;
			}
			else if (IsMayShotToHorizontal(enemy_field, Right, y)){
				ShootingRight = true;
				solution = y * ef_cols + Right;
			}
			else if (IsMayShotToHorizontal(enemy_field, Left, y)){
				solution = y * ef_cols + Left;
				ShootingRight = false;
			}
			
			if (Rotation == Unknown)
				return 2;
			else
				return 3;
		}
		void RecordBotMoveData(int *x = 0, int *y = 0){	//Записать в переданные аргументы значения от solution
			if (x)
				*x = solution % ef_cols;
			if (y)
				*y = solution / ef_cols;
			LastShotIndex = solution;
		}
	public:
		void ShotByBot(const SeaBattleField &enemy_field, int *x, int *y){ //Вычисления для хода, куда бот будет стрелять; В x и y будут записаны координаты для выстрела;
			int temp = 0, temp2;
			if (EmptyCellsSize = -1)
				InitializeEmptyCells(enemy_field);
			LastShotResult = enemy_field.CheckLastMove();	//0 - ходы отсутствуют; 1 - был выстрел по SHOT, STRIKE, KILL; 2 - было попадание в SHIP; 3 - был подрыв корабля; 4 - был выстрел по EMPTY; 5 - была установка корабля;
			switch(LastShotResult){	//Результат его последнего хода
				case 2:	//Попал в клетку корабля
					if (State == Searching){	//Только-только обнаружил корабль
						State = Destruction;
						FirstHitIndex = LastShotIndex;
					}

					break;
				case 3:	//Взорвал корабль
					if (State == Destruction){	//Взорвал корабль длиной в >1 клетку
						ResetAboutShipInfo();
						State = Searching;
						FirstHitIndex = -1;
					}
					InitializeEmptyCells(enemy_field);
					break;
				/*
				case 4:	//Попал по пустой клетке
					
					break;
				case 5:	//Была установка кораблей, это первый ход
					solution = 21;
					RecordBotMoveData(x, y);
					return;
				*/
			}
			
			switch(State){
				case Searching:	//Поиск какого-либо корабля
					
					srand(clock());
					solution = EmptyCells[rand() % EmptyCellsSize];
					
					
					
					/*
					temp = ((unsigned int)rand()) % ef_cols;
					//srand(clock() + 1);
					srand(clock() * 3 + 5);
					//solution = (((unsigned int)temp) + rand() % (ef_cols * ef_rows));
					solution = (temp + ef_cols * (((unsigned int)rand()) % ef_rows));
					for(int i = 0; i <= ef_cols * ef_rows; solution++, i++){
						if (solution >= ef_cols * ef_rows)
							solution = 0;
						if (CheckCell(enemy_field, solution) == 0)	//Чтобы выстрелить в пустую клетку
							break;
					}
					*/
					break;
				case Destruction:	//Уничтожение найденного корабля
					if (Rotation == Unknown){	//Попытка узнать направление корабля
						temp = FindOutShipRotation(enemy_field);	//Возвращает 1, 2, 3
						if (temp == 2 || temp == 3)	//значение в solution было записано в FIndOutShipRotation
							break;
					}
					
					//Определение следующей клетки для выстрела
					if (Rotation == Horizontal){	
						temp2 = (LastShotResult == 2 ? (LastShotIndex / ef_cols) : (FirstHitIndex / ef_cols));
						solution = ((LastShotResult == 2 ? (LastShotIndex % ef_cols) : (FirstHitIndex % ef_cols)) + (ShootingRight ? 1 : -1));
					}
					else	//Rotation == Vertical
						solution = ((LastShotResult == 2 ? LastShotIndex : FirstHitIndex) + (ShootingRight ? ef_cols : -ef_cols));
					
					if ((LastShotResult == 4 && temp == 0) || (Rotation == Horizontal ? (!IsMayShotToHorizontal(enemy_field, solution, temp2)) : (!IsMayShotTo(enemy_field, solution))))	//Попал в пустую клетку или следующая клетка вне поля поля/не имеет смысла в неё стрелять
						ReverseShootingSide();
					else
						solution = (Rotation == Horizontal ? temp2 * ef_cols + solution : solution);
					break;
			}
			DeleteEmptyCellsByIndex(solution);
			RecordBotMoveData(x, y);
		}
	
	
	//Работа с файлами
	private:
		bool HaveFormatInFileName(string file_name, string file_format = ".txt"){
			if (file_name.length() < 4)
				return false;
			return file_name.compare(file_name.length() - 4, 4, file_format) == 0;
		}
	//Загрузка из файла
	public:
		bool BotLoadFromFile(string file_name){	//Загрузить данные бота из файла; 0 - успешно; 1 - некорректное название/ошибка открытия файла; 2 - ошибка чтения
			if (file_name.length() < 3)
				return 1;
			if (!HaveFormatInName(file_name, ".txt"))
				file_name += ".txt";
			
			ifstream file(file_name.c_str());	//Открываем файл для чтения
			if (!file.is_open())
				return 1;
			
			ResetBot();
			int temp, shr;
			if (
				!BotReadFindString(file, "SeaBattleBot") ||
				!BotReadElement(file, "efc", &ef_cols) ||
				!BotReadElement(file, "efr", &ef_rows) ||
				!BotReadElement(file, "sta", &temp)
				)	//Ошибка чтения
					return 2;
			
			State = (BotState)temp;
			if (State == Destruction)	//Если бот в состоянии уничтожения корабля
				if (
					!BotReadElement(file, "lsi", &LastShotIndex) ||
					!BotReadElement(file, "lsr", &LastShotResult) ||
					!BotReadElement(file, "rot", &temp) ||
					!BotReadElement(file, "fhi", &FirstHitIndex) ||
					!BotReadElement(file, "shr", &shr)
					)	//Ошибка чтения
						return 2;
			
			Rotation = (ShipRotation)temp;
			ShootingRight = (bool)shr;
			EmptyCellsSize = -1;
			return 0;
		}
	private:
		bool BotReadFindString(ifstream &flow_name, string your_string){	//Найти строку в файле; true - удалось; false - не удалось
			string str;
			while (getline(flow_name, str)){
				if (flow_name.fail() && !flow_name.eof())
	                return false;
	            if (str.find(your_string) != string::npos)
	                return true;
            }
        	return false;
		}
		bool BotReadIsCharEqualsReaded(ifstream &flow_name, char your_char){	//Равен ли считанный символ переданному; true - равны; false - не равны
			return (flow_name.get() == your_char);
		}
		bool BotReadIsStringEquals(ifstream &flow_name, string your_string){	//Совпадает ли ожидаемое имя элемента со считанным; true - совпадает; false - не совпадает
			for(int i = 0; i < your_string.length(); i++)
				if (your_string[i] != flow_name.get())
					return false;
			return true;
		}
		bool BotReadElement(ifstream &flow_name, string element_name, int *element){	//Считать из файла элемент в кавычках; true - успешно; false - ошибка чтения
			if (!BotReadIsStringEquals(flow_name, element_name + "=\"") || !BotReadNumber(flow_name, element) || !BotReadIsCharEqualsReaded(flow_name, '"') || !BotReadIsCharEqualsReaded(flow_name, '\n'))
				return false;
			return true;
		}
		bool BotReadNumber(ifstream &flow_name, int *element){	//Считать из файла элемент в кавычках; true - успешно; false - ошибка чтения
			return (flow_name >> *element).good();
		}
	//Сохранение в файл
	public:
		bool BotSaveToFile(string file_name){	//Сохранить данные бота в файл
			if (file_name.length() < 3)
				return 1;
			if (!HaveFormatInName(file_name, ".txt"))
				file_name += ".txt";
				
			ofstream file(file_name.c_str(), ios::app);	//Открываем файл для записи
			if (!file.is_open())
				return 1;
			if (
				!BotRecordString(file, "SeaBattleBot\n") ||
				!BotRecordElement(file, "efc", ef_cols) ||
				!BotRecordElement(file, "efr", ef_rows) ||
				!BotRecordElement(file, "sta", (int)State)
				){	//Ошибка записи
					return 2;
				}
			if (State == Destruction){	//Если бот в состоянии уничтожения корабля
				if (
					!BotRecordElement(file, "lsi", LastShotIndex) ||
					!BotRecordElement(file, "lsr", LastShotResult) ||
					!BotRecordElement(file, "rot", (int)Rotation) ||
					!BotRecordElement(file, "fhi", FirstHitIndex) ||
					!BotRecordElement(file, "shr", ShootingRight)
					){	//Ошибка записи
						return 2;
					}
			}
			return 0;
		}
	private:
		bool BotRecordString(ofstream &flow_name, string your_string){	//Записать в файл строку; true - успешно; false - ошибка записи
			return (flow_name << your_string).good();
		}
		bool BotRecordElement(ofstream &flow_name, string element_name, int number){	//Записать в файл элемент в кавычки; true - успешно; false - ошибка записи
			return (flow_name << element_name << "=\"" << number << "\"\n").good();
		}
};















class SeaBattleGameMenu {
private:
    SeaBattleGame* player1;
    SeaBattleBot* player2;
    int player1_color;
    int player2_color;
    bool IsModeVsComputer;
    bool player_turn;
    
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
    
    bool Multiplayer(){
    	cout << "Создание сетевого\n";
	    P2PMessenger SeaNet;
	    //SeaNet.run();
    	//SeaBattleGameNetwork SeaNet;
    	//std::cout << "Мой публичный IP: " << SeaNet.get_public_ip() << std::endl;
		//std::cout << "Мой код друга: " << SeaNet.get_my_code() << std::endl;
		
    	cout << "Создание сетевого закончено\n";
    	int choice = -1;
    	std::string message;
    	std::string connect_ip = "127.0.0.1";
    	unsigned int rem_p = SeaNet.remote_port_;
		while (true) {
            cout << "Вы находитесь в меню сетевой игры. Введите номер того действия, которое хотите выполнить.\n";
            
            cout << "Публичный код друга: " << SeaNet.get_public_code() << " и IP: " << SeaNet.get_public_ip() << "\n";
            cout << "Локальный код друга: " << SeaNet.get_local_code() << " и IP: " << SeaNet.get_local_ip() << "\n";
            cout << "0. Подключиться\n"
            	<< "1. Отправить сообщение\n"
				<< "2. К какому порту подключаться? (Текущий: " << rem_p << ")\n"
				<< "\tОжидает подключения на: (" << SeaNet.listen_port_ << ")\n"
				<< "3. Применить код друга\n" 
				<< "4. К какому IP подключаться (Текущий: " << connect_ip << ")\n";
            
            
            if (!(cin >> choice) || choice < 0 || choice > 5) {
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
