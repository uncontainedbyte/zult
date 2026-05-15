#pragma once
#include <string>

enum class TokenType{
	Ident,
	Symbol,
	String,
	Number,
	Eof,
	
	Keyword,
};

struct Token{
	TokenType type;
	std::string value;
	uint line;
	uint column;
};
