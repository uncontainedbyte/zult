#include "tok_refiner.h"

std::string TokenIDToString(TokenID id) {
    switch (id) {
        case TokenID::INVALID: return "INVALID";

        case TokenID::NUMBER_INT: return "NUMBER_INT";
        case TokenID::NUMBER_FLOAT: return "NUMBER_FLOAT";
        case TokenID::STRING: return "STRING";
        case TokenID::IDENT: return "IDENT";

        case TokenID::IF: return "IF";
        case TokenID::ELSE: return "ELSE";
        case TokenID::MATCH: return "MATCH";
        case TokenID::WHILE: return "WHILE";
        case TokenID::LOOP: return "LOOP";
        case TokenID::FOR: return "FOR";
        case TokenID::FUNC: return "FUNC";
        case TokenID::RET: return "RET";
        case TokenID::OP: return "OPERATION";
        case TokenID::STRUCT: return "STRUCT";
        case TokenID::CONSTRUCTOR: return "CONSTRUCTOR";
        case TokenID::DESTROY: return "DESTROY";
        case TokenID::ALLOC: return "ALLOC";
        case TokenID::FREE: return "FREE";
        case TokenID::GOTO: return "GOTO";
        case TokenID::CONTINUE: return "CONTINUE";
        case TokenID::BREAK: return "BREAK";
        case TokenID::EXTERN: return "EXTERN";

        case TokenID::VOID: return "VOID";
        case TokenID::BOOL: return "BOOL";
        case TokenID::I8: return "I8";
        case TokenID::I16: return "I16";
        case TokenID::I32: return "I32";
        case TokenID::I64: return "I64";
        case TokenID::U8: return "U8";
        case TokenID::U16: return "U16";
        case TokenID::U32: return "U32";
        case TokenID::U64: return "U64";
        case TokenID::F32: return "F32";
        case TokenID::F64: return "F64";

        case TokenID::BRACKET_OPEN: return "BRACKET_OPEN";
        case TokenID::BRACKET_CLOSE: return "BRACKET_CLOSE";
        case TokenID::CURLY_OPEN: return "CURLY_OPEN";
        case TokenID::CURLY_CLOSE: return "CURLY_CLOSE";
        case TokenID::SQUARE_BRACKET_OPEN: return "SQUARE_BRACKET_OPEN";
        case TokenID::SQUARE_BRACKET_CLOSE: return "SQUARE_BRACKET_CLOSE";

        case TokenID::ASSIGN: return "ASSIGN";
        case TokenID::PTR_ARROW: return "PTR_ARROW";
        case TokenID::PLUS: return "PLUS";
        case TokenID::MINUS: return "MINUS";
        case TokenID::ASSIGN_PLUS: return "ASSIGN_PLUS";
        case TokenID::ASSIGN_MINUS: return "ASSIGN_MINUS";
        case TokenID::ASSIGN_MULTIPLY: return "ASSIGN_MULTIPLY";
        case TokenID::ASSIGN_DIVIDE: return "ASSIGN_DIVIDE";
        case TokenID::ASSIGN_MOD: return "ASSIGN_MOD";
        case TokenID::ASSIGN_XOR: return "ASSIGN_XOR";
        case TokenID::ASSIGN_AND: return "ASSIGN_AND";
        case TokenID::ASSIGN_OR: return "ASSIGN_OR";
        case TokenID::EQUAL: return "EQUAL";
        case TokenID::NOT_EQUAL: return "NOT_EQUAL";
        case TokenID::GREATER_EQUAL: return "GREATER_EQUAL";
        case TokenID::LESS_EQUAL: return "LESS_EQUAL";
        case TokenID::BOOL_OR: return "BOOL_OR";
        case TokenID::BOOL_AND: return "BOOL_AND";

        case TokenID::COMMA: return "COMMA";
        case TokenID::TILDA: return "TILDA";
        case TokenID::COLON: return "COLON";
        case TokenID::SEMI: return "SEMI";
        case TokenID::DOT: return "DOT";
        case TokenID::UNDERSCORE: return "UNDERSCORE";
        case TokenID::ASTERISK: return "ASTERISK";
        case TokenID::SLASH: return "SLASH";
        case TokenID::BACKSLASH: return "BACKSLASH";
        case TokenID::GREATER: return "GREATER";
        case TokenID::LESS: return "LESS";
        case TokenID::CARET: return "CARET";
        case TokenID::EXCLAMATION: return "EXCLAMATION";
        case TokenID::PERCENT: return "PERCENT";
        case TokenID::AMPERSAND: return "AMPERSAND";
        case TokenID::PIPE: return "PIPE";
        case TokenID::QUOTE: return "QUOTE";

        default: return "UNKNOWN";
    }
}
namespace{
	void printLineError(std::string line, uint row, uint colum){
		std::cout<<row<<": "<<line<<std::endl;
		std::cout<<"  "<<std::string(std::to_string(row).size(),' ')<<std::string(colum-1,' ')<<"^"<<std::endl;
	}
}

