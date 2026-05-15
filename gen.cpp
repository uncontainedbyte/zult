#include "gen.h"


namespace{
	int tab = 4;
	void printLineError(std::string line, uint row, uint colum){
		int c = 0;
		for(int s=0;s<line.size();s++){
			if(line[s]=='\t'){ c+=tab; }else{ break; }
		}
		std::cout<<row<<": "<<line<<std::endl;
		std::cout<<"  "<<std::string(std::to_string(row).size(),' ')<<std::string(colum-1+c,' ')<<"^"<<std::endl;
	}
}


void add_extern(InstBuilder& builder,std::string name,int ret=-1,std::vector<int> args = {}){
	
	builder.addInst(InstID::EXTERN);
	builder.argStr(name);
	builder.push();
	
	for(int s=0;s<args.size();s++){
		if(args[s]==1){
			builder.addInst(InstID::ARG_I32);
			builder.push();
		}else if(args[s]==2){
			builder.addInst(InstID::ARG_PTR);
			builder.push();
		}else if(args[s]==3){
			builder.addInst(InstID::ARG_F32);
			builder.push();
		}
	}
	
	if(ret>0){
		builder.addInst(InstID::RET_I32);
		builder.push();
	}
	
	builder.funcAdd(name,args.size(),(ret>0)?1:0);
	
	builder.addInst(InstID::END_EXTERN);
	builder.push();
	
}
void external(InstBuilder& builder){
	
	add_extern(builder,"sleep",-1,{3});
	add_extern(builder,"tin",1,{});
	add_extern(builder,"tut",-1,{2});
	add_extern(builder,"tuti",-1,{1});
	
	
}

InstBuilder CodeGen::gen(Node* node,const std::vector<std::string>* l){
	lines = l;
	builder.reset();
	external(builder);
	
	for(int s=0;s<node->lower.size();s++){
		switch(node->lower[s]->id){
			case NodeID::FUNC:{
				genFunc(node->lower[s]);
			}break;
			case NodeID::EXTERN:{
				genExtern(node->lower[s]);
			}break;
			
		};
	}
	
	builder.addInst(InstID::CALL);
	builder.argStr("main");
	builder.argInt(1);
	builder.argInt(0);
	builder.argInt(0);
	builder.push();
	
	builder.addInst(InstID::EXIT);
	builder.argInt(0);
	builder.push();
	
	builder.addInst(InstID::END_PROGRAM);
	builder.push();
	
	return builder;
}


