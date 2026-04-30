#ifndef SEABATTLEFIELD_BM_H
#define SEABATTLEFIELD_BM_H


//#define SEABATTLEFIELD_BM_H_DEV_MODE

#include <vector>	//Для сохранения и отката ходов
#include <cstring>	//Для работы с файлами
#include <fstream>	//Для работы с файлами

class SeaBattleField{	//Механика и логика поля и кораблей
	//Обозначения значений в клетке поля: EMPTY, SHOT, STRIKE, KILL, SHIP
	
		//Значения
	private:
		struct CellValueChange{	//Хранение изменения состояния клетки при ходе
		    int coordinate_index;	//Индекс клетки
		    unsigned char prevState;	//Состояние клетки до изменения
		    unsigned char newState;		//Состояние клетки после изменения
		    
		    bool operator==(const CellValueChange& other) const{
				return ((coordinate_index == other.coordinate_index) && (prevState == other.prevState) && (newState == other.newState));
			}
			bool operator!=(const CellValueChange& other) const{
				return (!(*this == other));
			}
		};
		int cols, rows;	//Допустимы значения от 1 до 100 миллионов, но cols * rows до 1 миллиарда
		unsigned char *field = 0;	//Допустимы значения от 0 до 255
		unsigned char *field_value_mean = 0;	//Допустимы значения от 0 до 255
		unsigned char *ships = 0;	//ships[0] - максимальная длина корабля, ships[от 1 до 255] - кол-во кораблей. Макс. кол-во кораблей любой длины - 255
		unsigned char *ships_remain = 0;	//копия ships, однако ships_remain является оставшимся кол-вом кораблей в игре, а не настройкой игры
		std::vector<std::vector<CellValueChange> > moves;	//Хранение ходов
	
	
		//Конструкторы и перегрузки операторов
	public:	
		SeaBattleField(int cols_ = 10, int rows_ = 10);
		~SeaBattleField();
		SeaBattleField(const SeaBattleField& other);
		SeaBattleField &operator=(const SeaBattleField& other);
		bool operator==(const SeaBattleField& other) const;
		bool operator!=(const SeaBattleField& other) const;
		
