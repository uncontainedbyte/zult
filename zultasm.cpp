#include "zultasm.h"









std::string toString(InstID id) {
	switch (id) {
		case InstID::ASSIGN:       return "ASSIGN       ";
		case InstID::ASSIGN_R:     return "ASSIGN       ";
		case InstID::ADD:          return "ADD          ";
		case InstID::SUB:          return "SUB          ";
		case InstID::MUL:          return "MUL          ";
		case InstID::DIV:          return "DIV          ";
		case InstID::MOD:          return "MOD          ";
		case InstID::NEGATE:       return "NEGATE       ";
		case InstID::BIT_AND:      return "BIT_AND      ";
		case InstID::BIT_OR:       return "BIT_OR       ";
		case InstID::BIT_XOR:      return "BIT_XOR      ";
		case InstID::BIT_NOT:      return "BIT_NOT      ";
		case InstID::CMP_EQ:       return "CMP_EQ       ";
		case InstID::CMP_NT_EQ:    return "CMP_NT_EQ    ";
		case InstID::CMP_GR:       return "CMP_GR       ";
		case InstID::CMP_LS:       return "CMP_LS       ";
		case InstID::CMP_GR_EQ:    return "CMP_GR_EQ    ";
		case InstID::CMP_LS_EQ:    return "CMP_LS_EQ    ";
		case InstID::CMP_AND:      return "CMP_AND      ";
		case InstID::CMP_OR:       return "CMP_OR       ";
		case InstID::CMP_NOT:      return "CMP_NOT      ";
		case InstID::JMP0:         return "JMP0         ";
		case InstID::JMP1:         return "JMP1         ";
		case InstID::JMP:          return "JMP          ";
		case InstID::LABEL:        return "LABEL        ";
		case InstID::SYSCALL:      return "SYSCALL      ";
		case InstID::SCOPE_START:  return "SCOPE_START  ";
		case InstID::SCOPE_END:    return "SCOPE_END    ";
		case InstID::ARG_I32:      return "ARG_I32      ";
		case InstID::ARG_PTR:      return "ARG_PTR      ";
		case InstID::ARG_F32:      return "ARG_F32      ";
		case InstID::RET_I32:      return "RET_I32      ";
		case InstID::RET_PTR:      return "RET_PTR      ";
		case InstID::CALL:         return "CALL         ";
		case InstID::RET:          return "RET          ";
		case InstID::FUNCTION:     return "FUNCTION     ";
		case InstID::EXTERN:       return "EXTERN       ";
		case InstID::END_FUNCTION: return "END_FUNCTION ";
		case InstID::END_EXTERN:   return "END_EXTERN   ";
		case InstID::END_PROGRAM:  return "END_PROGRAM  ";
		case InstID::EXIT:         return "EXIT         ";
		default: return "UNKNOWN";
	}
}


struct Register{
	float f32;
	int32_t i32;
	void* ptr;
	char type=0;
};

void handleExternalR0(std::string name,std::vector<Register>& args){
	
	if("tut" == name){
		std::cout<<(char*)args[0].ptr<<std::flush;
	}else if("tuti" == name){
		std::cout<<args[0].i32<<std::flush;
	}else if("sleep" == name){
		std::this_thread::sleep_for(std::chrono::milliseconds((int)(args[0].i32)));
	}else{
		std::cout<<"Unhandled Extern"<<std::endl;
	}
	
}
Register handleExternalR1(std::string name,std::vector<Register>& args){
	
	if("tin" == name){
		int val = 0;
		std::cin>>val;
		Register r;
		r.i32 = val;
		r.type = 'i';
		return r;
	}else{
		std::cout<<"Unhandled Extern"<<std::endl;
	}
	
	return {0};
}


