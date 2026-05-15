// 2 cpp START
//>mk-obj "lexer.cpp"
//;>mk-obj "tok_refiner.cpp"
//;>mk-obj "parser.cpp"
//>mk-obj "gen.cpp"
//>mk-obj "zultasm.cpp"
//args = "main.cpp lexer.o tok_refiner.o parser.o gen.o zultasm.o"
//mk-exe args "main"
//END

#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <vector>
#include <cstdlib>

#include "lexer.h"
#include "tok_refiner.h"
#include "parser.h"
#include "gen.h"


void printTokens(std::vector<Token> tokens){
	for(const auto& token:tokens){
		std::cout<<TokenTypeToString(token.type);
		std::cout<<"<"<<token.value<<">"<<"\n";
	}
	std::cout<<std::endl;
}
void printPointersToTokens(const std::vector<std::string>& lines,const std::vector<Token>& tokens,int tab){
	int j = 0;
	for(uint i = 1; i < lines.size(); i++){
		if(lines[i].empty()) continue;
		int c = 0;
		for(int s=0;s<lines[i].size();s++){
			if(lines[i][s]=='\t'){ c+=tab-1; }else{ break; }
		}
		std::cout << "Line" << i << ": " << lines[i] << std::endl;
		std::string pointers(lines[i].size()+c, ' ');
		while(j < tokens.size() && tokens[j].line == i){
			pointers[tokens[j].column+c - 1] = '^';
			j++;
		}
		std::cout << "      "<< std::string(std::to_string(i).size(), ' ') << pointers << std::endl;
	}
	std::cout<<std::endl;
}
void printAST(Node* node, int depth = 0, bool isLast = true, std::string prefix = "") {
	std::cout << prefix;
	if(depth!=0) std::cout << (isLast ? "└── " : "├── ");
	std::cout << NodeTypeToString(node->type);
	std::cout << " {"<<NodeIDToString(node->id)<<"}";
	if(node->value.type=='s') std::cout << " <" << node->value.s << ">";
	if(node->value.type=='i') std::cout << " <" << node->value.i << ">";
	if(node->value.type=='f') std::cout << " <" << node->value.f << ">";
	std::cout << "\n";
	// Print errors
	//for (const auto& error : node->errorData) {
		//std::cout << prefix << ((depth!=0)?(isLast ? "    " : "│   "):"") << "\033[31m[Error] " << error << "\033[0m\n";
	//}
	// Recur for children
	for (size_t i = 0; i < node->lower.size(); i++) {
		if(node->lower[i] == nullptr) continue;
		printAST(node->lower[i], depth + 1, i == node->lower.size() - 1, prefix + ((depth!=0)?(isLast ? "    " : "│   "):""));
	}
	if(depth==0) std::cout << "\n";
}


int main(){
	/*if (argc < 2) {
		std::cerr << "Incorrect usage. Correct usage is..." << std::endl;
		std::cerr << "compiler filename" << std::endl;
		return EXIT_FAILURE;
	}*/
	
	//std::string fileName = argv[1];
	std::string fileName = "test.zlt";
	uint8_t debugMode = 0b00000000;
	
	// Read the input file into a string
	std::string contents;
	{
		std::stringstream contents_stream;
		std::fstream input(fileName, std::ios::in);
		contents_stream << input.rdbuf();
		contents = contents_stream.str();
	}
	
	// Lexer
	std::vector<Token> tokens = lexer.lex(contents);
	std::vector<std::string> lines = lexer.lines;
	
	if(debugMode&0b000000001) printTokens(tokens);
	if(debugMode&0b000000010) printPointersToTokens(lines,tokens,8);
	
	std::vector<RefinedToken> refined = RefineTokens(tokens,lines);
	
	if(debugMode&0b00000100){
		for(int s=0;s<refined.size();s++){
			std::cout<<TokenIDToString(refined[s].id)<<std::endl;
		}std::cout<<std::endl;
	}
	
	Node* AST = parser.parse(refined,lines);
	if(debugMode&0b00001000) printAST(AST);
	
	InstBuilder IR = gen.gen(AST,&lines);
	if(debugMode&0b00010000) IR.print();
	
	int exitCode = Interpreter(IR.getIR());
	
	
	return exitCode;
}












































