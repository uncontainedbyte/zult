#pragma once

#include "token.hpp"
#include <vector>
#include <variant>
#include <iostream>
#include <unordered_map>
#include <cstdint>



enum class TokenID{
	INVALID,
	
	NUMBER_INT,
	NUMBER_FLOAT,
	STRING,
	IDENT,
	
	IF,
	ELSE,
	MATCH,
	WHILE,
	LOOP,
	FOR,
	FUNC,
	RET,
	OP,
	STRUCT,
	CONSTRUCTOR,
	DESTROY,
	ALLOC,
	FREE,
	GOTO,
	CONTINUE,
	BREAK,
	EXTERN,
	
	VOID, BOOL,
	U8, U16, U32, U64,
	I8, I16, I32, I64,
	F32, F64,
	
	BRACKET_OPEN,
	BRACKET_CLOSE,
	CURLY_OPEN,
	CURLY_CLOSE,
	SQUARE_BRACKET_OPEN,
	SQUARE_BRACKET_CLOSE,
	
	ASSIGN,
	PTR_ARROW,
	PLUS,
	MINUS,
	ASSIGN_PLUS,
	ASSIGN_MINUS,
	ASSIGN_MULTIPLY,
	ASSIGN_DIVIDE,
	ASSIGN_MOD,
	ASSIGN_XOR,
	ASSIGN_AND,
	ASSIGN_OR,
	EQUAL,
	NOT_EQUAL,
	GREATER_EQUAL,
	LESS_EQUAL,
	GREATER,
	LESS,
	BOOL_OR,
	BOOL_AND,
	
	COMMA,
	TILDA,
	COLON,
	SEMI,
	DOT,
	UNDERSCORE,
	ASTERISK,
	SLASH,
	BACKSLASH,
	CARET,
	EXCLAMATION,
	PERCENT,
	AMPERSAND,
	PIPE,
	QUOTE,
};

std::string TokenIDToString(TokenID id);

struct RefinedToken{
	TokenType type;
	TokenID id;
	union{
		uint64_t i;
		double f;
		const std::string* s;
	} data;
	uint ln;
	uint cn;
};

std::vector<RefinedToken> RefineTokens(const std::vector<Token>& tokens,const std::vector<std::string>& lines);




































