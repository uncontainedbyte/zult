#pragma once

#include "symbol-table.hpp"
#include <iostream>




class Parser{
	public:
		void parse();
	private:
		int index = 0;
		
		SyntaxNode* Expr(int p = 0);
		SyntaxNode* Literal();
		SyntaxNode* UnaryOp();
		SyntaxNode* BinOp(const Token* tk);
		
		
		
		
		const Token* peek(int offset = 0) const;
		const Token* inc();
};

inline Parser parser;