struct Function{
	int argCount;
	int retCount;
	int start;
	std::vector<Inst> irs;
	bool isExtern = 0;
};
bool isArg(InstID id){
	switch(id){
		case InstID::ARG_I32:
		case InstID::ARG_PTR:
		case InstID::ARG_F32:
			return true;
	};
	return false;
}
bool isRet(InstID id){
	switch(id){
		case InstID::RET_I32:
		case InstID::RET_PTR:
			return true;
	};
	return false;
}
int Interpreter(std::vector<Inst> IR){
	std::vector<std::unordered_map<uint32_t,Register>> registerStack;
	registerStack.push_back({});
	std::unordered_map<std::string,Function> functions;
	std::vector<int> callStack;
	std::vector<std::string> AllocatedStrings;
	
	int pc=0;
	while(true){
		auto& r = registerStack.back();
		if(pc >= IR.size()) break;
		Inst ir = IR[pc];
		
		switch(ir.id){
			case InstID::EXTERN:{
				Function func;
				std::string label = ir.args[0].s;
				
				pc++;
				ir = IR[pc];
				int C = 0;
				while(isArg(ir.id)){
					C++;
					pc++;
					ir = IR[pc];
				}
				func.argCount = C;
				C = 0;
				while(isRet(ir.id)){
					C++;
					pc++;
					ir = IR[pc];
				}
				func.retCount = C;
				func.start = pc;
				func.isExtern = 1;
				
				functions[label] = func;
			}break;
			case InstID::FUNCTION:{
				Function func;
				std::string label = ir.args[0].s;
				
				pc++;
				ir = IR[pc];
				int C = 0;
				while(isArg(ir.id)){
					C++;
					pc++;
					ir = IR[pc];
				}
				func.argCount = C;
				C = 0;
				while(isRet(ir.id)){
					C++;
					pc++;
					ir = IR[pc];
				}
				func.retCount = C;
				func.start = pc;
				while(ir.id != InstID::END_FUNCTION){
					func.irs.push_back(ir);
					pc++;
					ir = IR[pc];
				}
				
				functions[label] = func;
			}break;
			case InstID::ASSIGN:{
				if('i' == ir.args[1].type){
					r[ir.args[0].i].i32 = ir.args[1].i;
					r[ir.args[0].i].type = 'i';
				}else if('f' == ir.args[1].type){
					r[ir.args[0].i].f32 = ir.args[1].f;
					r[ir.args[0].i].type = 'f';
				}else if('p' == ir.args[1].type){
					r[ir.args[0].i].ptr = ir.args[1].p;
					r[ir.args[0].i].type = 'p';
				}else if('s' == ir.args[1].type){
					AllocatedStrings.push_back(ir.args[1].s);
					r[ir.args[0].i].ptr = AllocatedStrings.back().data();
					r[ir.args[0].i].type = 'p';
				}
			}break;
			case InstID::ASSIGN_R:{
				r[ir.args[0].i] = r[ir.args[1].i];
			}break;
			
			case InstID::RET:{
				if(callStack.empty()){
					std::cout<<"Error: call stack empty"<<std::endl;
				}
				pc = callStack.back();
				callStack.pop_back();
				
				std::vector<Register> args;
				int c = ir.args[0].i;
				
				for(int s=0;s<c;s++){
					args.push_back(r[ir.args[s+1].i]);
				}
				registerStack.pop_back();
				auto& rg = registerStack.back();
				ir = IR[pc];
				for(int s=0;s<c;s++){
					rg[ir.args[s+3].i] = args[s];
				}
			}break;
			case InstID::CALL:{
				if(functions.count(ir.args[0].s)&&functions[ir.args[0].s].isExtern){
					if(ir.args[2].i != functions[ir.args[0].s].argCount){
						std::cout<<"Error: Passed Args doesnt match Function input"<<std::endl;
						std::cout<<ir.args[2].i<<std::endl;
						exit(1);
					}
					
					std::vector<Register> args;
					std::vector<int> rets;
					for(int s=0;s<ir.args[2].i;s++){
						args.push_back(r[ir.args[3+ir.args[1].i+s].i]);
					}
					for(int s=0;s<ir.args[1].i;s++){
						rets.push_back(ir.args[3+s].i);
					}
					if(0 == ir.args[1].i){
						handleExternalR0(ir.args[0].s,args);
					}else if(1 == ir.args[1].i){
						Register tmp = handleExternalR1(ir.args[0].s,args);
						r[rets[0]] = tmp;
					}else{
						std::cout<< "Unhandled Extern return"<<std::endl;
					}
					break;
				}
				callStack.push_back(pc);
				if(functions.count(ir.args[0].s)){
					if(ir.args[2].i != functions[ir.args[0].s].argCount){
						std::cout<<"Error: Passed Args doesnt match Function input"<<std::endl;
						exit(1);
					}
					
					std::vector<Register> args;
					for(int s=0;s<ir.args[2].i;s++){
						args.push_back(r[ir.args[3+ir.args[1].i+s].i]);
					}
					pc = functions[ir.args[0].s].start;
					registerStack.push_back({});
					auto& rg = registerStack.back();
					for(int s=0;s<functions[ir.args[0].s].argCount;s++){
						rg[s] = args[s];
					}
					continue;
				}
			}break;
			case InstID::END_PROGRAM:{
				return 0;
			}break;
			case InstID::EXIT:{
				return r[ir.args[0].i].i32;
			}break;
			case InstID::END_FUNCTION:{
				if(callStack.empty()){
					std::cout<<"Error: call stack empty"<<std::endl;
				}
				pc = callStack.back();
				callStack.pop_back();
				registerStack.pop_back();
			}break;
			
			case InstID::ADD:{
				r[ir.args[0].i].i32 = r[ir.args[1].i].i32 + r[ir.args[2].i].i32;
			}break;
			case InstID::SUB:{
				r[ir.args[0].i].i32 = r[ir.args[1].i].i32 - r[ir.args[2].i].i32;
			}break;
			case InstID::MUL:{
				r[ir.args[0].i].i32 = r[ir.args[1].i].i32 * r[ir.args[2].i].i32;
			}break;
			case InstID::DIV:{
				r[ir.args[0].i].i32 = r[ir.args[1].i].i32 / r[ir.args[2].i].i32;
			}break;
			case InstID::MOD:{
				r[ir.args[0].i].i32 = r[ir.args[1].i].i32 % r[ir.args[2].i].i32;
			}break;
			case InstID::NEGATE:{
				r[ir.args[0].i].i32 = -r[ir.args[1].i].i32;
			}break;
			
			case InstID::CMP_EQ:{
				r[ir.args[0].i].i32 = r[ir.args[1].i].i32 == r[ir.args[2].i].i32;
			}break;
			case InstID::CMP_NT_EQ:{
				r[ir.args[0].i].i32 = r[ir.args[1].i].i32 != r[ir.args[2].i].i32;
			}break;
			case InstID::CMP_GR:{
				r[ir.args[0].i].i32 = r[ir.args[1].i].i32 > r[ir.args[2].i].i32;
			}break;
			case InstID::CMP_LS:{
				r[ir.args[0].i].i32 = r[ir.args[1].i].i32 < r[ir.args[2].i].i32;
			}break;
			case InstID::CMP_GR_EQ:{
				r[ir.args[0].i].i32 = r[ir.args[1].i].i32 >= r[ir.args[2].i].i32;
			}break;
			case InstID::CMP_LS_EQ:{
				r[ir.args[0].i].i32 = r[ir.args[1].i].i32 <= r[ir.args[2].i].i32;
			}break;
			case InstID::CMP_AND:{
				r[ir.args[0].i].i32 = r[ir.args[1].i].i32 && r[ir.args[2].i].i32;
			}break;
			case InstID::CMP_OR:{
				r[ir.args[0].i].i32 = r[ir.args[1].i].i32 || r[ir.args[2].i].i32;
			}break;
			case InstID::CMP_NOT:{
				r[ir.args[0].i].i32 = !r[ir.args[1].i].i32;
			}break;
			
			case InstID::BIT_AND:{
				r[ir.args[0].i].i32 = r[ir.args[1].i].i32 & r[ir.args[2].i].i32;
			}break;
			case InstID::BIT_OR:{
				r[ir.args[0].i].i32 = r[ir.args[1].i].i32 | r[ir.args[2].i].i32;
			}break;
			case InstID::BIT_XOR:{
				r[ir.args[0].i].i32 = r[ir.args[1].i].i32 ^ r[ir.args[2].i].i32;
			}break;
			case InstID::BIT_NOT:{
				r[ir.args[0].i].i32 = ~r[ir.args[1].i].i32;
			}break;
			
			case InstID::JMP1:{
				if(r[ir.args[0].i].i32==0){ break; }
				//std::cout<<"Jump: "<<ir.args[1].s<<std::endl;
				for(int s=0;s<IR.size();s++){
					if(IR[s].id!=InstID::LABEL){ continue; }
					if(IR[s].args[0].s!=ir.args[1].s){ continue; }
					pc = s;
					break;
				}
			}break;
			case InstID::JMP0:{
				if(r[ir.args[0].i].i32!=0){ break; }
				//std::cout<<"Jump: "<<ir.args[1].s<<std::endl;
				for(int s=0;s<IR.size();s++){
					if(IR[s].id!=InstID::LABEL){ continue; }
					if(IR[s].args[0].s!=ir.args[1].s){ continue; }
					pc = s;
					break;
				}
			}break;
			case InstID::JMP:{
				//std::cout<<"Jump: "<<ir.args[0].s<<std::endl;
				for(int s=0;s<IR.size();s++){
					if(IR[s].id!=InstID::LABEL){ continue; }
					if(IR[s].args[0].s!=ir.args[0].s){ continue; }
					pc = s;
					break;
				}
			}break;
			
			case InstID::LABEL:{}break;
			default:{
				std::cout<<"Error: Unhandled Case in Interpreter <"<<toString(ir.id)<<">"<<std::endl;
			}
		}
		pc++;
	}
	
	
	
	
	return -1;
}








