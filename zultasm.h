#pragma once

#include <cstdint>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <stack>
#include <thread>

enum class InstID{
	ASSIGN,
	ASSIGN_R,
	ADD,
	SUB,
	MUL,
	DIV,
	MOD,
	NEGATE,
	BIT_AND,
	BIT_OR,
	BIT_XOR,
	BIT_NOT,
	CMP_EQ,
	CMP_NT_EQ,
	CMP_GR,
	CMP_LS,
	CMP_GR_EQ,
	CMP_LS_EQ,
	CMP_AND,
	CMP_OR,
	CMP_NOT,
	JMP1,
	JMP0,
	JMP,
	LABEL,
	SYSCALL,
	SCOPE_START,
	SCOPE_END,
	ARG_I32,
	ARG_PTR,
	ARG_F32,
	RET_I32,
	RET_PTR,
	CALL,
	RET,
	FUNCTION,
	EXTERN,
	END_FUNCTION,
	END_EXTERN,
	END_PROGRAM,
	EXIT,
};



struct InstARG{
	uint64_t i;
	double f;
	void* p;
	char type;
	
	std::string s;
};
struct Inst{
	InstID id;
	std::vector<InstARG> args;
};

int Interpreter(std::vector<Inst> IR);
class InstBuilder{
	public:
		struct Func{
			int argCount = 0;
			int retCount = 0;
		};
		
		void addInst(InstID id);
		void argFloat(double val);
		void argInt(uint64_t val);
		void argStr(const std::string& val);
		void push();
		void reset();
		void print();
		int var(std::string);
		void varAdd(std::string,int i=-1);
		Func func(std::string);
		void funcAdd(std::string,int a,int r);
		int freeReg();
		std::string freeJmp();
		void pushStack();
		void popStack();
		
		std::vector<Inst> getIR();
	private:
		uint32_t JmpIndex = 0;
		
		std::unordered_map<std::string,Func> Functions;
		std::stack<std::vector<std::unordered_map<std::string,int>>> Variables;
		std::stack<uint32_t> RegIndex;
		Inst* current = nullptr;
		std::vector<Inst> IR;
};

int run(std::vector<Inst> ir);