void CodeGen::genCall(Node* node){
	std::string f = node->lower[0]->value.s;
	std::vector<int> args;
	auto v = builder.func(f);
	
	for(int s=0;s<v.argCount;s++){
		args.push_back(genExpr(node->lower[1+s]));
	}
	
	if(0 == v.retCount){
		builder.addInst(InstID::CALL);
		builder.argStr(f);
		builder.argInt(0);
		builder.argInt(v.argCount);
		for(auto z:args){
			builder.argInt(z);
		}
		builder.push();
	}else{
		std::cout<<"Error: Unhandled Call"<<std::endl;
		if(node->token!=nullptr)
			printLineError(lines->at(node->token->ln),node->token->ln,node->token->cn);
	}
}
void CodeGen::genExtern(Node* node){
	node = node->lower[0];
	builder.addInst(InstID::EXTERN);
	builder.argStr(node->lower[0]->value.s);
	
	int c=0,a=0;
	if(node->lower[1]->child(0)!=nullptr){
		c++;
	}
	if(node->lower[2]->type==NodeType::ARGS){
		for(int z=0;z<node->lower[2]->lower.size();z++){
			a++;
		}
	}
	
	builder.argInt(a);
	builder.argInt(c);
	builder.push();
	
	builder.pushStack();
	
	if(node->lower[2]->type==NodeType::ARGS){
		for(int z=0;z<node->lower[2]->lower.size();z++){
			builder.addInst(InstID::ARG_I32);
			builder.push();
			builder.varAdd(node->lower[2]->lower[z]->lower[1]->value.s);
		}
	}
	if(node->lower[1]->child(0)!=nullptr){
		builder.addInst(InstID::RET_I32);
		builder.push();
	}
	
	builder.funcAdd(node->lower[0]->value.s,a,c);
	
	builder.addInst(InstID::END_EXTERN);
	builder.push();
}
void CodeGen::genFunc(Node* node){
	builder.addInst(InstID::FUNCTION);
	builder.argStr(node->lower[0]->value.s);
	builder.push();
	
	builder.pushStack();
	
	int a=0,c=0;
	if(node->lower[2]->type==NodeType::ARGS){
		for(int z=0;z<node->lower[2]->lower.size();z++){
			builder.addInst(InstID::ARG_I32);
			builder.push();
			builder.varAdd(node->lower[2]->lower[z]->lower[1]->value.s);
			a++;
		}
	}
	if(node->lower[1]->type==NodeType::RET){
		builder.addInst(InstID::RET_I32);
		builder.push();
		c++;
	}
	
	builder.funcAdd(node->lower[0]->value.s,a,c);
	
	genBlock(node->lower[1+c+1]);
	
	builder.popStack();
	
	builder.addInst(InstID::END_FUNCTION);
	builder.push();
}
void CodeGen::genBlock(Node* node){
	
	for(int s=0;s<node->lower.size();s++){
		if(node->lower[s]->type == NodeType::RET){
			genRet(node->lower[s]);
		}else if(node->lower[s]->type == NodeType::DECLARE){
			genDeclare(node->lower[s]);
		}else if(node->lower[s]->type == NodeType::STATEMENT){
			genStatement(node->lower[s]);
		}else if(node->lower[s]->type == NodeType::ASSIGN){
			genAssign(node->lower[s]);
		}else if(node->lower[s]->type == NodeType::CALL){
			genCall(node->lower[s]);
		}else{
			std::cout<<"Error: Unhandled Node <"<<NodeIDToString(node->id)<<"> of Type <"<<NodeTypeToString(node->type)<<">"<<std::endl;
		}
	}
	
}
void CodeGen::genRet(Node* node){
	int r = genExpr(node->lower[0]);
	builder.addInst(InstID::RET);
	builder.argInt(1);
	builder.argInt(r);
	builder.push();
}
int CodeGen::genExpr(Node* node){
	int s = genCMPop(node);
	if(s!=-1) return s;
	
	s = genBITop(node);
	if(s!=-1) return s;
	
	s = genBINop(node);
	if(s!=-1) return s;
	
	if(node->type == NodeType::NUMBER){
		if(node->id == NodeID::NUM_INT){
			builder.addInst(InstID::ASSIGN);
			int r = builder.freeReg();
			builder.argInt(r);
			builder.argInt(node->value.i);
			builder.push();
			return r;
		}
		if(node->id == NodeID::NUM_FLOAT){
			builder.addInst(InstID::ASSIGN);
			int r = builder.freeReg();
			builder.argInt(r);
			builder.argFloat(node->value.f);
			builder.push();
			return r;
		}
	}else if(node->type == NodeType::IDENT){
		int r = builder.var(node->value.s);
		return r;
	}else if(node->type == NodeType::STRING){
		int r = builder.freeReg();
		builder.addInst(InstID::ASSIGN);
		builder.argInt(r);
		builder.argStr(node->value.s);
		builder.push();
		return r;
	}else if(node->type == NodeType::CALL){
		std::string f = node->lower[0]->value.s;
		std::vector<int> args;
		auto v = builder.func(f);
		
		for(int s=0;s<v.argCount;s++){
			args.push_back(genExpr(node->lower[1+s]));
			
		}
		
		if(1 == v.retCount){
			builder.addInst(InstID::CALL);
			builder.argStr(f);
			builder.argInt(1);
			builder.argInt(v.argCount);
			int o = builder.freeReg();
			builder.argInt(o);
			for(auto z:args){
				builder.argInt(z);
			}
			builder.push();
			return o;
		}else{
			std::cout<<"Error: Invalid Function Call"<<std::endl;
			printLineError(lines->at(node->token->ln),node->token->ln,node->token->cn);
		}
	}else{
		std::cout<<"Error: Unhandled Node <"<<NodeIDToString(node->id)<<"> of Type <"<<NodeTypeToString(node->type)<<">"<<std::endl;
	}
	return -1;
}
void CodeGen::genDeclare(Node* node){
	if(node->child(1)->child(0) == nullptr){
		
		std::cout<<"Error: Unhandled genDeclare"<<std::endl;
		
	}else{
		int s = genExpr(node->child(1)->child(0));
		builder.addInst(InstID::ASSIGN_R);
		int r = builder.freeReg();
		builder.argInt(r);
		builder.argInt(s);
		builder.push();
		builder.varAdd(node->lower[1]->value.s,r);
	}
}
void CodeGen::genStatement(Node* node){
	if(node->id == NodeID::IF){
		genIf(node);
	}else if(node->id == NodeID::WHILE){
		genWhile(node);
	}else{
		std::cout<<"Error: Unhandled Node <"<<NodeIDToString(node->id)<<"> of Type <"<<NodeTypeToString(node->type)<<">"<<std::endl;
	}
	
}
void CodeGen::genIf(Node* node){
	int s = genExpr(node->child(0));
	builder.addInst(InstID::JMP0);
	builder.argInt(s);
	std::string j = builder.freeJmp();
	builder.argStr(j);
	builder.push();
	
	genBlock(node->child(1));
	
	std::string lbl = builder.freeJmp();
	
	builder.addInst(InstID::JMP);
	builder.argStr(lbl);
	builder.push();
	
	builder.addInst(InstID::LABEL);
	builder.argStr(j);
	builder.push();
	
	if(node->child(2) != nullptr){
		genElse(node->child(2),lbl);
	}
	
	builder.addInst(InstID::LABEL);
	builder.argStr(lbl);
	builder.push();
}
void CodeGen::genAssign(Node* node){
	int s = genExpr(node->child(1));
	builder.addInst(InstID::ASSIGN_R);
	int r = builder.var(node->child(0)->value.s);
	builder.argInt(r);
	builder.argInt(s);
	builder.push();
}
void CodeGen::genElse(Node* node,std::string lbl){
	if(node->child(0)->type == NodeType::EXPR){
		int o = genExpr(node->child(0));
		
		builder.addInst(InstID::JMP0);
		builder.argInt(o);
		std::string j = builder.freeJmp();
		builder.argStr(j);
		builder.push();
		
		genBlock(node->child(1));
		
		builder.addInst(InstID::JMP);
		builder.argStr(lbl);
		builder.push();
		
		builder.addInst(InstID::LABEL);
		builder.argStr(j);
		builder.push();
		
		if(node->child(2) != nullptr){
			genElse(node->child(2),lbl);
		}
	}else{
		genBlock(node->child(0));
	}
}
void CodeGen::genWhile(Node* node){
	std::string start = builder.freeJmp();
	builder.addInst(InstID::LABEL);
	builder.argStr(start);
	builder.push();
	
	int s = genExpr(node->child(0));
	
	builder.addInst(InstID::JMP0);
	builder.argInt(s);
	std::string j = builder.freeJmp();
	builder.argStr(j);
	builder.push();
	
	genBlock(node->child(1));
	
	std::string lbl = builder.freeJmp();
	
	builder.addInst(InstID::JMP);
	builder.argStr(start);
	builder.push();
	
	builder.addInst(InstID::LABEL);
	builder.argStr(j);
	builder.push();
}





