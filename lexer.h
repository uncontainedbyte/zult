#pragma once

#include "symbol-table.hpp"
#include <iostream>


class Lexer{
	public:
		void lex();
	private:
		std::string line_buffer;
		std::string buffer = "";
		int index = 0;
		int column = 1;
		int row = 1;
		
		char specialChar(char);
		void convertSpecialNums();
		void refineIdent(int);
		void refineSymbols(int);
		
		struct PeekRet{
			bool h;
			char v;
		};
		
		PeekRet peek(int offset = 0) const;
		char inc();
};
inline Lexer lexer;