void InstBuilder::addInst(InstID id){
	IR.push_back({});
	current = &IR.back();
	current->id = id;
}
void InstBuilder::argFloat(double val){
	if(current == nullptr){
		std::cout<<"Error: tried to assign float without new instruction"<<std::endl;
		exit(1);
	}
	
	current->args.push_back({});
	current->args.back().f = val;
	current->args.back().type = 'f';
}
void InstBuilder::argInt(uint64_t val){
	if(current == nullptr){
		std::cout<<"Error: tried to assign int without new instruction"<<std::endl;
		exit(1);
	}
	
	current->args.push_back({});
	current->args.back().i = val;
	current->args.back().type = 'i';
}
void InstBuilder::argStr(const std::string& val){
	if(current == nullptr){
		std::cout<<"Error: tried to assign string without new instruction"<<std::endl;
		exit(1);
	}
	
	current->args.push_back({});
	current->args.back().s = val;
	current->args.back().type = 's';
}
void InstBuilder::push(){
	if(current == nullptr){
		std::cout<<"Error: tried to push without new instruction"<<std::endl;
		exit(1);
	}
	current = nullptr;
}

std::vector<Inst> InstBuilder::getIR(){
	return IR;
}



void InstBuilder::reset(){
	current = nullptr;
	IR.clear();
}
void InstBuilder::print(){
	
	for(int s=0;s<IR.size();s++){
		std::cout<<toString(IR[s].id)<<"    ";
		for(int ss=0;ss<IR[s].args.size();ss++){
			if(IR[s].args[ss].type=='s'){
				std::cout<<'"'<<IR[s].args[ss].s<<'"';
			}else if(IR[s].args[ss].type=='f'){
				std::cout<<IR[s].args[ss].f;
			}else if(IR[s].args[ss].type=='i'){
				std::cout<<IR[s].args[ss].i;
			}
			std::cout<<" ";
		}
		std::cout<<std::endl;
	}
	std::cout<<std::endl;
}
int InstBuilder::var(std::string str){
	for(int s=0;s<Variables.top().size();s++){
		if(Variables.top()[s].count(str)){
			return Variables.top()[s][str];
		}
	}
	std::cout<<"Error: Variable <"<<str<<"> is not declared in this scope"<<std::endl;
	return -1;
}
void InstBuilder::varAdd(std::string str,int i){
	if(Variables.size()==0) Variables.push({});
	if(Variables.top().size()==0) Variables.top().push_back({});
	for(int s=0;s<Variables.top().size();s++){
		if(Variables.top()[s].count(str)){
			std::cout<<"Error: Variable <"<<str<<"> is already declared in this scope"<<std::endl;
			return;
		}
	}
	if(i==-1){
		Variables.top().back()[str] = RegIndex.top();
		RegIndex.top()++;
	}else{
		Variables.top().back()[str] = i;
	}
}
InstBuilder::Func InstBuilder::func(std::string str){
	if(Functions.count(str)){
		return Functions[str];
	}
	std::cout<<"Error: Function <"<<str<<"> is not declared in this scope"<<std::endl;
	return {-1,-1};
}
void InstBuilder::funcAdd(std::string str,int a,int r){
	if(Functions.count(str)){
		std::cout<<"Error: Function <"<<str<<"> is already declared in this scope"<<std::endl;
		return;
	}
	Functions[str] = {a,r};
}
int InstBuilder::freeReg(){
	RegIndex.top()++;
	return RegIndex.top()-1;
}
std::string InstBuilder::freeJmp(){
	JmpIndex++;
	return "LB"+std::to_string(JmpIndex-1);
}
void InstBuilder::pushStack(){
	Variables.push({});
	RegIndex.push(0);
}
void InstBuilder::popStack(){
	Variables.pop();
	RegIndex.pop();
}
