bool isHex(const std::string& str){
	for(int s=0;s<str.size();s++){
		if(str[s]=='x'||str[s]=='X'){
			return true;
		}
	}
	return false;
}
bool isBin(const std::string& str){
	for(int s=0;s<str.size();s++){
		if(str[s]=='b'||str[s]=='B'){
			return true;
		}
	}
	return false;
}
bool isFloat(const std::string& str){
	for(int s=0;s<str.size();s++){
		if(str[s]=='.'){
			return true;
		}
	}
	return false;
}
uint64_t toHex(const std::string& str){
	uint64_t result = 0;
	result = std::stoull(str, nullptr, 16);
	return result;
}
uint64_t toBin(const std::string& str){
	uint64_t result = 0;
	result = std::stoull(str, nullptr, 2);
	return result;
}


struct SymbolEntry{
	TokenID id=TokenID::INVALID;
	std::unordered_map<char,TokenID> e1;
};
const std::unordered_map<char,SymbolEntry> SymbolTable = {
	{'=',{TokenID::ASSIGN,     {{'=',TokenID::EQUAL}}}},
	{'!',{TokenID::EXCLAMATION,{{'=',TokenID::NOT_EQUAL}}}},
	{'>',{TokenID::GREATER,    {{'=',TokenID::GREATER_EQUAL}}}},
	{'<',{TokenID::LESS,       {{'=',TokenID::LESS_EQUAL}}}},
	{'+',{TokenID::PLUS,       {{'=',TokenID::ASSIGN_PLUS}}}},
	{'-',{TokenID::MINUS,      {{'=',TokenID::ASSIGN_MINUS}}}},
	{'*',{TokenID::ASTERISK,   {{'=',TokenID::ASSIGN_MULTIPLY}}}},
	{'/',{TokenID::SLASH,      {{'=',TokenID::ASSIGN_DIVIDE}}}},
	{'%',{TokenID::PERCENT,    {{'=',TokenID::ASSIGN_MOD}}}},
	{'^',{TokenID::CARET,      {{'=',TokenID::ASSIGN_XOR}}}},
	{'&',{TokenID::AMPERSAND,  {{'=',TokenID::ASSIGN_AND},{'&',TokenID::BOOL_AND}}}},
	{'|',{TokenID::PIPE,       {{'=',TokenID::ASSIGN_OR},{'|',TokenID::BOOL_OR}}}},
	{'~',{TokenID::TILDA}},
	{':',{TokenID::COLON}},
	{';',{TokenID::SEMI}},
	{'.',{TokenID::DOT}},
	{'\'',{TokenID::QUOTE}},
	{',',{TokenID::COMMA}},
	{'(',{TokenID::BRACKET_OPEN}},
	{')',{TokenID::BRACKET_CLOSE}},
	{'{',{TokenID::CURLY_OPEN}},
	{'}',{TokenID::CURLY_CLOSE}},
	{'[',{TokenID::SQUARE_BRACKET_OPEN}},
	{']',{TokenID::SQUARE_BRACKET_CLOSE}},
};
const std::unordered_map<std::string,TokenID> KeywordTable = {
	{"if",TokenID::IF},
	{"else",TokenID::ELSE},
	{"match",TokenID::MATCH},
	{"while",TokenID::WHILE},
	{"loop",TokenID::LOOP},
	{"for",TokenID::FOR},
	{"func",TokenID::FUNC},
	{"ret",TokenID::RET},
	{"operation",TokenID::OP},
	{"struct",TokenID::STRUCT},
	{"constructor",TokenID::CONSTRUCTOR},
	{"destroy",TokenID::DESTROY},
	{"alloc",TokenID::ALLOC},
	{"free",TokenID::FREE},
	{"_",TokenID::UNDERSCORE},
	{"goto",TokenID::GOTO},
	{"continue",TokenID::CONTINUE},
	{"break",TokenID::BREAK},
	{"extern",TokenID::EXTERN},
	{"i8",TokenID::I8},
	{"i16",TokenID::I16},
	{"i32",TokenID::I32},
	{"i64",TokenID::I64},
	{"u8",TokenID::U8},
	{"u16",TokenID::U16},
	{"u32",TokenID::U32},
	{"u64",TokenID::U64},
	{"f32",TokenID::F32},
	{"f64",TokenID::F64},
	{"int",TokenID::I32},
	{"uint",TokenID::U32},
	{"float",TokenID::F32},
	{"bool",TokenID::BOOL},
	{"void",TokenID::VOID},
	//{"",TokenID::},
};

