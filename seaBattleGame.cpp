#ifndef SEABATTLEGAME_BM_CPP
#define SEABATTLEGAME_BM_CPP


#include "seaBattleField.h"	//Библиотека "Поле игры 'Морской бой'"
#include <iomanip> //Для форматирования текста
#include <iostream>

class SeaBattleGame : public SeaBattleField{ //Интерфейс и реализация игры
	private:
		char *FieldSymbol = 0;	//символы отображения клеток во время игры
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
		SeaBattleGame(const SeaBattleGame& other) : SeaBattleField(other) {
			FieldSymbol = 0;
			FieldSymbol = new char[5];
			if (!FieldSymbol)
				exit(1);
			*this = other;
		}
		SeaBattleGame &operator=(const SeaBattleGame& other){
			SeaBattleField::operator=(other);
			FieldSymbol[0] = other.FieldSymbol[0];	//Пустая клетка
			FieldSymbol[1] = other.FieldSymbol[1];	//Стреленая клетка
			FieldSymbol[2] = other.FieldSymbol[2];	//Раненая клетка корабля
			FieldSymbol[3] = other.FieldSymbol[3];	//Клетка взорванного корабля
			FieldSymbol[4] = other.FieldSymbol[4];	//Целая клетка корабля
			return *this;
		}
		
		void PrintColored(const std::string& text, int color) 
		{
		    std::cout << "\033[1;3" << color << "m" << text << "\033[0m";
		}
		
		void DrawSymbol(int index){	//Отрисовка элемента index поля, в виде игрового символа
			unsigned char value = GetValueOfCellByIndex(index);
			std::string myString(1, FieldSymbol[0]);
			if (value == GetValueOfValueMean(0))
				PrintColored(myString, 5);
				//cout << FieldSymbol[0];
			else if (value == GetValueOfValueMean(1))
				std::cout << FieldSymbol[1];
			else if (value == GetValueOfValueMean(2))
				std::cout << FieldSymbol[2];
			else if (value == GetValueOfValueMean(3))
				std::cout << FieldSymbol[3];
			else if (value == GetValueOfValueMean(4))
				std::cout << FieldSymbol[4];
		}
		void DrawFields(SeaBattleGame &enemy_field) 
		{
	        int y;
	        
	        PrintWordNTimes(" ", GetCols() - 1);
	        std::cout << "Ваше поле ";
	        PrintWordNTimes(" ", 2 * GetCols() + 3);
	        std::cout << "Поле врага" << std::endl;
	        
	        draw_top_letters();
	        std::cout << "          ";
	        enemy_field.draw_top_letters();
	        
	        for(std::cout << std::endl, y = 0; y < GetRows(); y++, std::cout << " |\n") {
	            draw_row(y, 1);
	            std::cout << " |        ";
	            enemy_field.draw_row(y, 0);
	        }
	        std::cout << std::endl;
	    }
		
	    void draw_row(int y, int owner) //строки с учётом владения полем
		{ 
	        int x, value;
	        for(std::cout << std::setfill(' ') << std::setw(2) << y << "|", x = 0; x < GetCols(); x++) {
	            value = GetValueOfCell(x, y);
	            if (value == GetValueOfValueMean(0)) 
				{
	                std::cout << " " << FieldSymbol[0];
	            } 
				else if (value == GetValueOfValueMean(4)) 
				{
	                if (owner)
	                    std::cout << " " << FieldSymbol[4];
	                else
	                    std::cout << " " << FieldSymbol[0];
	            } 
				else 
				{
	                for (int i = 0; i < 5; i++) {
	                    if (value == GetValueOfValueMean(i)) {
	                        std::cout << " " << FieldSymbol[i];
	                        break;
	                    }
	                }
	            }
	        }
	    }
		
		void DrawField(){	//Отрисовать 1 поле игровыми символами
			draw_top_letters();
			std::cout << std::endl;
			for(int y = 0; y < GetRows(); y++, std::cout << " |\n")
	            draw_row(y, 1);
			std::cout << std::endl;
		}
		
		void draw_top_letters()
		{						//буквенная полоса горизонтальных координат
			short i;
			for(std::cout << "   ", i = 0; i < this->GetCols(); i++)
				//printf(" %c", i + 'A');
				std::cout << " " << (char)(i + 'A');
		}
		
		void PrintWordNTimes(std::string word, int n){ 				//вывести строку n раз
			for(n; n > 0; n--)
				std::cout << word;
		}
		
		void PrintFieldValues(){	//Отрисовать 1 поле хранимыми значениями
			for(int index = 0; index < GetCols() * GetRows(); index++){
				std::cout << GetValueOfCellByIndex(index);
				if (((index + 1) % GetCols() == 0) && (index > 0))
					std::cout << std::endl;
				else
					std::cout << " ";
			}
			std::cout << std::endl;
		}
		void PrintFieldIndexs(){	//Отрисовать индексы 1 поля
			for(int index = 0; index < GetCols() * GetRows(); index++){
				std::cout << index;
				if (((index + 1) % GetCols() == 0) && (index > 0))
					std::cout << std::endl;
				else
					std::cout << " ";
			}
		}
		void PrintFieldValueMean(){	//Отобразить значения, которыми обозначаются EMPTY, SHOT, ..., SHIP
			std::cout << "Обозначения:\nEMPTY - " << GetValueOfValueMean(0) << "\nSHOT - " << GetValueOfValueMean(1)
			<< "\nSTRIKE - " << GetValueOfValueMean(2) << "\nKILL - " << GetValueOfValueMean(3)
			<< "\nSHIP - " << GetValueOfValueMean(4) << "\n";
		}
		
		void SetShipsRandomly() 
		{
	        Reset();
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


#endif

