// 2 cpp START
//>mk-obj "lexer.cpp" "objs/lexer.o"
//args = "main.cpp objs/*.o"
//mk-exe args "main"
//END

#include "lexer.h"
#include <sstream>
#include <fstream>
#include <iostream>





void printTokens(){
	
	int space=80;
	for(const auto& token:tokens){
		std::string line = TokenToString(token.id);
		if(token.value.size()>0){
			line += "<"+token.value+">";
		}
		if(line.size()>=space){
			space=80;
			std::cout<<std::endl;
			std::cout<<line<<' ';
			space -= (line.size()+1);
		}else{
			std::cout<<line<<' ';
			space -= (line.size()+1);
		}
	}
	std::cout<<std::endl<<std::endl;
	
}
void printPointersToTokens(int tab){
	int ti=0; // token index
	for(uint i = 1; i < file_lines.size(); i++){
		std::string line = file_lines[i];
		int c=0;
		for(int s=0;s<line.size();s++){
			if(line[s]=='\t'){
				line[s] = ' ';
				line.insert(s, tab-1, ' ');
				c+=tab-1;
			}else{ break; }
		}
		std::string pointers(line.size(), ' ');
		
		int any = 0;
		for(;ti<tokens.size();ti++){
			if(tokens[ti].row < i) continue;
			if(tokens[ti].row > i) break;
			
			for(int s=0;s<tokens[ti].len;s++){
				any++;
				pointers[tokens[ti].column+c+s - 1] = '^';
			}
		}
		if(any>0){
			std::cout << "Line" << i << ": " << line << std::endl;
			std::cout << "      "<< std::string(std::to_string(i).size(), ' ') << pointers << std::endl;
		}
	}
	std::cout<<std::endl;
}


int main(){
	std::string fileName = "test.zlt";
	uint debug = 0b11;
	uint compile = 0b1;
	
	{
		std::stringstream contents_stream;
		std::fstream input(fileName, std::ios::in);
		contents_stream << input.rdbuf();
		raw_file = contents_stream.str();
	}
	
	if(compile&0b1) lexer.lex();
	if(debug&0b1) printTokens();
	if(debug&0b10) printPointersToTokens(1);
	
	
	
	
	
	
}
