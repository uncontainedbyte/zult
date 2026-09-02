#pragma once

#include "symbol-table.hpp"
#include <iostream>




class Parser{
	public:
		void parse();
	private:
		int index = 0;
		
		SyntaxNode* Global();
		
		SyntaxNode* Overload();
		SyntaxNode* Namespace();
		SyntaxNode* Using();
		
		SyntaxNode* MultiRetAssign();
		
		SyntaxNode* AccessingChain(int allow=255);
		SyntaxNode* Call(int allow);
		SyntaxNode* CallArg();
		SyntaxNode* Accessor(int allow);
		SyntaxNode* MemberAccess(int allow);
		SyntaxNode* BoxIndex(int allow);
		SyntaxNode* Cast(int allow);
		
		SyntaxNode* Struct();
		SyntaxNode* StructBlock();
		SyntaxNode* StructStmt();
		SyntaxNode* StructDestructor();
		SyntaxNode* StructConstructor();
		
		SyntaxNode* Enum();
		SyntaxNode* EnumBlock();
		SyntaxNode* EnumValue();
		
		SyntaxNode* Stmt();
		SyntaxNode* Assignment();
		SyntaxNode* If();
		SyntaxNode* Condition();
		SyntaxNode* Else();
		SyntaxNode* While();
		SyntaxNode* For();
		SyntaxNode* ForInit();
		SyntaxNode* ForUpdate();
		SyntaxNode* Return();
		SyntaxNode* Switch();
		SyntaxNode* Case();
		SyntaxNode* Pattern();
		SyntaxNode* Defer();
		
		SyntaxNode* Function();
		SyntaxNode* FuncRet();
		SyntaxNode* RetType();
		SyntaxNode* FuncArguments();
		SyntaxNode* FuncArg();
		SyntaxNode* Block();
		SyntaxNode* FuncName();
		
		SyntaxNode* Variable();
		SyntaxNode* RawType();
		SyntaxNode* TypeModifier();
		SyntaxNode* Type();
		SyntaxNode* Assign(int allow = 255);
		
		SyntaxNode* Expr(int p = 0);
		SyntaxNode* Literal();
		SyntaxNode* UnaryOp();
		SyntaxNode* BinOp(const Token* tk);
		
		SyntaxNode* Ident();
		SyntaxNode* Number();
		
		const Token* peek(int offset = 0) const;
		const Token* inc();
		void del(SyntaxNode* node);
		int State(int v = -1);
		bool Eof(SyntaxNode** node,std::vector<std::string>);
};

inline Parser parser;





