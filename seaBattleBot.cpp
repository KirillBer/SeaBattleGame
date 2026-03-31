#ifndef SEABATTLEBOT_BM_CPP
#define SEABATTLEBOT_BM_CPP

#include "seaBattleGame.cpp"

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
		bool HaveFormatInFileName(std::string file_name, std::string file_format = ".txt"){
			if (file_name.length() < 4)
				return false;
			return file_name.compare(file_name.length() - 4, 4, file_format) == 0;
		}
	//Загрузка из файла
	public:
		bool BotLoadFromFile(std::string file_name){	//Загрузить данные бота из файла; 0 - успешно; 1 - некорректное название/ошибка открытия файла; 2 - ошибка чтения
			if (file_name.length() < 3)
				return 1;
			if (!HaveFormatInName(file_name, ".txt"))
				file_name += ".txt";
			
			std::ifstream file(file_name.c_str());	//Открываем файл для чтения
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
		bool BotReadFindString(std::ifstream &flow_name, std::string your_string){	//Найти строку в файле; true - удалось; false - не удалось
			std::string str;
			while (getline(flow_name, str)){
				if (flow_name.fail() && !flow_name.eof())
	                return false;
	            if (str.find(your_string) != std::string::npos)
	                return true;
            }
        	return false;
		}
		bool BotReadIsCharEqualsReaded(std::ifstream &flow_name, char your_char){	//Равен ли считанный символ переданному; true - равны; false - не равны
			return (flow_name.get() == your_char);
		}
		bool BotReadIsStringEquals(std::ifstream &flow_name, std::string your_string){	//Совпадает ли ожидаемое имя элемента со считанным; true - совпадает; false - не совпадает
			for(int i = 0; i < your_string.length(); i++)
				if (your_string[i] != flow_name.get())
					return false;
			return true;
		}
		bool BotReadElement(std::ifstream &flow_name, std::string element_name, int *element){	//Считать из файла элемент в кавычках; true - успешно; false - ошибка чтения
			if (!BotReadIsStringEquals(flow_name, element_name + "=\"") || !BotReadNumber(flow_name, element) || !BotReadIsCharEqualsReaded(flow_name, '"') || !BotReadIsCharEqualsReaded(flow_name, '\n'))
				return false;
			return true;
		}
		bool BotReadNumber(std::ifstream &flow_name, int *element){	//Считать из файла элемент в кавычках; true - успешно; false - ошибка чтения
			return (flow_name >> *element).good();
		}
	//Сохранение в файл
	public:
		bool BotSaveToFile(std::string file_name){	//Сохранить данные бота в файл
			if (file_name.length() < 3)
				return 1;
			if (!HaveFormatInName(file_name, ".txt"))
				file_name += ".txt";
				
			std::ofstream file(file_name.c_str(), std::ios::app);	//Открываем файл для записи
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
		bool BotRecordString(std::ofstream &flow_name, std::string your_string){	//Записать в файл строку; true - успешно; false - ошибка записи
			return (flow_name << your_string).good();
		}
		bool BotRecordElement(std::ofstream &flow_name, std::string element_name, int number){	//Записать в файл элемент в кавычки; true - успешно; false - ошибка записи
			return (flow_name << element_name << "=\"" << number << "\"\n").good();
		}
};

#endif
