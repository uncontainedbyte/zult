// 2 cpp START
//>mk-obj "lexer.cpp" "objs/lexer.o"
//>mk-obj "parser.cpp" "objs/parser.o"
//args = "main.cpp objs/*.o"
//mk-exe args "zultc"
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
	if(node->value!=""&&node->id!=SyntaxID::Error) std::cout << " <" << node->value << ">";
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

void printPointerToToken(const Token* token,int tab){
	if(token->row>=file_lines.size()) return;
	std::string line = file_lines[token->row];
	int c=0;
	for(int s=0;s<line.size();s++){
		if(line[s]=='\t'){
			line[s] = ' ';
			line.insert(s, tab-1, ' ');
			c+=tab-1;
		}else{ break; }
	}
	std::string pointers(line.size(), ' ');
	
	for(int s=0;s<token->len;s++){
		pointers[token->column+c+s - 1] = '^';
	}
	std::cout << "Line" << token->row << ": " << line << std::endl;
	std::cout << "      "<< std::string(std::to_string(token->row).size(), ' ') << pointers << std::endl;
}
std::string Syntax_formatToken(const Token* found){
	if(found->value!="") return "`"+found->value+"`";
	return "`"+SymbolTokenToString(found->id)+"`";
}
std::string Syntax_formatFound(const Token* found){
	return Syntax_formatToken(found);
}
std::string Syntax_formatExpected(const std::vector<std::string>& expected){
	std::string out;
	for(int s=0;s<expected.size();s++){
		out += "`"+expected[s]+"`";
		if(s+1<expected.size()) out += " or ";
	}
	return out;
}
std::string Syntax_formatError(const SyntaxError* error){
	switch(error->kind){
		case SyntaxErrorKind::Expected:
			return "expected " + Syntax_formatExpected(error->expected) +
			       ", found "  + Syntax_formatToken(error->found);
		case SyntaxErrorKind::Unexpected:
			return "unexpected " + Syntax_formatToken(error->found);
		case SyntaxErrorKind::UnexpectedEOF:
			return "unexpected end of file, expected " + Syntax_formatExpected(error->expected);
	}
	return "this should not be seen.";
}
void printSyntaxError(SyntaxNode* node){
	if(node->error==nullptr) return;
	
	std::cout << "line-" << node->error->found->row << ":" << node->error->found->column;
	std::cout << std::endl;
	std::cout << Syntax_formatError(node->error);
	std::cout << std::endl;
	if(node->error->kind != SyntaxErrorKind::UnexpectedEOF) printPointerToToken(node->error->found,1);
}
void printSyntaxErrors(SyntaxNode* node = nullptr){
	if(node==nullptr) node = RootSyntax;
	if(node==nullptr){ return; }
	if(node->id==SyntaxID::Error) printSyntaxError(node);
	
	if(node->A!=nullptr){ printSyntaxErrors(node->A); }
	if(node->B!=nullptr){ printSyntaxErrors(node->B); }
	if(node->C!=nullptr){ printSyntaxErrors(node->C); }
	if(node->D!=nullptr){ printSyntaxErrors(node->D); }
	if(node->E!=nullptr){ printSyntaxErrors(node->E); }
	if(node->F!=nullptr){ printSyntaxErrors(node->F); }
}

int main(){
	std::string fileName = "test.zlt";
	uint debug = 0b1111;
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
	if(debug&0b1000) printSyntaxErrors();
	
	
	
	return 0;
}
