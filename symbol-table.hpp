#pragma once



#include <vector>
#include <unordered_map>
#include <string>




inline std::string raw_file;
inline std::vector<std::string> file_lines;




enum class TokenID{
	IF,
	Number,
	String,
	Ident,
	eof,
	Unknown,
};
inline std::string TokenToString(TokenID id){
	switch(id){
		case TokenID::IF: return "IF";
		case TokenID::Number: return "Number";
		case TokenID::String: return "String";
		case TokenID::Ident: return "Ident";
		case TokenID::eof: return "eof";
		case TokenID::Unknown: return "Unknown";
	};
	return "Unhandled TokenToString Case";
}
inline const std::unordered_map<std::string,TokenID> Lexer_Table = {
	{"if",TokenID::IF},
};

inline bool isSymbol(char c){
	switch(c){
		case '+':case '=':case '-':case '{':
		case '\\':case '\'':case '\"':case ';':
		case ':':case '?':case '/':case '.':
		case ')':case '<':case ',':case '>':
		case '(':case '*':case '&':case '^':
		case '@':case '#':case '$':case '%':
		case '!':case '~':case '|':case '[':
		case ']':case '}': return true;
	};
	return false;
}

struct Token{
	TokenID id;
	std::string value;
	int row;
	int column;
	int len;
};

inline std::vector<Token> tokens;






