int CodeGen::genCMPop(Node* node){
	switch(node->id){
		case NodeID::EQUAL:{
			int o1 = genExpr(node->lower[0]);
			int o2 = genExpr(node->lower[1]);
			builder.addInst(InstID::CMP_EQ);
			int r = builder.freeReg();
			builder.argInt(r);
			builder.argInt(o1);
			builder.argInt(o2);
			builder.push();
			return r;
		}break;
		case NodeID::NOT_EQUAL:{
			int o1 = genExpr(node->lower[0]);
			int o2 = genExpr(node->lower[1]);
			builder.addInst(InstID::CMP_NT_EQ);
			int r = builder.freeReg();
			builder.argInt(r);
			builder.argInt(o1);
			builder.argInt(o2);
			builder.push();
			return r;
		}break;
		case NodeID::GREATER:{
			int o1 = genExpr(node->lower[0]);
			int o2 = genExpr(node->lower[1]);
			builder.addInst(InstID::CMP_GR);
			int r = builder.freeReg();
			builder.argInt(r);
			builder.argInt(o1);
			builder.argInt(o2);
			builder.push();
			return r;
		}break;
		case NodeID::LESS:{
			int o1 = genExpr(node->lower[0]);
			int o2 = genExpr(node->lower[1]);
			builder.addInst(InstID::CMP_LS);
			int r = builder.freeReg();
			builder.argInt(r);
			builder.argInt(o1);
			builder.argInt(o2);
			builder.push();
			return r;
		}break;
		case NodeID::GREATER_EQUAL:{
			int o1 = genExpr(node->lower[0]);
			int o2 = genExpr(node->lower[1]);
			builder.addInst(InstID::CMP_GR_EQ);
			int r = builder.freeReg();
			builder.argInt(r);
			builder.argInt(o1);
			builder.argInt(o2);
			builder.push();
			return r;
		}break;
		case NodeID::LESS_EQUAL:{
			int o1 = genExpr(node->lower[0]);
			int o2 = genExpr(node->lower[1]);
			builder.addInst(InstID::CMP_LS_EQ);
			int r = builder.freeReg();
			builder.argInt(r);
			builder.argInt(o1);
			builder.argInt(o2);
			builder.push();
			return r;
		}break;
		case NodeID::AND:{
			int o1 = genExpr(node->lower[0]);
			int o2 = genExpr(node->lower[1]);
			builder.addInst(InstID::CMP_AND);
			int r = builder.freeReg();
			builder.argInt(r);
			builder.argInt(o1);
			builder.argInt(o2);
			builder.push();
			return r;
		}break;
		case NodeID::OR:{
			int o1 = genExpr(node->lower[0]);
			int o2 = genExpr(node->lower[1]);
			builder.addInst(InstID::CMP_OR);
			int r = builder.freeReg();
			builder.argInt(r);
			builder.argInt(o1);
			builder.argInt(o2);
			builder.push();
			return r;
		}break;
		case NodeID::NOT:{
			int o1 = genExpr(node->lower[0]);
			builder.addInst(InstID::CMP_NOT);
			int r = builder.freeReg();
			builder.argInt(r);
			builder.argInt(o1);
			builder.push();
			return r;
		}break;
	}
	return -1;
}
int CodeGen::genBITop(Node* node){
	switch(node->id){
		case NodeID::BIT_AND:{
			int o1 = genExpr(node->lower[0]);
			int o2 = genExpr(node->lower[1]);
			builder.addInst(InstID::BIT_AND);
			int r = builder.freeReg();
			builder.argInt(r);
			builder.argInt(o1);
			builder.argInt(o2);
			builder.push();
			return r;
		}break;
		case NodeID::BIT_OR:{
			int o1 = genExpr(node->lower[0]);
			int o2 = genExpr(node->lower[1]);
			builder.addInst(InstID::BIT_OR);
			int r = builder.freeReg();
			builder.argInt(r);
			builder.argInt(o1);
			builder.argInt(o2);
			builder.push();
			return r;
		}break;
		case NodeID::BIT_XOR:{
			int o1 = genExpr(node->lower[0]);
			int o2 = genExpr(node->lower[1]);
			builder.addInst(InstID::BIT_XOR);
			int r = builder.freeReg();
			builder.argInt(r);
			builder.argInt(o1);
			builder.argInt(o2);
			builder.push();
			return r;
		}break;
		case NodeID::BIT_NOT:{
			int o1 = genExpr(node->lower[0]);
			builder.addInst(InstID::BIT_NOT);
			int r = builder.freeReg();
			builder.argInt(r);
			builder.argInt(o1);
			builder.push();
			return r;
		}break;
	}
	return -1;
}
int CodeGen::genBINop(Node* node){
	switch(node->id){
		case NodeID::ADD:{
			int o1 = genExpr(node->lower[0]);
			int o2 = genExpr(node->lower[1]);
			builder.addInst(InstID::ADD);
			int r = builder.freeReg();
			builder.argInt(r);
			builder.argInt(o1);
			builder.argInt(o2);
			builder.push();
			return r;
		}break;
		case NodeID::SUBTRACT:{
			int o1 = genExpr(node->lower[0]);
			int o2 = genExpr(node->lower[1]);
			builder.addInst(InstID::SUB);
			int r = builder.freeReg();
			builder.argInt(r);
			builder.argInt(o1);
			builder.argInt(o2);
			builder.push();
			return r;
		}break;
		case NodeID::MULTIPLY:{
			int o1 = genExpr(node->lower[0]);
			int o2 = genExpr(node->lower[1]);
			builder.addInst(InstID::MUL);
			int r = builder.freeReg();
			builder.argInt(r);
			builder.argInt(o1);
			builder.argInt(o2);
			builder.push();
			return r;
		}break;
		case NodeID::DIVIDE:{
			int o1 = genExpr(node->lower[0]);
			int o2 = genExpr(node->lower[1]);
			builder.addInst(InstID::DIV);
			int r = builder.freeReg();
			builder.argInt(r);
			builder.argInt(o1);
			builder.argInt(o2);
			builder.push();
			return r;
		}break;
		case NodeID::MODULUS:{
			int o1 = genExpr(node->lower[0]);
			int o2 = genExpr(node->lower[1]);
			builder.addInst(InstID::MOD);
			int r = builder.freeReg();
			builder.argInt(r);
			builder.argInt(o1);
			builder.argInt(o2);
			builder.push();
			return r;
		}break;
		case NodeID::NEGATIVE:{
			int o1 = genExpr(node->lower[0]);
			builder.addInst(InstID::NEGATE);
			int r = builder.freeReg();
			builder.argInt(r);
			builder.argInt(o1);
			builder.push();
			return r;
		}break;
		case NodeID::POSITIVE:{
			int o1 = genExpr(node->lower[0]);
			return o1;
		}break;
	}
	return -1;
}

































