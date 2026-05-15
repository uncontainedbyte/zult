#include "lexer.h"

std::string TokenTypeToString(TokenType type){
	switch(type){
		case TokenType::Ident: return "Ident";
		case TokenType::Symbol: return "Symbol";
		case TokenType::String: return "String";
		case TokenType::Number: return "Number";
		case TokenType::Eof: return "EOF";
	};
	return "NULL_TYPE(How?\?\?)";
}
void printLineError(std::string line, uint row, uint colum){
	std::cout<<row<<" "<<line<<std::endl;
	std::cout<<" "<<std::string(std::to_string(row).size(),' ')<<std::string(colum-1,' ')<<"^"<<std::endl;
}

std::vector<Token> Lexer::lex(std::string& str) {
	m_str = std::move(str);
	std::istringstream iss(m_str);
	std::string line;
	lines.push_back("this should not be seen");
	while(std::getline(iss, line)){
		lines.push_back(line);
	}
	uint lineNumber = 1;
	uint columnNumber = 1;
	std::vector<Token> tokens;
	while(peek().h){
		begin:
		if(peek().v == '\n'){
			inc();
			lineNumber++;
			columnNumber=1;
			continue;
		}
		if(std::isspace(peek().v)){
			inc();
			columnNumber++;
			continue;
		}
		
		if(std::isalpha(peek().v)||peek().v=='_'){
			tokens.push_back(lexIdent(lineNumber,columnNumber));
			continue;
		}
		
		if(std::isdigit(peek().v)){
			tokens.push_back(lexNumber(lineNumber,columnNumber));
			continue;
		}
		
		if(lexComment(lineNumber,columnNumber)){
			continue;
		}
		
		if(lexString(lineNumber,columnNumber,tokens)){
			continue;
		}
		
		lexSymbol(lineNumber,columnNumber,tokens);
		
	}
	m_index = 0;
	tokens.push_back({ TokenType::Eof, "EOF", lineNumber, columnNumber });
	return tokens;
}


Token Lexer::lexIdent(uint& ln,uint& cn) {
	std::string buf;
	uint tmp=cn;
	buf.push_back(inc());
	cn++;
	while(peek().h && (std::isalnum(peek().v) || peek().v == '_')){
		buf.push_back(inc());
		cn++;
	}
	return { TokenType::Ident, buf, ln, tmp };
}
Token Lexer::lexNumber(uint& ln, uint& cn) {
	std::string buf;
	uint tmp=cn;
	buf.push_back(inc());
	cn++;
	while(peek().h && std::isdigit(peek().v)){
		buf.push_back(inc());
		cn++;
	}
	if(peek().h && (peek().v == 'b' || peek().v == 'x' || peek().v == '.')){
		buf.push_back(inc());
		cn++;
		while(peek().h && std::isdigit(peek().v)){
			buf.push_back(inc());
			cn++;
		}
	}
	return { TokenType::Number, buf, ln, tmp };
}
bool Lexer::lexComment(uint& ln,uint& cn) {
	uint tmp = cn;
	uint tmp1 = ln;
	if(peek().h && peek().v == '/' && peek(1).h && peek(1).v == '/'){
		inc();
		inc();
		cn+=2;
		while(peek().h && peek().v != '\n'){
			inc();
			cn++;
		}
		inc();
		ln++;
		cn = 1;
		return true;
	}
	if(peek().h && peek().v == '/' && peek(1).h && peek(1).v == '\''){
		inc();
		inc();
		cn+=2;
		while(1){
			if(!peek().h || !peek(1).h){
				std::cout<<"ERROR: No End To Comment"<<std::endl;
				printLineError(lines[tmp1],tmp1,tmp);
				break;
			}
			
			if(peek().v == '\'' && peek(1).v == '/'){
				inc();
				inc();
				cn+=2;
				return true;
			}
			if(peek().v == '\n'){ inc(); ln++; cn=1; continue; }
			
			inc();
			cn++;
		}
	}
	return false;
}
bool Lexer::lexString(uint& ln,uint& cn,std::vector<Token>& tokens){
	uint tmpcn = cn;
	uint tmpln = ln;
	if(!peek().h){ return false; }
	if(peek().v != '\"'){ return false; }
	
	inc();
	cn++;
	
	std::string buf;
	while(1){
		if(!peek().h){
			std::cout<<"ERROR: No End To String"<<std::endl;
			printLineError(lines[tmpln],tmpln,tmpcn);
			break;
		}
		if(peek().v == '\"'){
			inc();
			cn++;
			tokens.push_back({ TokenType::String, buf, tmpln, tmpcn });
			return true;
		}
		if('\\' == peek().v){ inc(); cn++;
			if('n' == peek().v){
				buf.push_back('\n');
				inc(); cn++;
			}else if('\"' == peek().v){
				buf.push_back('\"');
				inc(); cn++;
			}else{
				buf.push_back('\\');
			}
			continue;
		}
		if(peek().v == '\n'){ inc(); ln++; cn=1; continue; }
		buf.push_back(inc());
		cn++;
	}
	
	return false;
}
void Lexer::lexSymbol(uint& ln,uint& cn,std::vector<Token>& tokens){
	uint tmp = cn;
	std::string buf;
	while(peek().h){
		if(peek().v == '\n'){
			break;
		}
		if(std::isspace(peek().v)){
			break;
		}
		if(std::isalpha(peek().v)||peek().v=='_') break;
		if(std::isdigit(peek().v)) break;
		if(peek().v=='\"') break;
		if(peek().v=='/'&&peek(1).h&&peek(1).v=='/'){
			break;
		}
		if(peek().v=='/'&&peek(1).h&&peek(1).v=='\''){
			break;
		}
		
		buf.push_back(inc());
		cn++;
	}
	if(buf.size()<1) return;
	tokens.push_back({ TokenType::Symbol, buf, ln, tmp });
}



Lexer::PeekRet Lexer::peek(int offset) const{
	if(m_index + offset >= m_str.size()){
		return {0,'\0'};
	}
	return {1,m_str[m_index + offset]};
}
char Lexer::inc(){
	return m_str.at(m_index++);
}
Lexer::Lexer(){}