RefinedToken ti(TokenType type,TokenID id,uint64_t i,uint l,uint c){
	RefinedToken out;
	out.type = type;
	out.id = id;
	out.data.i = i;
	out.ln = l;
	out.cn = c;
	return out;
}
RefinedToken tf(TokenType type,TokenID id,double f,uint l,uint c){
	RefinedToken out;
	out.type = type;
	out.id = id;
	out.data.f = f;
	out.ln = l;
	out.cn = c;
	return out;
}
RefinedToken ts(TokenType type,TokenID id,const std::string& s,uint l,uint c){
	RefinedToken out;
	out.type = type;
	out.id = id;
	out.data.s = &s;
	out.ln = l;
	out.cn = c;
	return out;
}
std::vector<RefinedToken> RefineTokens(const std::vector<Token>& tokens,const std::vector<std::string>& lines){
	std::vector<RefinedToken> refined;
	
	int index=0;
	while(1){
		const Token& cur = tokens[index];
		if(TokenType::Ident == cur.type){
			if(KeywordTable.count(cur.value)){
				refined.push_back(ti(TokenType::Keyword,KeywordTable.at(cur.value),0,cur.line,cur.column));
			}else{
				refined.push_back(ts(TokenType::Ident,TokenID::IDENT,cur.value,cur.line,cur.column));
			}
		}else if(TokenType::Symbol == cur.type){
			std::string v = cur.value;
			v += "     ";
			for(int s=0;s<cur.value.size();){
				int c=s;
				if(SymbolTable.count(v[s])){
					const SymbolEntry& e = SymbolTable.at(v[s]); s++;
					if(e.e1.count(v[s])){s++;
						refined.push_back({TokenType::Symbol,e.e1.at(v[s-1]),0,cur.line,cur.column+c});
					}else{
						refined.push_back({TokenType::Symbol,e.id,0,cur.line,cur.column+c});
					}
				}else{
					std::cout<<"Error: Invalid Symbol"<<std::endl;
					printLineError(lines[cur.line],cur.line,cur.column+c);
					exit(1);
				}
			}
		}else if(TokenType::Number == cur.type){
			if(cur.value.size()<2){
				refined.push_back(ti(TokenType::Number,TokenID::NUMBER_INT,cur.value[0]-'0',cur.line,cur.column));
			}else if(isHex(cur.value)){
				refined.push_back(ti(TokenType::Number,TokenID::NUMBER_INT,toHex(cur.value),cur.line,cur.column));
			}else if(isBin(cur.value)){
				refined.push_back(ti(TokenType::Number,TokenID::NUMBER_INT,toBin(cur.value),cur.line,cur.column));
			}else if(isFloat(cur.value)){
				refined.push_back(tf(TokenType::Number,TokenID::NUMBER_FLOAT,std::stod(cur.value),cur.line,cur.column));
			}else{
				refined.push_back(ti(TokenType::Number,TokenID::NUMBER_INT,std::stoull(cur.value),cur.line,cur.column));
			}
		}else if(TokenType::String == cur.type){
			refined.push_back(ts(TokenType::String,TokenID::STRING,cur.value,cur.line,cur.column));
		}else if(TokenType::Eof == cur.type){
			break;
		}
		index++;
	}
	return refined;
}





































