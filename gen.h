#pragma once

#include "parser.h"
#include "zultasm.h"



class CodeGen{
	public:
		InstBuilder gen(Node* node,const std::vector<std::string>* l);
		
		
	private:
		
		void genWhile(Node* node);
		void genCall(Node* node);
		void genExtern(Node* node);
		void genFunc(Node* node);
		void genBlock(Node* node);
		void genRet(Node* node);
		int genExpr(Node* node);
		void genDeclare(Node* node);
		void genStatement(Node* node);
		void genIf(Node* node);
		void genAssign(Node* node);
		void genElse(Node* node,std::string lbl);
		
		int genCMPop(Node* node);
		int genBITop(Node* node);
		int genBINop(Node* node);
		
		const std::vector<std::string>* lines;
		InstBuilder builder;
};

inline CodeGen gen;

























