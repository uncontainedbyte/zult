// 2 cpp START
//>mk-obj "lexer.cpp" "objs/lexer.o"
//>mk-obj "parser.cpp" "objs/parser.o"
//args = "main.cpp objs/*.o"
//mk-exe args "main"
//END

#include "lexer.h"
#include "parser.h"
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
void printSyntaxTree(SyntaxNode* node = nullptr, int depth = 0, bool isLast = true, std::string prefix = ""){
	if(node==nullptr) node = RootSyntax;
	if(node==nullptr){ std::cout<<"no SyntaxTree!"<<std::endl; return; }
	std::cout << prefix;
	if(depth!=0) std::cout << (isLast ? "└── " : "├── ");
	std::cout << SyntaxToString(node->id);
	if(node->value!="") std::cout << " <" << node->value << ">";
	std::cout << "\n";
	// Print errors
	//for (const auto& error : node->errorData) {
		//std::cout << prefix << ((depth!=0)?(isLast ? "    " : "│   "):"") << "\033[31m[Error] " << error << "\033[0m\n";
	//}
	// Recur for children
	
	if(node->A!=nullptr){
		printSyntaxTree(node->A, depth + 1, node->B==nullptr, prefix + ((depth!=0)?(isLast ? "    " : "│   "):""));
	}
	if(node->B!=nullptr){
		printSyntaxTree(node->B, depth + 1, node->C==nullptr, prefix + ((depth!=0)?(isLast ? "    " : "│   "):""));
	}
	if(node->C!=nullptr){
		printSyntaxTree(node->C, depth + 1, node->D==nullptr, prefix + ((depth!=0)?(isLast ? "    " : "│   "):""));
	}
	if(node->D!=nullptr){
		printSyntaxTree(node->D, depth + 1, node->E==nullptr, prefix + ((depth!=0)?(isLast ? "    " : "│   "):""));
	}
	if(node->E!=nullptr){
		printSyntaxTree(node->E, depth + 1, node->F==nullptr, prefix + ((depth!=0)?(isLast ? "    " : "│   "):""));
	}
	if(node->F!=nullptr){
		printSyntaxTree(node->F, depth + 1, 1, prefix + ((depth!=0)?(isLast ? "    " : "│   "):""));
	}
	if(depth==0) std::cout << "\n";
}

int main(){
	std::string fileName = "test.zlt";
	uint debug = 0b111;
	uint compile = 0b11;
	
	{
		std::stringstream contents_stream;
		std::fstream input(fileName, std::ios::in);
		contents_stream << input.rdbuf();
		raw_file = contents_stream.str();
	}
	
	if(compile&0b1) lexer.lex();
	if(debug&0b1) printTokens();
	if(debug&0b10) printPointersToTokens(1);
	if(compile&0b10) parser.parse();
	if(debug&0b100) printSyntaxTree();
	
	
	
	return 0;
}
