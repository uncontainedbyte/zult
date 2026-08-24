#pragma once



#include <vector>
#include <unordered_map>
#include <string>




inline std::string raw_file;
inline std::vector<std::string> file_lines;




enum class TokenID{
	Number,String,Ident,eof,Unknown,
	IF, NAMESPACE, OPERATOR, PRIVATE, RETURN, SWITCH,
	DOUBLE, STRUCT, WHILE, USING, DEFER, FLOAT, CONST,
	ELSE, SELF, ENUM, UINT, CHAR, VOID, BOOL, FUNC,
	INT, U16, U32, U64, I16, I32, I64, F32, F64,
	FOR, I8, U8,
	
	LSHIFTASSIGN, RSHIFTASSIGN, LSHIFT, RSHIFT,
	ADDASSIGN, SUBASSIGN, MULASSIGN, DIVASSIGN, MODASSIGN, ANDASSIGN, ORASSIGN, XORASSIGN, 
	LESSEQUAL, GREATEREQUAL, EQUAL, NOTEQUAL, OR, AND, SPECIALASSIGN, ACCESSOR, UNDERSCORE,
	ASSIGN, BILL, COMMA, DOT, STAR, EXCLAMATION, TILDE, PLUS, MINUS, PERCENT, SLASH,
	OPENSQUAREBRACKET, CLOSEDSQUAREBRACKET, OPENBRACKET, CLOSEDBRACKET,
	CURLYOPENBRACKET, CURLYCLOSEDBRACKET, COLON,
	BITAND, BITXOR, BITOR, LESS, GREATER, SEMI,
};
enum class SyntaxID{
	Start,
	Expr, Number, Ident, UnaryOp, Dereference, BoolNot, BitwiseNot, Positive, Negitive, Literal, String,
	GreaterEqual,LessEqual,Accessor,NotEqual,Multiply,Add,Subtract,Modulos,Divide,LeftShift,RightShift,
	Equal,BoolOr,BoolAnd,PointerMath,BitNot,BitAnd,BitOr,BitXor,Less,Greater,MemberAccess
};
#define CASE_TOKEN(NAME) case TokenID::NAME: return #NAME;
#define CASE_SYNTAX(NAME) case SyntaxID::NAME: return #NAME;
inline std::string TokenToString(TokenID id){
	switch(id){
		CASE_TOKEN( Unknown )
		CASE_TOKEN( Number )
		CASE_TOKEN( String )
		CASE_TOKEN( Ident )
		CASE_TOKEN( eof )
		CASE_TOKEN( CLOSEDSQUAREBRACKET )
		CASE_TOKEN( CURLYCLOSEDBRACKET )
		CASE_TOKEN( OPENSQUAREBRACKET )
		CASE_TOKEN( CURLYOPENBRACKET )
		CASE_TOKEN( CLOSEDBRACKET )
		CASE_TOKEN( SPECIALASSIGN )
		CASE_TOKEN( LSHIFTASSIGN )
		CASE_TOKEN( RSHIFTASSIGN )
		CASE_TOKEN( GREATEREQUAL )
		CASE_TOKEN( OPENBRACKET )
		CASE_TOKEN( EXCLAMATION )
		CASE_TOKEN( UNDERSCORE )
		CASE_TOKEN( ADDASSIGN )
		CASE_TOKEN( SUBASSIGN )
		CASE_TOKEN( MULASSIGN )
		CASE_TOKEN( DIVASSIGN )
		CASE_TOKEN( MODASSIGN )
		CASE_TOKEN( ANDASSIGN )
		CASE_TOKEN( XORASSIGN )
		CASE_TOKEN( LESSEQUAL )
		CASE_TOKEN( NAMESPACE )
		CASE_TOKEN( OPERATOR )
		CASE_TOKEN( ACCESSOR )
		CASE_TOKEN( ORASSIGN )
		CASE_TOKEN( NOTEQUAL )
		CASE_TOKEN( PRIVATE )
		CASE_TOKEN( GREATER )
		CASE_TOKEN( PERCENT )
		CASE_TOKEN( RETURN )
		CASE_TOKEN( SWITCH )
		CASE_TOKEN( DOUBLE )
		CASE_TOKEN( STRUCT )
		CASE_TOKEN( LSHIFT )
		CASE_TOKEN( RSHIFT )
		CASE_TOKEN( SLASH )
		CASE_TOKEN( ASSIGN )
		CASE_TOKEN( BITAND )
		CASE_TOKEN( BITXOR )
		CASE_TOKEN( WHILE )
		CASE_TOKEN( USING )
		CASE_TOKEN( DEFER )
		CASE_TOKEN( FLOAT )
		CASE_TOKEN( CONST )
		CASE_TOKEN( EQUAL )
		CASE_TOKEN( COMMA )
		CASE_TOKEN( TILDE )
		CASE_TOKEN( MINUS )
		CASE_TOKEN( BITOR )
		CASE_TOKEN( COLON )
		CASE_TOKEN( ELSE )
		CASE_TOKEN( SELF )
		CASE_TOKEN( ENUM )
		CASE_TOKEN( UINT )
		CASE_TOKEN( CHAR )
		CASE_TOKEN( VOID )
		CASE_TOKEN( BOOL )
		CASE_TOKEN( FUNC )
		CASE_TOKEN( BILL )
		CASE_TOKEN( STAR )
		CASE_TOKEN( PLUS )
		CASE_TOKEN( LESS )
		CASE_TOKEN( SEMI )
		CASE_TOKEN( INT )
		CASE_TOKEN( U16 )
		CASE_TOKEN( U32 )
		CASE_TOKEN( U64 )
		CASE_TOKEN( I16 )
		CASE_TOKEN( I32 )
		CASE_TOKEN( I64 )
		CASE_TOKEN( F32 )
		CASE_TOKEN( F64 )
		CASE_TOKEN( FOR )
		CASE_TOKEN( AND )
		CASE_TOKEN( DOT )
		CASE_TOKEN( IF )
		CASE_TOKEN( I8 )
		CASE_TOKEN( U8 )
		CASE_TOKEN( OR )
	};
	return "Unhandled TokenToString Case";
}
inline std::string SyntaxToString(SyntaxID id){
	switch(id){
		CASE_SYNTAX( Start )
		CASE_SYNTAX( Number )
		CASE_SYNTAX( Ident )
		CASE_SYNTAX( UnaryOp )
		CASE_SYNTAX( Dereference )
		CASE_SYNTAX( BoolNot )
		CASE_SYNTAX( BitwiseNot )
		CASE_SYNTAX( Positive )
		CASE_SYNTAX( Negitive )
		CASE_SYNTAX( Literal )
		CASE_SYNTAX( String )
		CASE_SYNTAX( GreaterEqual )
		CASE_SYNTAX( LessEqual )
		CASE_SYNTAX( Accessor )
		CASE_SYNTAX( NotEqual )
		CASE_SYNTAX( Multiply )
		CASE_SYNTAX( Add )
		CASE_SYNTAX( Subtract )
		CASE_SYNTAX( Modulos )
		CASE_SYNTAX( Divide )
		CASE_SYNTAX( LeftShift )
		CASE_SYNTAX( RightShift )
		CASE_SYNTAX( Equal )
		CASE_SYNTAX( BoolOr )
		CASE_SYNTAX( BoolAnd )
		CASE_SYNTAX( PointerMath )
		CASE_SYNTAX( BitNot )
		CASE_SYNTAX( BitAnd )
		CASE_SYNTAX( BitOr )
		CASE_SYNTAX( BitXor )
		CASE_SYNTAX( Less )
		CASE_SYNTAX( Greater )
		CASE_SYNTAX( MemberAccess )
	};
	return "Unhandled TokenToString Case";
}
inline const std::unordered_map<std::string,TokenID> Lexer_Table = {
	{"namespace",TokenID::NAMESPACE},
	{"operator",TokenID::OPERATOR},
	{"private",TokenID::PRIVATE},
	{"return",TokenID::RETURN},
	{"switch",TokenID::SWITCH},
	{"double",TokenID::DOUBLE},
	{"struct",TokenID::STRUCT},
	{"while",TokenID::WHILE},
	{"using",TokenID::USING},
	{"defer",TokenID::DEFER},
	{"float",TokenID::FLOAT},
	{"const",TokenID::CONST},
	{"else",TokenID::ELSE},
	{"self",TokenID::SELF},
	{"enum",TokenID::ENUM},
	{"uint",TokenID::UINT},
	{"char",TokenID::CHAR},
	{"void",TokenID::VOID},
	{"bool",TokenID::BOOL},
	{"func",TokenID::FUNC},
	{"int",TokenID::INT},
	{"u16",TokenID::U16},
	{"u32",TokenID::U32},
	{"u64",TokenID::U64},
	{"i16",TokenID::I16},
	{"i32",TokenID::I32},
	{"i64",TokenID::I64},
	{"f32",TokenID::F32},
	{"f64",TokenID::F64},
	{"<<=",TokenID::LSHIFTASSIGN},
	{">>=",TokenID::RSHIFTASSIGN},
	{"for",TokenID::FOR},
	{"if",TokenID::IF},
	{"i8",TokenID::I8},
	{"u8",TokenID::U8},
	{"+=",TokenID::ADDASSIGN},
	{"-=",TokenID::SUBASSIGN},
	{"*=",TokenID::MULASSIGN},
	{"%=",TokenID::MODASSIGN},
	{"&=",TokenID::ANDASSIGN},
	{"|=",TokenID::ORASSIGN},
	{"^=",TokenID::XORASSIGN},
	{"/=",TokenID::DIVASSIGN},
	{"<<",TokenID::LSHIFT},
	{">>",TokenID::RSHIFT},
	{"<=",TokenID::LESSEQUAL},
	{">=",TokenID::GREATEREQUAL},
	{"==",TokenID::EQUAL},
	{"!=",TokenID::NOTEQUAL},
	{"||",TokenID::OR},
	{"&&",TokenID::AND},
	{":=",TokenID::SPECIALASSIGN},
	{"::",TokenID::ACCESSOR},
	{"_",TokenID::UNDERSCORE},
	{"=",TokenID::ASSIGN},
	{"[",TokenID::OPENSQUAREBRACKET},
	{"]",TokenID::CLOSEDSQUAREBRACKET},
	{".",TokenID::DOT},
	{"*",TokenID::STAR},
	{"!",TokenID::EXCLAMATION},
	{"~",TokenID::TILDE},
	{"+",TokenID::PLUS},
	{"-",TokenID::MINUS},
	{"%",TokenID::PERCENT},
	{"/",TokenID::SLASH},
	{"&",TokenID::BITAND},
	{"^",TokenID::BITXOR},
	{"|",TokenID::BITOR},
	{"<",TokenID::LESS},
	{">",TokenID::GREATER},
	{";",TokenID::SEMI},
	{"(",TokenID::OPENBRACKET},
	{")",TokenID::CLOSEDBRACKET},
	{"{",TokenID::CURLYOPENBRACKET},
	{"}",TokenID::CURLYCLOSEDBRACKET},
	{"$",TokenID::BILL},
	{",",TokenID::COMMA},
	{":",TokenID::COLON},
};
/* Parser_Table
 * 
 * Start -> Function | Variable | Struct | Enum
 * Variable -> Type & Ident | Type & Ident & Assign | Ident & Ident | Ident & Ident & Assign
 * RawType -> INT | UINT | U8 | U16 | U32 | U64 |
 *            I8 | I16 | I32 | I64 | F32 | F64 |
 *            FLOAT | DOUBLE | CHAR | VOID
 * TypeModifier -> CONST | STAR | BoxOp
 *                 CONST & TypeModifier | STAR & TypeModifier | BoxOp & TypeModifier
 * BoxOp -> OPENSQUAREBRACKET & Expr & CLOSEDSQUAREBRACKET
 * Type -> RawType | TypeModifier(s) & RawType
 * Function -> FUNC & Ident & FuncRet & FuncArguments & Block
 * FuncRet -> OPENSQUAREBRACKET & RetType & CLOSEDSQUAREBRACKET |
 *            OPENSQUAREBRACKET & COLON & RetType & CLOSEDSQUAREBRACKET |
 *            OPENSQUAREBRACKET & RetType & COLON & RetType & CLOSEDSQUAREBRACKET
 * RetType -> Type | EXCLAMATION & Type
 * FuncArguments -> OPENBRACKET & FuncArg & CLOSEDBRACKET
 * FuncArg -> Type & Ident | Type & Ident & COMMA & FuncArg |
 *            Type & Ident & Assign | Type & Ident & Assign & COMMA & FuncArg
 * Assign -> (ADDASSIGN | SUBASSIGN | MULASSIGN | DIVASSIGN |
 *            MODASSIGN | ASSIGN    | ANDASSIGN | ORASSIGN  |
 *            XORASSIGN | SPECIALASSIGN) & Expr
 * Stmt - > Variable | Variable & Stmt | Assignment | Assignment & Stmt |
 *          If | If & Stmt | While | While & Stmt | For | For & Stmt
 * Block -> OPENCURLYBRACKET & Stmt & CLOSEDCURLYBRACKET |
 *          OPENCURLYBRACKET & Block & CLOSEDCURLYBRACKET
 * Assignment -> Ident & Assign
 * If -> IF & Condition & Block | IF & Condition & Block & Else
 * Else -> ELSE & Block | ELSE & Condition & Block | ELSE & Condition & Block & Else
 * While -> WHILE & Condition & Block
 * For -> FOR & Stmt & Condition & Stmt & Block
 * Condition -> OPENBRACKET & Expr & CLOSEDBRACKET
 * Switch -> SWITCH & Condition & OPENCURLYBRACKET & Case & CLOSEDCURLYBRACKET |
 *           SWITCH & Condition & OPENCURLYBRACKET & Case & CLOSEDCURLYBRACKET & Else
 * Case -> OPENBRACKET & Pattern & CLOSEDBRACKET & Block |
 *         OPENBRACKET & Pattern & CLOSEDBRACKET & Block & Case |
 * Pattern -> Number | STAR |
 *            Number & TILDE & Number |
 *            Number & COMMA & Pattern |
 *            Number & TILDE & Number & COMMA & Pattern |
 *            Number & COMMA |
 *            Number & TILDE & Number & COMMA |
 * 
 * Expr -> Literal | UnaryOp | ExprGroup | Literal & BinOp & Expr | UnaryOp & BinOp & Expr |
 *         ExprGroup & BinOp & Expr
 * ExprGroup -> OPENBRACKET & Expr & CLOSEDBRACKET
 * Literal -> Number | Ident | String
 * UnaryOp -> STAR & Expr | EXCLAMATION & Expr | TILDE & Expr | PLUS & Expr | MINUS & Expr
 * BinOp -> STAR | PLUS | MINUS | PERCENT | SLASH | LSHIFT | RSHIFT | LESSEQUAL | GREATEREQUAL | EQUAL |
 *          NOTEQUAL | OR | AND | ACCESSOR | BILL | TILDE | BITAND | BITOR | BITXOR | LESS | GREATER | DOT
 * 
 * 
 * 
 * 
 * 
 * 
*/

