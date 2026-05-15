#pragma once

#include "tok_refiner.h"




enum class NodeType{
	ERROR,
	PROG,
	IDENT,
	NUMBER,
	OP,
	EXPR,
	GLOBAL,
	RET,
	TYPE,
	ARGS,
	ARG,
	BLOCK,
	STATEMENT,
	DECLARE,
	ASSIGN,
	CALL,
	STRING,
};
enum class NodeID{
	NONE,
	UNKNOWN,
	ADD,
	SUBTRACT,
	MULTIPLY,
	DIVIDE,
	EQUAL,
	NOT_EQUAL,
	GREATER,
	LESS,
	GREATER_EQUAL,
	LESS_EQUAL,
	OR,
	AND,
	MODULUS,
	NOT,
	BIT_OR,
	BIT_AND,
	BIT_XOR,
	BIT_NOT,
	
	POSITIVE,
	NEGATIVE,
	NUM_INT,
	NUM_FLOAT,
	NUM_BOOL,
	
	VARIABLE,
	FUNC,
	ARG,
	
	VOID, BOOL,
	U8, U16, U32, U64,
	I8, I16, I32, I64,
	F32, F64,
	
	IF,
	ELSE,
	MATCH,
	CASE,
	RANGE,
	WHILE,
	LOOP,
	FOR,
	EXTERN,
};
struct NodeValue{
	char type = 0;
	uint64_t i;
	double f;
	std::string s;
	
	void S(std::string str){ s=str; type='s'; }
	void I(uint32_t it){ i=it; type='i'; }
	void F(double ft){ f=ft; type='f'; }
};

std::string NodeTypeToString(NodeType type);
std::string NodeIDToString(NodeID id);

struct Node{
	NodeType type;
	NodeID id;
	const RefinedToken* token;
	NodeValue value;
	std::vector<Node*> lower;
	
	Node() = delete;
	Node(NodeType _type,NodeID _id){ type = _type; id = _id; }
	
	void push(Node* node){ lower.push_back(node); }
	Node* left() { return lower.size() > 0 ? lower[0] : nullptr; }
	Node* right(){ return lower.size() > 1 ? lower[1] : nullptr; }
	Node* child(uint i){ return i < lower.size() ? lower[i] : nullptr; }
};

class Parser{
	public:
		Node* parse(const std::vector<RefinedToken>&,const std::vector<std::string>&);
		
		
	private:
		
		Node* parseExtern();
		Node* parseCall(int z=0);
		Node* parseAssign();
		Node* parseFor();
		Node* parseVariable();
		Node* parseLoop();
		Node* parseWhile();
		Node* parseMatchCondition();
		Node* parseMatch();
		Node* parseElse();
		Node* parseIf();
		Node* parseRet();
		Node* parseBlock();
		Node* parseArg();
		Node* parseArgs();
		Node* parseType();
		Node* parseReturnTypes();
		Node* parseFunc();
		Node* parseExpr(int prev);
		Node* parseUnary();
		Node* parseNumber();
		Node* parseIdent(NodeID id);
		Node* parseString();
		
		
		NodeID TokenIDtoNodeID(TokenID id, bool isUnary);
		const RefinedToken* peek(int offset=0);
		const RefinedToken* inc();
		int index=0;
		const std::vector<RefinedToken>* tokens;
		const std::vector<std::string>* lines;
};

inline Parser parser;