		//Получение хранящихся значений
	public:
		const int GetRows() const;	//Возвращает значение rows
		const int GetCols() const;	//Возвращает значение cols
		const int GetLastMoveIndex() const;	//Возвращает индекс поля, в который был сделан последний ход
		const int GetMovesSize() const;	//Возвращает количество ходов
		const int GetMoveSize(int move_index) const;	//Возвращает количество ходов в ходе с индексом move_index
		const int GetValueOfValueMean(int type_position) const;	//Если type_position[0, 4] - вернётся текущее значение типа, иначе вернётся 1111 (невозможное значение)
		const int GetShipsRemainCountOfNLen(int n) const;	//[1, (ship_max_len - 1)] - количество кораблей; -1 - ошибка длины корабля
		const int GetMaxShipLen() const;	//Возвращает длину самого большого корабля
		const int GetCountOfShips() const;	//Возвращает сумму кораблей, которое будет в игре
		const int GetCountOfShipsRemain() const;	//Возвращает сумму оставшихся кораблей в игре
		const int GetCellsOfShipsSum() const;	//Возвращает сумму клеток кораблей в которое будет в игре
		const int GetCellsOfShipsRemainSum() const;	//Возвращает сумму клеток не взорванных кораблей, оставшееся в игре
		const int LookAtCellByIndex(int index) const;	//Возвращает хранящееся значение в клетке (при клетке с живым кораблём возвращает значение пустой клетки), иначе -1
	protected:
	#ifdef SEABATTLEFIELD_BM_H_DEV_MODE
	public:
	#endif
		int GetValueOfCellByIndex(int index);	//Возвращает хранящееся значение в клетке, иначе -1
		int GetValueOfCell(int col_index, int row_index);	//Возвращает хранящееся значение в клетке, иначе -1
		
		
		//Сбросы значений до стандартных
	public:
		void ClearField();	//Каждая клетка принимает значение EMPTY этого поля
		void ResetField();	//Сбросить размер поля до размера 10 на 10, удалив ходы и значения в клетках
		void ResetFieldValueMean();	//Значения типов клеток поля становятся стандартными (EMPTY - 0, SHOT - 1, ..., SHIP - 4)
		void ResetShips();	//Количество кораблей становится равно 4 1-палубных, ..., 1 4-палубный
		void ResetShipsRemain();	//Значения ships_remain становятся равны значениям ships
		void ResetMoves();	//Удалить все сделанные ходы и очистить значения на поле
		void Reset();	//Сброс значений объекта для новой игры с тем же количеством кораблей и размере поля
		void FullReset();	//Полный сброс объекта до значений по умолчанию
		
		
		//Изменения значений для игры
	public:
		int ChangeFieldValueMean(int EMPTY = 0, int SHOT = 1, int STRIKE = 2, int KILL = 3, int SHIP = 4);	//Изменить значения, которыми обозначаются клетки в игре; 0 - успешно; 1 - ошибка, одинаковые значения; 2 - недопустимое значение (<0 или >255)
		int ChangeFieldSize(int new_cols_count, int new_rows_count);	//Изменить размер поля; 0 - успешно; 1 - ошибка выделения памяти; 2 - некорректные размеры поля; 3 - новые размеры равны предыдущим
		int ChangeShipLenCount(int ship_len, int new_ship_count);	//Изменяет максимум возможных кораблей определённой длины. 0 - успешно; 1 - ошибка значений
		int ChangeMaxShipLen(int new_max_len);	//Изменяет максимальную возможную длину корабля. 0 - успешно; 1 - ошибка выделения памяти; 2 - недопустимая максимальная длина
		
		
		//Работа с сохраняемыми ходами
	private:
		void MakeMove();	//Создаёт новый ход
		void AddCellValueChange(int index, unsigned char prev_value, unsigned char new_value);	//Добавляет в последний ход изменение ячейки
		void TransformByIndexValueChange(int index, unsigned char prev_value, unsigned char new_value);	//Ищет в последнем ходе по index клетку и меняет prev и new значения на новые
		bool RegenerateFieldByMoves();	//Восстановить поле по ходам; true - успешно; false - ошибка
	public:
		int CancelLastMove();	//Откатывает изменения за последний ход; Возвращает: 0 - ходы отсутствуют; 1 - был выстрел по SHOT, STRIKE, KILL; 2 - было попадание в SHIP; 3 - был подрыв корабля; 4 - был выстрел по EMPTY; 5 - была установка корабля;
		const int CheckLastMove() const; //"Подсмотреть", что делал последний ход; Возвращает то же, что и CancelLastMove()
		
		
		//Чтение из файла
	public:
		int LoadFromFile(std::string file_name, bool is_it_second_field = false);	//Загрузка из файла; 0 - успешно; 1 - некорректное название/ошибка открытия файла; 2 - ошибка чтения
	private:
		bool ReadFindString(std::ifstream &flow_name, std::string your_string);	//Найти строку в файле; true - удалось; false - не удалось
		bool ReadIsCharEqualsReaded(std::ifstream &flow_name, char your_char);	//Равен ли считанный символ переданному; true - равны; false - не равны
		bool ReadArray(std::ifstream &flow_name, std::string element_name, unsigned char **array);	//Считать из файла элемент в кавычках; true - успешно; false - ошибка чтения
		bool ReadIsStringEquals(std::ifstream &flow_name, std::string your_string);	//Совпадает ли ожидаемое имя элемента со считанным; true - совпадает; false - не совпадает
		bool ReadElement(std::ifstream &flow_name, std::string element_name, int *element);	//Считать из файла элемент в кавычках; true - успешно; false - ошибка чтения
		bool ReadChar(std::ifstream &flow_name, unsigned char *element);	//Считать из файла символ; true - успешно; false - ошибка чтения
		bool ReadNumber(std::ifstream &flow_name, int *element);	//Считать из файла элемент в кавычках; true - успешно; false - ошибка чтения
		bool ReadMoves(std::ifstream &flow_name);	//Считать из файла ходы; true - успешно; false - ошибка чтения
		
		
		//Запись в файл
	public:
		bool HaveFormatInName(std::string file_name, std::string file_format = ".txt");
		int SaveToFile(std::string file_name, bool append = false); //Сохранение в файл; 0 - успешно; 1 - некорректное название/ошибка открытия файла; 2 - ошибка записи
	private:
		bool RecordString(std::ofstream &flow_name, std::string your_string);	//Записать в файл строку; true - успешно; false - ошибка записи
		bool RecordNumber(std::ofstream &flow_name, int number);	//Записать в файл число; true - успешно; false - ошибка записи
		bool RecordElement(std::ofstream &flow_name, std::string element_name, int number);	//Записать в файл элемент в кавычки; true - успешно; false - ошибка записи
		bool RecordArray(std::ofstream &flow_name, std::string element_name, unsigned char *array, int array_size);	//Записать в файл массив в скобках; true - успешно; false - ошибка записи
		bool RecordMoves(std::ofstream &flow_name);	//Записать в файл ходы; true - успешно; false - ошибка записи
		
		
		//Работа с кораблями
	private:
		void SetInSide(unsigned char set_what, int side, int len, int x, int y);	//ставит set_what, len клеток в направлении side
		int CheckOneBlock(int x, int y) const;	//0 - всё в порядке / координата за пределами поля; >0 - кол-во кораблей
		int CheckSide(int side, int len, int x, int y) const;	//0 - всё в порядке; 1 - за пределами поля / корабли
		int FindShipEdge(int *x, int *y);	//Возвращает направление корабля и в *x и *y запишет координаты крайней клетки корабля
		int CalculateWhatInSide(int x, int y, int side, int what);	//считает количество what в направлении side до EMPTY или SHOT клетки
		int SetShotAroundShips(int side, int len, int x, int y);	//возвращает итоговое кол-во уст. SHOT
		int ShotToMiddleOfSide(int side, int x, int y, int len);	//возвращает количество установленных SHOT
		int ShotToWallOfSide(int side, int x, int y, int back);	//возвращает количество установленных SHOT
	public:
		int ShotTo(int x, int y);	//Выстрелить; 0 - попадание по SHIP; 1 - подрыв последнего корабля; 2 - выстрел в пустую клетку; 3 - бессмысленный выстрел, попадание в SHOT, STRIKE, KILL; 4 - ошибка координаты; 5 - ошибка значения ячейки
		int SetShip(int ship_len, int side, int x, int y); //Установить корабль; 0 - успешно; 1 - ошибка аргумента; 2 - ошибка установки первой клетки; 3 - ошибка установки в направлении
		const int AvailableSides(int x, int y, int ship_len) const; //возвращает доступные стороны закодированные числом (верх - 1б право - 10, низ - 100, лево - 1000)
		const int IsSideAvailable(int x, int y, int ship_len, int side) const;	//Проверяет доступность стороны side для установки корабля. Возвращает: -1 - некорректная координата; 0 -доступна; 1 - недоступна (side =: 0 - верх; 1 - право; 2 - низ; 3 - лево);
		
		
		//Перевод объекта в строковый формат
	private:
		std::string MoveToString_(int move_index, bool with_state_change);	//Приватный метод MoveToString
	public:
		std::string MoveToString(int move_index);	//Вернуть изменения всего хода как строку
		std::string MovesToString();	//Вернуть изменения всех ходов как строку
		std::string FieldToString();	//Записать поле и ходы в виде строки
		std::string ShipsToString();	//Записать ships в виде строки
		std::string ShipsRemainToString();	//Записать ships_remain в виде строки
		std::string ToString();	//Записать весь объект в виде строки
		bool FieldFromString(std::string& field_string);	//Восстановить поле и ходы из строки
		bool ShipsFromString(std::string& ships_string);	//Восстановить ships из строки
		bool ShipsRemainFromString(std::string& ships_remain_string);	//Восстановить ships_remain из строки
		bool FromString(std::string string_data);	//Восстановить весь объект из строки
		
		
		//Необходимые для работы методы
	private:
		const int IntPow(int x, int y) const;	//Возвести x в степень y
		const int IsInCharRange(int number) const;	//0 - в диапазоне; 1 - за диапазоном
		const int IsNewSizeAvailable(int cols_, int rows_) const;	//0 - доступно; 1 - недоступно
		const int MemoryAllocationError(int error_type) const;	//Значения error_type: 0 - критически важный объект; 1 - не критически важный объект
		const int XYChangesBySide(int x_is_0_y_is_1, int side) const;	//значения изменения x, y в зависимости от стороны
		const int IsOneOfFieldValueMean(unsigned char number) const;	//0 - является каким-либо значением из field_value_mean; 1 - не является
		const int StateToStandard(unsigned char value) const;	//Перевести число из value в значение из field_value_mean
		const int StandardToState(unsigned char value) const;	//Перевести число из индекса field_value_mean в соответствующее значение из field_value_mean
		const int StrToInt(std::string& text, bool change_string = true);	//Вернуть первое число из начала строки
	public:
		const void Print();
};

#endif
