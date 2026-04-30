#ifndef ANSISEQUENCES_BM_CPP
#define ANSISEQUENCES_BM_CPP

#include <iostream>
#include <cstring>

class AnsiSequences{
	private:	//Функции Escape-последовательностей
		static std::string EscapeCodeTemplate		(bool out_to_cout_once, std::string code_text){
			if (out_to_cout_once){
				std::cout << code_text;
				return "";
			}
			return code_text;
		}
		AnsiSequences() = delete;
	public:	
	//Работа с курсором
		static std::string StrClear				(bool out_to_cout_once = true, bool set_cursor_at_string_start = true){	//Очистка строки, на которой находится курсор
			return EscapeCodeTemplate(out_to_cout_once, (set_cursor_at_string_start ? "\033[2K\r" : "\033[2K"));	//\033[2K - перезаписать все символы в строке пробелами; \r - перевести курсор в начало строки
		}
		static std::string CursorON				(bool out_to_cout_once = true){	//Включение мигания курсора
			return EscapeCodeTemplate(out_to_cout_once, "\033[?25h");	//\033[?25h - включить мигание курсора
		}
		static std::string CursorOFF			(bool out_to_cout_once = true){	//Выключение мигания курсора	
			return EscapeCodeTemplate(out_to_cout_once, "\033[?25l");	//\033[?25l - выключить мигание курсора
		}
		static std::string RemNSymbols			(bool out_to_cout_once = true, int symbols_count_for_removing = 1){	//Удалить symbols_count_for_removing символов слева от курсора в текущей строке
			std::string code_text = CursorMoveLeft(false, symbols_count_for_removing), spaces_for_add = "";
			for(int i = 0; i < symbols_count_for_removing; i++)
				spaces_for_add += ' ';
			return EscapeCodeTemplate(out_to_cout_once, (code_text + spaces_for_add + code_text));
		}
		static std::string CursorSetPosition	(bool out_to_cout_once = true, unsigned int column_index = 0, unsigned int row_index = 0){
			return EscapeCodeTemplate(out_to_cout_once, "\033[" + std::to_string(row_index + 1) + ";" + std::to_string(column_index + 1) + "H");
		}
		static std::string CursorMoveTop		(bool out_to_cout_once = true, unsigned int symbols_count_for_move = 1){
			return EscapeCodeTemplate(out_to_cout_once, "\033[" + std::to_string(symbols_count_for_move) + "A");
		}
		static std::string CursorMoveRight		(bool out_to_cout_once = true, unsigned int symbols_count_for_move = 1){
			return EscapeCodeTemplate(out_to_cout_once, "\033[" + std::to_string(symbols_count_for_move) + "C");
		}
		static std::string CursorMoveBottom		(bool out_to_cout_once = true, unsigned int symbols_count_for_move = 1){
			return EscapeCodeTemplate(out_to_cout_once, "\033[" + std::to_string(symbols_count_for_move) + "B");
		}
		static std::string CursorMoveLeft		(bool out_to_cout_once = true, unsigned int symbols_count_for_move = 1){
			return EscapeCodeTemplate(out_to_cout_once, "\033[" + std::to_string(symbols_count_for_move) + "D");
		}
	//Форматирование и графика
		static std::string DrawLine				(bool out_to_cout_once = true, bool top = false, bool right = false, bool bottom = false, bool left = false){
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
			/*char symbol;
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
				return "";*/
			char symbol;
			int index = (10000 + (top * 1000) + (right * 100) + (bottom * 10) + left);
			switch(index){
				case 11111: symbol = 'n'; break;
				case 11110: symbol = 't'; break;
				case 11101: symbol = 'v'; break;
				case 11100: symbol = 'm'; break;
				case 11011: symbol = 'u'; break;
				case 11010: symbol = 'x'; break;
				case 11001: symbol = 'j'; break;
			//	case 11000: symbol = ' '; break;	//Только верх
				case 10111: symbol = 'w'; break;
				case 10110: symbol = 'l'; break;
				case 10101: symbol = 'q'; break;
			//	case 10100: symbol = ' '; break;	//Только право
				case 10011: symbol = 'k'; break;
			//	case 10010: symbol = ' '; break;	//Только низ
			//	case 10001: symbol = ' '; break;	//Только лево
			//	case 10000: symbol = ' '; break;	//Ни одна сторона
				default:			  return "";
			}	// \033(0 - включить режим особых символов (псевдографики); \033(B - выключить режим особых символов (псевдографики)
			return EscapeCodeTemplate(out_to_cout_once, std::string("\033(0") + symbol + std::string("\033(B"));
		}
		static std::string BlinkingTextON		(bool out_to_cout_once = true){	//Включить мигание для всего последующего выводимого текста
			return EscapeCodeTemplate(out_to_cout_once, "\033[5m");	// \033[5m - сделать следующий выводимый текст мигающим
		}
		static std::string BlinkingTextOFF		(bool out_to_cout_once = true){	//Выключить мигание для всего последующего выводимого текста
			return EscapeCodeTemplate(out_to_cout_once, "\033[0m");	// \033[0m - сбросить атрибуты
		}
		static std::string BlinkingText			(bool out_to_cout_once = true, std::string text = "", int text_color = 0, int text_background_color = 0){	//Вывести текст мигающим, с возможностью выбора фонового и основного цвета
			//Информация про атрибуты текста - https://blog.sedicomm.com/2026/03/24/kak-izmenyat-stili-vyvoda-komandy-echo-v-linux/
			// \033[5m - Сделать следующий выводимый текст мигающим. \033[0m - сбросить атрибуты
			return EscapeCodeTemplate(out_to_cout_once, "\033[5" +	(text_color == 0 ? "" : (";" + std::to_string(text_color))) +
									(text_background_color == 0 ? "" : (";" + std::to_string(text_background_color))) + "m" + text + "\033[0m");
		}
		static std::string AltWindowOpen		(bool out_to_cout_once = true, bool continue_from_current_cursor_position = false){	//Открыть альтернативное окно терминала
			return EscapeCodeTemplate(out_to_cout_once, std::string("\e[?1049h") + std::string(continue_from_current_cursor_position ? "" : CursorSetPosition()));	// \e[?1049h - открыть альтернативное окно терминала
		}
		static std::string AltWindowBackToMain	(bool out_to_cout_once = true, bool continue_from_current_cursor_position = false){	//Закрыть альтернативное окно терминала (вернуться в основное)
			return EscapeCodeTemplate(out_to_cout_once, std::string("\e[?1049l") + std::string(continue_from_current_cursor_position ? "" : CursorSetPosition()));;	// \e[?1049l - закрыть альтернативное окно терминала (вернуться в основное)
		}
};


#endif
