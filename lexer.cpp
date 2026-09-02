#include "lexer.h"



void Lexer::lex(){
	file_lines.push_back("this should not be seen");
	line_buffer = "";
	index = 0;
	column = 1;
	row = 1;
	buffer = "";
	
	int Lcolumn; // local column
	while(peek().h){
		Lcolumn = column;
		if(peek().v=='\n'){
			row++; column=1; inc();
			line_buffer.pop_back();
			file_lines.push_back(line_buffer);
			line_buffer.clear();
			continue;
		}else
		if(std::isspace(peek().v)){
			column++; inc(); continue;
		}else
		if(std::isalpha(peek().v)||peek().v=='_'){
			column++;
			buffer.push_back(inc());
			while(peek().h && (std::isalnum(peek().v) || peek().v == '_')){
				column++;
				buffer.push_back(inc());
			}
			refineIdent(Lcolumn);
			continue;
		}else
		if(std::isdigit(peek().v)){
			column++;
			buffer.push_back(inc());
			while(peek().h && std::isdigit(peek().v)){
				column++;
				buffer.push_back(inc());
			}
			if(peek().h && (peek().v=='x' || peek().v=='b' || peek().v=='X' || peek().v=='B' || peek().v=='.')){
				convertSpecialNums();
			}
			
			tokens.push_back({TokenID::Number,buffer,row,Lcolumn,column-Lcolumn});
			buffer.clear();
			continue;
		}else
		if(peek().v == '\"'){
			column++;
			inc();
			while(peek().h && peek().v!='\"'){
				if(peek().v=='\\'){
					column++;
					inc();
					if(!peek().h) break;
					column++;
					buffer.push_back(specialChar(inc()));
				}else{
					column++;
					buffer.push_back(inc());
				}
			}
			if(!peek().h){
				
				// error, no closing quotes
				
				std::cout<<"No Closing Quotes"<<std::endl;
				
				tokens.push_back({TokenID::eof,"",row,column,0});
				
				return;
			}
			column++;
			inc();
			
			tokens.push_back({TokenID::String,buffer,row,Lcolumn,column-Lcolumn});
			buffer.clear();
			continue;
		}else
		if(peek().v=='/'&&(peek(1).h&&peek(1).v=='\'')){
			inc();inc();
			column++;column++;
			
			while(peek().h&&peek(1).h&&!(peek().v=='\''&&peek(1).v=='/')){
				if(peek().v=='\n'){
					row++; column=1; inc();
					line_buffer.pop_back();
					file_lines.push_back(line_buffer);
					line_buffer.clear();
				}else{
					column++; inc();
				}
			}
			if(!peek().h||!peek(1).h){
				
				// error, unclosed comment
				
				std::cout<<"Unclosed Comment"<<std::endl;
				
				tokens.push_back({TokenID::eof,"",row,column,0});
				
				return;
			}
			inc();inc();
			continue;
		}else
		if(peek().v=='/'&&(peek(1).h&&peek(1).v=='/')){
			inc();inc();
			column++;column++;
			
			while(peek().h){
				if(peek().v=='\n'){
					row++; column=1; inc();
					line_buffer.pop_back();
					file_lines.push_back(line_buffer);
					line_buffer.clear();
					break;
				}else{
					column++; inc();
				}
			}
			continue;
		}else
		if(isSymbol(peek().v)){
			column++;
			buffer.push_back(inc());
			while(peek().h && isSymbol(peek().v)){
				if(peek().v=='/'&&(peek(1).h&&peek(1).v=='\'')) break;
				if(peek().v=='/'&&(peek(1).h&&peek(1).v=='/')) break;
				if(peek().v=='\"') break;
				column++;
				buffer.push_back(inc());
			}
			
			refineSymbols(Lcolumn);
			continue;
		}
		
		tokens.push_back({TokenID::Unknown,std::string(1,peek().v),row,column,1});
		column++;
		inc();
	}
	
	tokens.push_back({TokenID::eof,"",row,column,0});
}


char Lexer::specialChar(char c){
	switch(c){
		case 'n': return '\n';
		case 'r': return '\r';
		case 't': return '\t';
		case 'v': return '\v';
		case 'b': return '\b';
		case 'f': return '\f';
		case 'a': return '\a';
		case '0': return '\0';
		case '\'': return '\'';
		case '\"': return '\"';
		case '\\': return '\\';
		default: return c;
	};
}
bool isHex(char c){
	const char hex[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	const char HEX[16] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
	for(int s=0;s<16;s++){
		if(hex[s]==c){ return true; }
		if(HEX[s]==c){ return true; }
	}
	return false;
}
void Lexer::convertSpecialNums(){
	std::string part1 = buffer;
	std::string part2 = "";
	if(!isHex(peek(1).v)){
		return;
	}
	column++;
	char type = inc();
	while(peek().h && isHex(peek().v)){
		column++;
		part2.push_back(inc());
	}
	if(type == 'x'||type == 'X'){
		buffer = std::to_string(std::stoull(part2, nullptr, 16));
	}else
	if(type == 'b'||type == 'B'){
		buffer = std::to_string(std::stoull(part2, nullptr, 2));
	}else{
		buffer = part1+'.'+part2;
	}
}
void Lexer::refineIdent(int Lcolumn){
	if(Lexer_Table.count(buffer)){
		TokenID id = Lexer_Table.at(buffer);
		tokens.push_back({id,"",row,Lcolumn,column-Lcolumn});
		buffer.clear();
	}else{
		tokens.push_back({TokenID::Ident,buffer,row,Lcolumn,column-Lcolumn});
		buffer.clear();
	}
}
void Lexer::refineSymbols(int Lcolumn){
	if(Lexer_Table.count(buffer)){
		TokenID id = Lexer_Table.at(buffer);
		tokens.push_back({id,"",row,Lcolumn,column-Lcolumn});
		buffer.clear();
	}else{
		std::string tmp;
		bool g = false;
		while(!buffer.empty()){
			for(int s=0;s<buffer.size();s++){
				tmp = buffer.substr(0,buffer.size()-s);
				if(Lexer_Table.count(tmp)){
					TokenID id = Lexer_Table.at(tmp);
					tokens.push_back({id,"",row,Lcolumn,(int)tmp.size()});
					buffer = buffer.substr(buffer.size()-s);
					Lcolumn+=tmp.size();
					g = true;
				}
			}
			if(!g){
				if(buffer.size()>1){
					tokens.push_back({TokenID::Unknown,buffer.substr(0,1),row,Lcolumn,(column-((int)buffer.size()-1))-Lcolumn});
					buffer = buffer.substr(1);
					Lcolumn+=1;
				}else{
					tokens.push_back({TokenID::Unknown,buffer,row,Lcolumn,column-Lcolumn});
					buffer.clear();
				}
			}
			g = false;
		}
	}
}








Lexer::PeekRet Lexer::peek(int offset) const{
	if(index + offset >= raw_file.size()){
		return {0,'\0'};
	}
	return {1,raw_file[index + offset]};
}
char Lexer::inc(){
	char c = raw_file.at(index++);
	line_buffer.push_back(c);
	return c;
}
