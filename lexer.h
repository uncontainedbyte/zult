#pragma once

#include <vector>
#include <string>
#include <optional>
#include <unordered_map>
#include <sstream>
#include <stdarg.h>
#include <algorithm>
#include <iostream>
#include "token.hpp"

std::string TokenTypeToString(TokenType type);

class Lexer{
	public:
		Lexer();
		
		std::vector<Token> lex(std::string& str);
		
		std::vector<std::string> lines;
	private:
		
		std::string m_str;
		size_t m_index = 0;
		
		struct PeekRet{
			bool h;
			char v;
		};
		
		PeekRet peek(int offset = 0) const;
		char inc();
		Token lexIdent(uint&,uint&);
		Token lexNumber(uint&,uint&);
		bool lexComment(uint&,uint&);
		bool lexString(uint&,uint&,std::vector<Token>& tokens);
		void lexSymbol(uint&,uint&,std::vector<Token>& tokens);
};

inline Lexer lexer;