inline int checkPriority(TokenID id, bool unary){
	if(unary){
		switch(id){
			case TokenID::STAR:  return 12;
			case TokenID::EXCLAMATION: return 11;
			case TokenID::PLUS:  return 11;
			case TokenID::MINUS: return 11;
			case TokenID::TILDE: return 11;
		};
	}else{
		switch(id){
			case TokenID::DOT:   return 12;
			case TokenID::STAR:    return 10;
			case TokenID::PERCENT: return 10;
			case TokenID::SLASH:  return 10;
			case TokenID::PLUS:    return 9;
			case TokenID::MINUS:   return 9;
			case TokenID::RSHIFT:  return 8;
			case TokenID::LSHIFT:  return 8;
			case TokenID::BITAND:  return 7;
			case TokenID::BITXOR:  return 6;
			case TokenID::BITOR:   return 5;
			case TokenID::LESS:    return 4;
			case TokenID::GREATER: return 4;
			case TokenID::LESSEQUAL:    return 4;
			case TokenID::GREATEREQUAL: return 4;
			case TokenID::EQUAL:    return 3;
			case TokenID::NOTEQUAL: return 3;
			case TokenID::OR:       return 2;
			case TokenID::AND:      return 1;
		}
	}
	return -1;
}
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
inline bool isRawType(TokenID& id){
	switch(id){
		case TokenID::INT: return 1;
		case TokenID::UINT:return 1;
		case TokenID::U8:  return 1;
		case TokenID::U16: return 1;
		case TokenID::U32: return 1;
		case TokenID::U64: return 1;
		case TokenID::I8:  return 1;
		case TokenID::I16: return 1;
		case TokenID::I32: return 1;
		case TokenID::I64: return 1;
		case TokenID::F32: return 1;
		case TokenID::F64: return 1;
		case TokenID::FLOAT:  return 1;
		case TokenID::DOUBLE: return 1;
		case TokenID::CHAR: return 1;
		case TokenID::VOID: return 1;
	}
	return 0;
}

struct Token{
	TokenID id;
	std::string value;
	int row;
	int column;
	int len;
};

struct SyntaxNode{
	SyntaxID id;
	std::string value;
	const Token* token;
	SyntaxNode* A = nullptr;
	SyntaxNode* B = nullptr;
	SyntaxNode* C = nullptr;
	SyntaxNode* D = nullptr;
	SyntaxNode* E = nullptr;
	SyntaxNode* F = nullptr;
	SyntaxNode(SyntaxID ID){
		id = ID;
	}
};

inline std::vector<Token> tokens;
inline SyntaxNode* RootSyntax = nullptr;





































