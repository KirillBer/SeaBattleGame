#include "seaBattleField.h"


	//Конструкторы и перегрузки операторов
		SeaBattleField::SeaBattleField(int cols_, int rows_): cols(cols_), rows(rows_){
			field = 0;
			field_value_mean = 0;
			ships = 0;
			ships_remain = 0;
			if (IsNewSizeAvailable(cols_, rows_))
			{
				cols = 10;
				rows = 10;
			}
			field = new unsigned char[cols * rows];
			field_value_mean = new unsigned char[5];
			ships = new unsigned char[5];
			ships_remain = new unsigned char[5];
			if (!field || !field_value_mean || !ships || !ships_remain)
				MemoryAllocationError(0);
			ClearField();
			ResetFieldValueMean();
			ResetShips();
			ResetShipsRemain();
		}
		SeaBattleField::~SeaBattleField(){
			if (field)
				delete[] field;
			if (field_value_mean)
				delete[] field_value_mean;
			if (ships)
				delete[] ships;
			if (ships_remain)
				delete[] ships_remain;
		}
		SeaBattleField::SeaBattleField(const SeaBattleField& other){
			*this = other;
		}
		SeaBattleField & SeaBattleField::operator=(const SeaBattleField& other){
			if (this != &other){
				field = new unsigned char[1];
				field_value_mean = new unsigned char[5];
				ships = new unsigned char[1];
				ships_remain = new unsigned char[1];
				if (!field || !field_value_mean || !ships || !ships_remain)
					MemoryAllocationError(0);
				//Копируем данные из другого объекта
	            ChangeFieldValueMean(other.field_value_mean[0], other.field_value_mean[1], other.field_value_mean[2], other.field_value_mean[3], other.field_value_mean[4]);
				ChangeFieldSize(other.cols, other.rows);
				for(int i = cols * rows - 1; i >= 0; i--)
	            	field[i] = other.field[i];
	            ChangeMaxShipLen(other.ships[0]);
				for(int i = 1; i <= other.ships[0]; i++)
					ChangeShipLenCount(i, other.ships[i]);
				moves = other.moves;
	        }
	        return *this;	//Возвращаем текущий объект
	    }
		
		
	//Получение хранящихся значений
		const int SeaBattleField::GetRows() const{	//Возвращает значение rows
			return rows;
		}
		const int SeaBattleField::GetCols() const{	//Возвращает значение cols
			return cols;
		}
		const int SeaBattleField::GetValueOfValueMean(int type_position) const{	//Если type_position[0, 4] - вернётся текущее значение типа, иначе вернётся 1111 (невозможное значение)
			if (type_position < 0 || type_position > 4)
				return 1111;
			return field_value_mean[type_position];
		}
		const int SeaBattleField::GetShipsRemainCountOfNLen(int n) const{	//[1, (ship_max_len - 1)] - количество кораблей; -1 - ошибка длины корабля
			if (n < 1 || n > ships[0])
				return -1;
			return ships_remain[n]; // возврат кол-ва оставшихся кораблей длины n
		}
		const int SeaBattleField::GetMaxShipLen() const{	//Возвращает длину самого большого корабля
			return ships[0];
		}
		const int SeaBattleField::GetCountOfShips() const{	//Возвращает сумму кораблей, которое будет в игре
			int sum = 0;
			for(int i = 1; i <= ships[0]; i++)
				sum += ships[i];
			return sum;
		}
		const int SeaBattleField::GetCountOfShipsRemain() const{	//Возвращает сумму оставшихся кораблей в игре
			int sum = 0;
			for(int i = 1; i <= ships_remain[0]; i++)
				sum += ships_remain[i];
			return sum;
		}
		const int SeaBattleField::GetCellsOfShipsSum() const{	//Возвращает сумму клеток кораблей в которое будет в игре
			int sum = 0;
			for(int i = 1; i <= ships[0]; i++)
				sum += (ships[i] * i);
			return sum;
		}
		const int SeaBattleField::GetCellsOfShipsRemainSum() const{	//Возвращает сумму клеток не взорванных кораблей, оставшееся в игре
			int sum = 0;
			for(int i = cols * rows - 1; i >= 0; i--)
				if (field[i] == field_value_mean[4] || field[i] == field_value_mean[2])
					sum += 1;
			return sum;
		}
		const int SeaBattleField::LookAtCellByIndex(int index) const{	//Возвращает хранящееся значение в клетке (при клетке с живым кораблём возвращает значение пустой клетки), иначе -1
			if (index < 0 || index >= rows * cols)
				return -1;
			if (field[index] == field_value_mean[4])
				return field_value_mean[0];
			return field[index];
		}
		int SeaBattleField::GetValueOfCellByIndex(int index){	//Возвращает хранящееся значение в клетке, иначе -1
			if (index < 0 || index >= rows * cols)
				return -1;
			return field[index];
		}
		int SeaBattleField::GetValueOfCell(int col_index, int row_index){	//Возвращает хранящееся значение в клетке, иначе -1
			if ((col_index < 0 || row_index < 0) || (col_index >= cols || row_index >= rows))
				return -1;
			return field[row_index * rows + col_index];
		}
		
		
	//Сбросы значений до стандартных
		void SeaBattleField::ClearField(){	//Каждая клетка принимает значение EMPTY этого поля
			for(int i = cols * rows - 1; i >= 0; i--)
				field[i] = field_value_mean[0];
		}
		void SeaBattleField::ResetFieldValueMean(){	//Значения типов клеток поля становятся стандартными (EMPTY - 0, SHOT - 1, ..., SHIP - 4)
			for(int i = 0; i < 5; i++)
				field_value_mean[i] = i;
		}
		void SeaBattleField::ResetShips(){	//Количество кораблей становится равно 4 1-палубных, ..., 1 4-палубный
			unsigned char *t = 0;
			t = new unsigned char[5];
			if (!t)
				MemoryAllocationError(1);
			else
			{
				if (ships)
					delete[] ships;
				ships = t;
				ships[0] = 4;	//Макс. длина
				ships[1] = 4;
				ships[2] = 3;
				ships[3] = 2;
				ships[4] = 1;
			}
		}
		void SeaBattleField::ResetShipsRemain(){	//Значения ships_remain становятся равны значениям ships
			for(int i = 0; i <= ships[0]; i++)
				ships_remain[i] = ships[i];
		}
		void SeaBattleField::ResetField(){	//Удалить все сделанные ходы
			ClearField();
			while(moves.size() > 0)
				moves.pop_back();
		}
		void SeaBattleField::Reset(){
			ResetFieldValueMean();
			ResetShips();
			ResetShipsRemain();
			ResetField();
		}
		
	//Изменения значений для игры
		int SeaBattleField::ChangeFieldValueMean(int EMPTY, int SHOT, int STRIKE, int KILL, int SHIP){	//Изменить значения, которыми обозначаются клетки в игре; 0 - успешно; 1 - ошибка, одинаковые значения; 2 - недопустимое значение (<0 или >255)
			if (IsInCharRange(EMPTY) || IsInCharRange(SHOT) || IsInCharRange(STRIKE) || IsInCharRange(KILL) || IsInCharRange(SHIP))
				return 2;
			char temp[5] = {(char)EMPTY, (char)SHOT, (char)STRIKE, (char)KILL, (char)SHIP};
			for(int i = 0; i < 5; i++)	//Проверяется, если 2 разных типа клетки поля совпадают значением
				for(int n = 0; n < 5; n++)
					if (i != n)
						if (temp[i] == temp[n])
							return 1;
			for(int i = cols * rows - 1; i >= 0; i--)	//Замена старых значений типов в клетках на новые
				if (field[i] == field_value_mean[0])
					field[i] = EMPTY;
				else if (field[i] == field_value_mean[1])
					field[i] = SHOT;
				else if (field[i] == field_value_mean[2])
					field[i] = STRIKE;
				else if (field[i] == field_value_mean[3])
					field[i] = KILL;
				else if (field[i] == field_value_mean[4])
					field[i] = SHIP;
			
			for(int i = 0; i < moves.size(); i++)	//Замена старых значений типов в истории ходов на новые
				for(int n = 0; n < moves[i].size(); n++){
					if (moves[i][n].prevState == field_value_mean[0])	//Для предыдущего состояния клетки
						moves[i][n].prevState = EMPTY;
					else if (moves[i][n].prevState == field_value_mean[1])
						moves[i][n].prevState = SHOT;
					else if (moves[i][n].prevState == field_value_mean[2])
						moves[i][n].prevState = STRIKE;
					else if (moves[i][n].prevState == field_value_mean[3])
						moves[i][n].prevState = KILL;
					else if (moves[i][n].prevState == field_value_mean[4])
						moves[i][n].prevState = SHIP;
					
					if (moves[i][n].newState == field_value_mean[0])	//Для нового состояния клетки
						moves[i][n].newState = EMPTY;
					else if (moves[i][n].newState == field_value_mean[1])
						moves[i][n].newState = SHOT;
					else if (moves[i][n].newState == field_value_mean[2])
						moves[i][n].newState = STRIKE;
					else if (moves[i][n].newState == field_value_mean[3])
						moves[i][n].newState = KILL;
					else if (moves[i][n].newState == field_value_mean[4])
						moves[i][n].newState = SHIP;
				}
			
			field_value_mean[0] = EMPTY;
			field_value_mean[1] = SHOT;
			field_value_mean[2] = STRIKE;
			field_value_mean[3] = KILL;
			field_value_mean[4] = SHIP;
			return 0;
		}
		int SeaBattleField::ChangeFieldSize(int new_cols_count, int new_rows_count){	//Изменить размер поля; 0 - успешно; 1 - ошибка выделения памяти; 2 - некорректные размеры поля; 3 - новые размеры равны предыдущим
			if (IsNewSizeAvailable(new_cols_count, new_rows_count))
				return 2;
			if ((new_cols_count == cols) && (new_rows_count == rows))
				return 3;
			unsigned char *t = new unsigned char[new_cols_count * new_rows_count];
			if (!t)
				return MemoryAllocationError(1);
			if (field)
				delete[] field;
			field = t;
			cols = new_cols_count;
			rows = new_rows_count;
			ClearField();
			return 0;
		}
		int SeaBattleField::ChangeShipLenCount(int ship_len, int new_ship_count){	//Изменяет максимум возможных кораблей определённой длины. 0 - успешно; 1 - ошибка значений
			if ((ship_len < 1 || ship_len > ships[0]) || (new_ship_count < 0 || new_ship_count > 255))
				return 1;
			ships[ship_len] = new_ship_count;
			ships_remain[ship_len] = new_ship_count;
			return 0;
		}
		int SeaBattleField::ChangeMaxShipLen(int new_max_len){	//Изменяет максимальную возможную длину корабля. 0 - успешно; 1 - ошибка выделения памяти; 2 - недопустимая максимальная длина
			if (new_max_len < 1 || new_max_len > 255)
				return 1;
			unsigned char *temp = 0;
			temp = new unsigned char[new_max_len + 1];
			if (!temp)
				return MemoryAllocationError(1);
			temp[0] = new_max_len;
			for(int i = 1; i <= temp[0]; i++)	//Копирование в новый массив данных о кораблях и запись в новые ячейки 0
				if (i <= ships[0])
					temp[i] = ships[i];
				else
					temp[i] = 0;
			if (ships_remain)
				delete[] ships_remain;
			ships_remain = temp;
			temp = 0;
			temp = new unsigned char[new_max_len + 1];
			if (!temp)
				return MemoryAllocationError(1);
			for(int i = 0; i <= ships_remain[0]; i++)
				temp[i] = ships_remain[i];
			if (ships)
				delete[] ships;
			ships = temp;
			return 0;
		}
		
		
	//Работа с сохраняемыми ходами
		void SeaBattleField::MakeMove(){	//Создаёт новый ход
			std::vector<CellValueChange> move;
			moves.push_back(move);
		}
		void SeaBattleField::AddCellValueChange(int index, unsigned char prev_value, unsigned char new_value){	//Добавляет в последний ход изменение ячейки
			CellValueChange change = {index, prev_value, new_value};
        	moves[moves.size() - 1].push_back(change);
		}
		void SeaBattleField::TransformByIndexValueChange(int index, unsigned char prev_value, unsigned char new_value){	//Ищет в последнем ходе по index клетку и меняет prev и new значения на новые
			int i = 0, moves_index = (moves.size() - 1);
			int move_index = (moves[moves_index].size() - 1);
			while(index != moves[moves_index][i].coordinate_index && i <= move_index)
				i++;
			if (!(i > move_index))
			{
				moves[moves_index][i].prevState = prev_value;
				moves[moves_index][i].newState = new_value;
			}
		}
		int SeaBattleField::CancelLastMove(){	//Откатывает изменения за последний ход; Возвращает: 0 - ходы отсутствуют; 1 - был выстрел по SHOT, STRIKE, KILL; 2 - было попадание в SHIP; 3 - был подрыв корабля; 4 - был выстрел по EMPTY; 5 - была установка корабля;
			int result = 4;
			if (moves.size())
			{
				if (!(moves[moves.size() - 1].size()))
				{
					moves.pop_back();
					return 1;
				}
				for (int index, i = (moves[moves.size() - 1].size() - 1); i >= 0; i--)
	        	{
	        		index = moves[moves.size() - 1][i].coordinate_index;
	        		if (moves[moves.size() - 1][i].newState == field_value_mean[4])	//Стала SHIP
	        			result = 5;
					else if (moves[moves.size() - 1][i].newState == field_value_mean[2])	//Стала STRIKE
						result = 2;
					else if (moves[moves.size() - 1][i].newState == field_value_mean[3])	//Была SHIP
					{
	        			int x = (index % cols), y = (index / cols), side = FindShipEdge(&x, &y), len = CalculateWhatInSide(x, y, side, -1);
						ships_remain[len] += 1;
						result = 3;
					}
	        		field[index] = moves[moves.size() - 1][i].prevState;
				}
				moves.pop_back();
			}
			else
				return 0;
			return result;
		}
		const int SeaBattleField::CheckLastMove() const{ //"Подсмотреть", что делал последний ход; Возвращает то же, что и CancelLastMove()
			if (!moves.size())
				return 0;
			if (!moves[moves.size() - 1].size())
				return 1;
			if (moves[moves.size() - 1][0].newState == field_value_mean[4])
	        	return 5;
			else if (moves[moves.size() - 1][0].newState == field_value_mean[2])
				return 2;
			else if (moves[moves.size() - 1][0].newState == field_value_mean[3])
				return 3;
			else
				return 4;
		}
		
		
	//Чтение из файла
		int SeaBattleField::LoadFromFile(std::string file_name, bool is_it_second_field){	//Загрузка из файла; 0 - успешно; 1 - некорректное название/ошибка открытия файла; 2 - ошибка чтения
			if (file_name.length() < 3)
				return 1;
			if (!HaveFormatInName(file_name, ".txt"))
				file_name += ".txt";
			
			std::ifstream file(file_name.c_str());	//Открываем файл для чтения
			if (!file.is_open())
				return 1;
			ResetFieldValueMean();
			ResetShips();
			ResetShipsRemain();
			ResetField();
			if	(
				!ReadFindString(file, (is_it_second_field ? "SeaBattleField2" : "SeaBattleField1")) ||
				!ReadElement(file, "c", &cols) ||
				!ReadElement(file, "r", &rows) ||
				!ReadArray(file, "fvm", &field_value_mean) ||
				!ReadArray(file, "s", &ships) ||
				!ReadArray(file, "sr", &ships_remain) ||
				!ReadMoves(file) ||
				!RegenerateFieldByMoves()
				){	//Ошибка чтения
					file.clear();	//Сбросить состояние чтения
					file.close();
					return 2;
			}
			file.close();
			return 0;
		}
		bool SeaBattleField::RegenerateFieldByMoves(){	//Восстановить поле по ходам; true - успешно; false - ошибка
			ClearField();
			int temp = ChangeFieldSize(cols, rows);
			if (temp == 1 || temp == 2)
				return false;
			for(int i = 0; i < moves.size(); i++)
				for(int n = 0; n < moves[i].size(); n++)
					field[moves[i][n].coordinate_index] = moves[i][n].newState;
			return true;
		}
		bool SeaBattleField::ReadFindString(std::ifstream &flow_name, std::string your_string){	//Найти строку в файле; true - удалось; false - не удалось
			std::string str;
			while (getline(flow_name, str)){
				if (flow_name.fail() && !flow_name.eof())
	                return false;
	            if (str.find(your_string) != std::string::npos)
	                return true;
            }
        	return false;
		}
		bool SeaBattleField::ReadIsCharEqualsReaded(std::ifstream &flow_name, char your_char){	//Равен ли считанный символ переданному; true - равны; false - не равны
			return (flow_name.get() == your_char);
		}
		bool SeaBattleField::ReadArray(std::ifstream &flow_name, std::string element_name, unsigned char **array){	//Считать из файла элемент в кавычках; true - успешно; false - ошибка чтения
			int array_size = 0;
			if (!ReadIsStringEquals(flow_name, element_name + "=(") || !ReadNumber(flow_name, &array_size))
				return false;
			delete[] *array;
			*array = new unsigned char[array_size];
			for(int i = 0, temp ; i < array_size; i++){	//Чтение и запись всех элементов массива
				if (!ReadNumber(flow_name, &temp))
					return false;
				(*array)[i] = temp;
			}
			if (!ReadIsCharEqualsReaded(flow_name, ')') || !ReadIsCharEqualsReaded(flow_name, '\n'))
				return false;
			return true;
		}
		bool SeaBattleField::ReadIsStringEquals(std::ifstream &flow_name, std::string your_string){	//Совпадает ли ожидаемое имя элемента со считанным; true - совпадает; false - не совпадает
			for(int i = 0; i < your_string.length(); i++){
				if (your_string[i] != flow_name.get())
					return false;
			}
			return true;
		}
		bool SeaBattleField::ReadElement(std::ifstream &flow_name, std::string element_name, int *element){	//Считать из файла элемент в кавычках; true - успешно; false - ошибка чтения
			if (!ReadIsStringEquals(flow_name, element_name + "=\"") || !ReadNumber(flow_name, element) || !ReadIsCharEqualsReaded(flow_name, '"') || !ReadIsCharEqualsReaded(flow_name, '\n'))
				return false;
			return true;
		}
		bool SeaBattleField::ReadChar(std::ifstream &flow_name, unsigned char *element){	//Считать из файла символ; true - успешно; false - ошибка чтения
			*element = (flow_name.get() - '0');
			return !(flow_name.fail() || flow_name.eof());
		}
		bool SeaBattleField::ReadNumber(std::ifstream &flow_name, int *element){	//Считать из файла элемент в кавычках; true - успешно; false - ошибка чтения
			return (flow_name >> *element).good();
		}
		bool SeaBattleField::ReadMoves(std::ifstream &flow_name){	//Считать из файла ходы; true - успешно; false - ошибка чтения
			int moves_size, moves_i_size, index;
			if (!ReadIsStringEquals(flow_name, "m=\"") || !ReadNumber(flow_name, &moves_size) || !ReadIsCharEqualsReaded(flow_name, '"') || !ReadIsCharEqualsReaded(flow_name, '\n'))
				return false;
			unsigned char pr_st, ne_st;
			for(int i = 0; i < moves_size; i++){
				MakeMove();
				if (!ReadNumber(flow_name, &moves_i_size) || !ReadIsCharEqualsReaded(flow_name, ' '))
					return false;
				for(int n = 0; n < moves_i_size; n++){
					if (!ReadNumber(flow_name, &index) || !ReadIsCharEqualsReaded(flow_name, ' ') || !ReadChar(flow_name, &pr_st) || !ReadChar(flow_name, &ne_st))
							return false;
					if (pr_st > 4 || ne_st > 4)
							return false;
					pr_st = field_value_mean[pr_st];
					ne_st = field_value_mean[ne_st];
					AddCellValueChange(index, pr_st, ne_st);
				}
			}
			return true;
		}
		
	//Запись в файл
		bool SeaBattleField::HaveFormatInName(std::string file_name, std::string file_format){
			if (file_name.length() < 4)
				return false;
			return file_name.compare(file_name.length() - 4, 4, file_format) == 0;
		}
		int SeaBattleField::SaveToFile(std::string file_name, bool append){ //Сохранение в файл; 0 - успешно; 1 - некорректное название/ошибка открытия файла; 2 - ошибка записи
			if (file_name.length() < 3)
				return 1;
			if (!HaveFormatInName(file_name, ".txt"))
				file_name += ".txt";
			std::ofstream file(file_name.c_str(), (append ? std::ios::app : std::ios::trunc));	//Открываем файл для записи (append = true: добавим данные в конец файла, иначе очистим файл и запишем с начала)
			if (!file.is_open())
				return 1;
			if	(
				!RecordString(file, (append ? "SeaBattleField2\n" : "SeaBattleField1\n")) ||
				!RecordElement(file, "c", cols) ||
				!RecordElement(file, "r", rows) ||
				!RecordArray(file, "fvm", field_value_mean, 5) ||
				!RecordArray(file, "s", ships, ships[0] + 1) ||
				!RecordArray(file, "sr", ships_remain, ships_remain[0] + 1) ||
				!RecordMoves(file)
				){	//Ошибка записи
					file.clear();	//Сбросить состояние записи
					return 2;
			}
			return 0;
		}
		bool SeaBattleField::RecordString(std::ofstream &flow_name, std::string your_string){	//Записать в файл строку; true - успешно; false - ошибка записи
			return (flow_name << your_string).good();
		}
		bool SeaBattleField::RecordNumber(std::ofstream &flow_name, int number){	//Записать в файл число; true - успешно; false - ошибка записи
			return (flow_name << number).good();
		}
		bool SeaBattleField::RecordElement(std::ofstream &flow_name, std::string element_name, int number){	//Записать в файл элемент в кавычки; true - успешно; false - ошибка записи
			return (flow_name << element_name << "=\"" << number << "\"\n").good();
		}
		bool SeaBattleField::RecordArray(std::ofstream &flow_name, std::string element_name, unsigned char *array, int array_size){	//Записать в файл массив в скобках; true - успешно; false - ошибка записи
			flow_name << element_name << "=(" << array_size;
			for(int i = 0; i < array_size; i++)
				flow_name << " " << ((int)array[i]);
			flow_name << ")\n";
			return flow_name.good();
		}
		bool SeaBattleField::RecordMoves(std::ofstream &flow_name){	//Записать в файл ходы; true - успешно; false - ошибка записи
			if (!RecordElement(flow_name, "m", moves.size()))
				return false;
			int c1 = GetValueOfValueMean(0), c2 = GetValueOfValueMean(1), c3 = GetValueOfValueMean(2), c4 = GetValueOfValueMean(3), c5 = GetValueOfValueMean(4);
			ChangeFieldValueMean(0, 1, 2, 3, 4);
			for(int i = 0; i < moves.size(); i++){
				if (!RecordNumber(flow_name, moves[i].size()))
					return false;
				if (moves[i].size())
				flow_name << " ";
				for(int n = 0; n < moves[i].size(); n++){
					RecordNumber(flow_name, moves[i][n].coordinate_index);
					flow_name << ' ';
					RecordNumber(flow_name, moves[i][n].prevState);
					RecordNumber(flow_name, moves[i][n].newState);
					if (n < moves[i].size() - 1)
					flow_name << " ";
				}
				flow_name << ' ';
				if (!flow_name.good())
					return false;
			}
			flow_name << '\n';
			ChangeFieldValueMean(c1, c2, c3, c4, c5);
			return flow_name.good();
		}
		
		
	//Работа с кораблями
		void SeaBattleField::SetInSide(unsigned char set_what, int side, int len, int x, int y){	//ставит set_what, len клеток в направлении side
			int x_2 = XYChangesBySide(0, side), y_2 = XYChangesBySide(1, side), index;
			for(side = 0, x = x - x_2, y = y - y_2; side < len; x = x + x_2, y = y + y_2, side++)
			{
				index = cols * (y + y_2) + (x + x_2);
				AddCellValueChange(index, field[index], set_what);
				field[index] = set_what;
			}	
		}
		int SeaBattleField::CheckOneBlock(int x, int y) const{	//0 - всё в порядке / координата за пределами поля; >0 - кол-во кораблей
			int x_2, y_2, ships = 0;
			for(x_2 = -1; x_2 < 2; x_2++)
				if(x_2 + x >= 0 && x_2 + x < cols)
					for(y_2 = -1; y_2 < 2; y_2++)
						if(y_2 + y >= 0 && y_2 + y < rows)
							if(field[cols * (y_2 + y) + (x_2 + x)] == field_value_mean[4] || field[cols * (y_2 + y) + (x_2 + x)] == field_value_mean[3]
									|| field[cols * (y_2 + y) + (x_2 + x)] == field_value_mean[2])
								ships++;
			return ships;
		}
		int SeaBattleField::CheckSide(int side, int len, int x, int y) const{	//0 - всё в порядке; 1 - за пределами поля / корабли
		    int x_2 = XYChangesBySide(0, side), y_2 = XYChangesBySide(1, side);
			for(side = 0, len--, x += x_2, y += y_2; side < len; x += x_2, y += y_2, side++)
				if ((x < 0 || x >= cols) || (y < 0 || y >= rows) || CheckOneBlock(x, y))
					return 1;
			return 0;
		}
		int SeaBattleField::FindShipEdge(int *x, int *y){	//Возвращает направление корабля и в *x и *y запишет координаты крайней клетки корабля
			int index = cols * *y + *x;
			if ((*x + 1 < cols && (field[index + 1] == field_value_mean[4] || field[index + 1] == field_value_mean[3] || field[index + 1] == field_value_mean[2])) ||
        		  (*x - 1 >= 0 && (field[index - 1] == field_value_mean[4] || field[index - 1] == field_value_mean[3] || field[index - 1] == field_value_mean[2])))
			{
				*x += (CalculateWhatInSide(*x, *y, 2, -1) - 1) * XYChangesBySide(0, 2);	//горизонтальное расположение корабля
				return 4;
			}
			*y += (CalculateWhatInSide(*x, *y, 1, -1) - 1) * XYChangesBySide(1, 1);
			return 3;
		}
		int SeaBattleField::CalculateWhatInSide(int x, int y, int side, int what){	//считает количество what в направлении side до EMPTY или SHOT клетки
			int	x2 = XYChangesBySide(0, side), y2 = XYChangesBySide(1, side), pos;
			for(side = 0, pos = (cols * y) + x; !(field[pos] == field_value_mean[0] || field[pos] == field_value_mean[1]) && (pos >= 0 && pos < cols * rows); pos += (y2 * cols) + x2)
				if (what == -1)
				{
					if (field[pos] == field_value_mean[4] || field[pos] == field_value_mean[3] || field[pos] == field_value_mean[2])
						side++;
				}
				else
					if (field[pos] == what)
						side++;
			return side;
		}
		int SeaBattleField::SetShotAroundShips(int side, int len, int x, int y){	//возвращает итоговое кол-во уст. SHOT
			int i = 0;
			i += ShotToWallOfSide(side, x, y, 1);
			i += ShotToMiddleOfSide(side, x, y, len);
			x += (len - 1) * XYChangesBySide(0, side);
			y += (len - 1) * XYChangesBySide(1, side);
			i += ShotToWallOfSide(side, x, y, 0);
			return i;
		}
		int SeaBattleField::ShotToMiddleOfSide(int side, int x, int y, int len){	//возвращает количество установленных SHOT
			int i, index, n = len, x2 = XYChangesBySide(0, side), y2 = XYChangesBySide(1, side);
			for(len = 0, x -= x2, y -= y2; n > 0; n--, x += x2, y += y2)
			{
				if (x2)
				{
					for(i = -1; i < 2; i += 2)
						if (y + i >= 0 && y + i < rows)
							if (field[cols * (y + i) + (x + x2)] == field_value_mean[0])
							{
								index = (cols * (y + i) + (x + x2));
								AddCellValueChange(index, field[index], field_value_mean[1]);
								field[index] = field_value_mean[1];
								len++;
							}
				}
				else
					for(i = -1; i < 2; i += 2)
						if (x + i >= 0 && x + i < cols)
							if (field[cols * (y + y2) + (x + i)] == field_value_mean[0])
							{
								index = (cols * (y + y2) + (x + i));
								AddCellValueChange(index, field[index], field_value_mean[1]);
								field[index] = field_value_mean[1];
								len++;
							}
			}
			return len;
		}
		int SeaBattleField::ShotToWallOfSide(int side, int x, int y, int back){	//возвращает количество установленных SHOT
			if (back)	//чтобы отзеркалить от side сторону для установки SHOT
				if (side > 2)
					side -= 2;
				else
					side += 2;
			int count = 0, i, index, n, x2 = XYChangesBySide(0, side), y2 = XYChangesBySide(1, side);
			if (x2)
			{
				if (x + x2 >= 0 && x + x2 < cols)
					for(i = -1; i < 2; i++)
						if (y + i >= 0 && y + i < rows)
							if (field[cols * (y + i) + (x + x2)] == field_value_mean[0])
							{
								index = cols * (y + i) + (x + x2);
								AddCellValueChange(index, field[index], field_value_mean[1]);
								field[index] = field_value_mean[1];
								count++;
							}
			}
			else
				if (y + y2 >= 0 && y + y2 < rows)
					for(i = -1; i < 2; i++)
						if (x + i >= 0 && x + i < cols)
							if (field[cols * (y + y2) + (x + i)] == field_value_mean[0])
							{
								index = cols * (y + y2) + (x + i);
								AddCellValueChange(index, field[index], field_value_mean[1]);
								field[index] = field_value_mean[1];
								count++;
							}
			return count;
		}
		int SeaBattleField::ShotTo(int x, int y){	//Выстрелить; 0 - попадание по SHIP; 1 - подрыв последнего корабля; 2 - выстрел в пустую клетку; 3 - бессмысленный выстрел, попадание в SHOT, STRIKE, KILL; 4 - ошибка координаты; 5 - ошибка значения ячейки
			if ((x < 0 || x >= cols) || (y < 0 || y >= rows))
				return 4;
			int index = (cols * y + x), side = -1, len = -1, x_ = x, y_ = y;
			unsigned char cell_value = field[index];
			if (field_value_mean[0] == cell_value)	//EMPTY
			{
				MakeMove();
				field[index] = field_value_mean[1];
				AddCellValueChange(index, field_value_mean[0], field_value_mean[1]);
				return 2;
			}
			else if (field_value_mean[4] == cell_value)	//SHIP
			{
				MakeMove();
				field[index] = field_value_mean[2];
				side = FindShipEdge(&x_, &y_);	//После функции x_ и y_ становятся координатой крайней (правой / верхней) клетки корабля
				len = CalculateWhatInSide(x_, y_, side, -1);
				if (CalculateWhatInSide(x_, y_, side, field_value_mean[2]) == len)	//True - подрыв, False - попадание
				{
					SetInSide(field_value_mean[3], side, len, x_, y_);
					TransformByIndexValueChange(index, field_value_mean[4], field_value_mean[3]);
					SetShotAroundShips(side, len, x_, y_);
					ships_remain[len] -= 1;
					if (!GetCountOfShipsRemain())
						return 1;
				}
				else
					AddCellValueChange(index, field_value_mean[4], field_value_mean[2]);
				return 0;
			}
			else if ((cell_value == field_value_mean[1]) ||
					 (cell_value == field_value_mean[2]) ||
					 (cell_value == field_value_mean[3]))	//SHOT STRIKE KILL
				{
					MakeMove();
					return 3;
				}
			else
				return 5;
		}
		int SeaBattleField::SetShip(int ship_len, int side, int x, int y){ //Установить корабль; 0 - успешно; 1 - ошибка аргумента; 2 - ошибка установки первой клетки; 3 - ошибка установки в направлении
			if ((x < 0 || x >= cols) || (y < 0 || y >= rows) || (side < 1 || side > 4))
				return 1;
			if (CheckOneBlock(x, y))
				return 2;
			if (ship_len == 1)
			{
				MakeMove();
				AddCellValueChange(cols * y + x, field_value_mean[0], field_value_mean[4]);
				field[cols * y + x] = field_value_mean[4];
				return 0;
			}
			if (!((AvailableSides(x, y, ship_len) / IntPow(10, 4 - side)) % 10))	//проверяется доступность установки в сторону side
				return 3;
			MakeMove();
			SetInSide(field_value_mean[4], side, ship_len, x, y);
			return 0;
		}
		const int SeaBattleField::AvailableSides(int x, int y, int ship_len) const{ //возвращает доступные стороны закодированные числом (верх - 1б право - 10, низ - 100, лево - 1000)
			int side, sum = 0;
			if (((x < 0 || x >= cols) || (y < 0 || y >= rows)) || CheckOneBlock(x, y))
				return 0;
			for (side = 1; side < 5; side++)
				if (!CheckSide(side, ship_len, x, y))
					sum = sum + IntPow(10, 4 - side);
			return sum;
		}
		const int SeaBattleField::IsSideAvailable(int x, int y, int ship_len, int side) const{	//Проверяет доступность стороны side для установки корабля. Возвращает: -1 - некорректная координата; 0 - доступна; 1 - недоступна (side =: 1 - верх; 2 - право; 3 - низ; 4 - лево);
			if ((x < 0 || x >= cols) || (y < 0 || y >= rows))
				return -1;
			if (CheckOneBlock(x, y) || CheckSide(side, ship_len, x, y))
				return 1;
			return 0;
		}
		
	//Необходимые для работы методы
		const int SeaBattleField::IntPow(int x, int y) const{	//Возвести x в степень y
			int a = 1;
			for (y; y > 0; y--)
				a *= x;
			return a;
		}
		const int SeaBattleField::IsInCharRange(int number) const{	//0 - в диапазоне; 1 - за диапазоном
			return (number < 0 || number > 255) ? 1 : 0;
		}
		const int SeaBattleField::IsNewSizeAvailable(int cols_, int rows_) const{	//0 - доступно; 1 - недоступно
			if ((cols_ < 1 || cols_ > 100000000) || (rows_ < 1 || rows_ > 100000000) || (cols_ > 1000000000 / rows_))
				return 1;
			return 0;
		}
		const int SeaBattleField::MemoryAllocationError(int error_type) const{	//Значения error_type: 0 - критически важный объект; 1 - не критически важный объект
			switch(error_type){
				case 0:
					exit(1);
				case 1:
					return 1;
					break;
			}
		}
		const int SeaBattleField::XYChangesBySide(int x_is_0_y_is_1, int side) const{	//значения изменения x, y в зависимости от стороны
		    if (x_is_0_y_is_1)	//в зависимости от направления x = 0, 1, 0, -1;    y = 1, 0, -1, 0
		    	return (side < 3) ? 0 - side%2 : (side == 3) ? 1 : 0;
			return (side%2) ? 0 : (side == 2) ? 1 : -1;
		}
		const int SeaBattleField::IsOneOfFieldValueMean(unsigned char number) const{	//0 - является каким-либо значением из field_value_mean; 1 - не является
			if (number == field_value_mean[0] || number == field_value_mean[1] || number == field_value_mean[2] || number == field_value_mean[3] || number == field_value_mean[4])
				return 0;
			return 1;
		}
