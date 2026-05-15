#include "parser.h"
#include "lexer.h"

std::string NodeTypeToString(NodeType type){
	switch(type){
		case NodeType::PROG: return "PROG";
		case NodeType::NUMBER: return "NUMBER";
		case NodeType::IDENT: return "IDENT";
		case NodeType::EXPR: return "EXPR";
		case NodeType::OP: return "OP";
		case NodeType::ERROR: return "ERROR";
		case NodeType::GLOBAL: return "GLOBAL";
		case NodeType::RET: return "RET";
		case NodeType::TYPE: return "TYPE";
		case NodeType::ARGS: return "ARGS";
		case NodeType::ARG: return "ARG";
		case NodeType::BLOCK: return "BLOCK";
		case NodeType::STATEMENT: return "STATEMENT";
		case NodeType::DECLARE: return "DECLARE";
		case NodeType::ASSIGN: return "ASSIGN";
		case NodeType::CALL: return "CALL";
		case NodeType::STRING: return "STRING";
		default: return "UNKNOWN";
	};
	return "UNKNOWN";
}
std::string NodeIDToString(NodeID id){
	switch(id){
		case NodeID::NONE: return "NONE";
		case NodeID::ADD: return "ADD";
		case NodeID::SUBTRACT: return "SUBTRACT";
		case NodeID::MULTIPLY: return "MULTIPLY";
		case NodeID::DIVIDE: return "DIVIDE";
		case NodeID::POSITIVE: return "POSITIVE";
		case NodeID::NEGATIVE: return "NEGATIVE";
		case NodeID::NUM_BOOL: return "NUM_BOOL";
		case NodeID::NUM_FLOAT: return "NUM_FLOAT";
		case NodeID::NUM_INT: return "NUM_INT";
		case NodeID::UNKNOWN: return "UNKNOWN";
		case NodeID::VARIABLE: return "VARIABLE";
		case NodeID::OR: return "OR";
		case NodeID::AND: return "AND";
		case NodeID::EQUAL: return "EQUAL";
		case NodeID::NOT_EQUAL: return "NOT_EQUAL";
		case NodeID::LESS: return "LESS";
		case NodeID::LESS_EQUAL: return "LESS_EQUAL";
		case NodeID::GREATER: return "GREATER";
		case NodeID::GREATER_EQUAL: return "GREATER_EQUAL";
		case NodeID::MODULUS: return "MODULUS";
		case NodeID::NOT: return "NOT";
		case NodeID::BIT_OR: return "BIT_OR";
		case NodeID::BIT_XOR: return "BIT_XOR";
		case NodeID::BIT_AND: return "BIT_AND";
		case NodeID::BIT_NOT: return "BIT_NOT";
		case NodeID::FUNC: return "FUNC";
		case NodeID::U8: return "U8";
		case NodeID::U16: return "U16";
		case NodeID::U32: return "U32";
		case NodeID::U64: return "U64";
		case NodeID::I8: return "I8";
		case NodeID::I16: return "I16";
		case NodeID::I32: return "I32";
		case NodeID::I64: return "I64";
		case NodeID::F32: return "F32";
		case NodeID::F64: return "F64";
		case NodeID::BOOL: return "BOOL";
		case NodeID::VOID: return "VOID";
		case NodeID::ARG: return "ARG";
		case NodeID::IF: return "IF";
		case NodeID::ELSE: return "ELSE";
		case NodeID::MATCH: return "MATCH";
		case NodeID::CASE: return "CASE";
		case NodeID::RANGE: return "RANGE";
		case NodeID::WHILE: return "WHILE";
		case NodeID::LOOP: return "LOOP";
		case NodeID::FOR: return "FOR";
		case NodeID::EXTERN: return "EXTERN";
		default: return "UNKNOWN";
	}
	return "UNKNOWN";
}


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

int priority(TokenID id, bool isUnary) {
	if(isUnary){
		switch(id){
			case TokenID::EXCLAMATION:   // !
			case TokenID::TILDA:         // ~
			case TokenID::PLUS:
			case TokenID::MINUS:
				return 11;
			default:
				return -1;
		}
	}else{
		switch (id) {
			case TokenID::ASSIGN:
			case TokenID::ASSIGN_PLUS:
			case TokenID::ASSIGN_MINUS:
			case TokenID::ASSIGN_MULTIPLY:
			case TokenID::ASSIGN_DIVIDE:
			case TokenID::ASSIGN_MOD:
			case TokenID::ASSIGN_XOR:
			case TokenID::ASSIGN_AND:
			case TokenID::ASSIGN_OR:
				return 1;
			case TokenID::BOOL_OR:
				return 2;
			case TokenID::BOOL_AND:
				return 3;
			case TokenID::EQUAL:
			case TokenID::NOT_EQUAL:
				return 4;
			case TokenID::LESS:
			case TokenID::LESS_EQUAL:
			case TokenID::GREATER:
			case TokenID::GREATER_EQUAL:
				return 5;
			case TokenID::PIPE:
				return 6;
			case TokenID::CARET:
				return 7;
			case TokenID::AMPERSAND:
				return 8;
			case TokenID::PLUS:
			case TokenID::MINUS:
				return 9;
			case TokenID::ASTERISK:
			case TokenID::SLASH:
			case TokenID::PERCENT:
				return 10;
			case TokenID::DOT:
			case TokenID::PTR_ARROW:
				return 12;
			default:
				return -1;
		}
	}
}

const RefinedToken* Parser::peek(int offset){
	if(tokens->size() > index+offset){
		return &tokens->at(index+offset);
	}
	return nullptr;
}
const RefinedToken* Parser::inc(){
	const RefinedToken* tok = peek();
	index++;
	return tok;
}
NodeID Parser::TokenIDtoNodeID(TokenID id, bool isUnary=0){
	if(isUnary){
		switch(id){
			case TokenID::TILDA: return NodeID::BIT_NOT;
			case TokenID::EXCLAMATION: return NodeID::NOT;
			case TokenID::PLUS:  return NodeID::POSITIVE;
			case TokenID::MINUS: return NodeID::NEGATIVE;
			default: return NodeID::UNKNOWN;
		}
	}else{
		switch(id){
			//case TokenID::ASSIGN:
			//case TokenID::ASSIGN_PLUS:
			//case TokenID::ASSIGN_MINUS:
			//case TokenID::ASSIGN_MULTIPLY:
			//case TokenID::ASSIGN_DIVIDE:
			//case TokenID::ASSIGN_MOD:
			//case TokenID::ASSIGN_XOR:
			//case TokenID::ASSIGN_AND:
			//case TokenID::ASSIGN_OR:
			case TokenID::BOOL_OR:    return NodeID::OR;
			case TokenID::BOOL_AND:   return NodeID::AND;
			case TokenID::EQUAL:      return NodeID::EQUAL;
			case TokenID::NOT_EQUAL:  return NodeID::NOT_EQUAL;
			case TokenID::LESS:       return NodeID::LESS;
			case TokenID::LESS_EQUAL: return NodeID::LESS_EQUAL;
			case TokenID::GREATER:    return NodeID::GREATER;
			case TokenID::GREATER_EQUAL:return NodeID::GREATER_EQUAL;
			case TokenID::PIPE:       return NodeID::BIT_OR;
			case TokenID::CARET:      return NodeID::BIT_XOR;
			case TokenID::AMPERSAND:  return NodeID::BIT_AND;
			case TokenID::PLUS:       return NodeID::ADD;
			case TokenID::MINUS:      return NodeID::SUBTRACT;
			case TokenID::ASTERISK:   return NodeID::MULTIPLY;
			case TokenID::SLASH:      return NodeID::DIVIDE;
			case TokenID::PERCENT:    return NodeID::MODULUS;
			//case TokenID::DOT:
			//case TokenID::PTR_ARROW:
			case TokenID::I8:  return NodeID::I8;
			case TokenID::I16: return NodeID::I16;
			case TokenID::I32: return NodeID::I32;
			case TokenID::I64: return NodeID::I64;
			case TokenID::U8:  return NodeID::U8;
			case TokenID::U16: return NodeID::U16;
			case TokenID::U32: return NodeID::U32;
			case TokenID::U64: return NodeID::U64;
			case TokenID::F32: return NodeID::F32;
			case TokenID::F64: return NodeID::F64;
			case TokenID::BOOL: return NodeID::BOOL;
			case TokenID::VOID: return NodeID::VOID;
			default: return NodeID::UNKNOWN;
		}
	}
	return NodeID::UNKNOWN;
}
bool isType(TokenID id){
	switch(id){
		case TokenID::I8:
		case TokenID::I16:
		case TokenID::I32:
		case TokenID::I64:
		case TokenID::U8:
		case TokenID::U16:
		case TokenID::U32:
		case TokenID::U64:
		case TokenID::F32:
		case TokenID::F64:
		case TokenID::VOID:
		case TokenID::BOOL:
		return 1;
		default: return 0;
	}
	return 0;
}
bool isAssign(TokenID id){
	switch(id){
		case TokenID::ASSIGN:
		case TokenID::ASSIGN_AND:
		case TokenID::ASSIGN_DIVIDE:
		case TokenID::ASSIGN_MINUS:
		case TokenID::ASSIGN_MOD:
		case TokenID::ASSIGN_MULTIPLY:
		case TokenID::ASSIGN_OR:
		case TokenID::ASSIGN_PLUS:
		case TokenID::ASSIGN_XOR:
		return 1;
		default: return 0;
	}
	return 0;
}






Node* Parser::parse(const std::vector<RefinedToken>& _tokens,const std::vector<std::string>& _lines){
	tokens = &_tokens;
	lines = &_lines;
	
	Node* node = new Node(NodeType::PROG,NodeID::NONE);
	
	while(true){
		if(peek()==nullptr) return node;
		if(peek()->id == TokenID::FUNC){
			node->push(parseFunc());
		}else if(peek()->id == TokenID::EXTERN){
			node->push(parseExtern());
		}else{
			Node* t = new Node(NodeType::ERROR,NodeID::NONE);
			t->token = inc();
			node->push(t);
		}
	}
	
	return node;
}


Node* Parser::parseExtern(){
	if(peek() == nullptr) return nullptr;
	Node* root = new Node(NodeType::GLOBAL,NodeID::EXTERN);
	root->token = inc();
	
	Node* node = new Node(NodeType::GLOBAL,NodeID::FUNC);
	node->token = inc();
	
	root->push(node);
	
	Node* tmp = parseIdent(NodeID::FUNC);
	if(tmp == nullptr) return root;
	node->push(tmp);
	
	tmp = parseReturnTypes();
	if(tmp == nullptr) return root;
	node->push(tmp);
	
	tmp = parseArgs();
	if(tmp == nullptr) return root;
	node->push(tmp);
	
	if(peek()->id == TokenID::SEMI){
		inc();
	}else{
		std::cout<<"Error: Missing Semicolon ';'"<<std::endl;
		printLineError(lines->at(peek()->ln),peek()->ln,peek()->cn);
	}
	
	return root;
}
Node* Parser::parseCall(int z){
	Node* node = new Node(NodeType::CALL,NodeID::NONE);
	
	node->push(parseIdent(NodeID::NONE));
	inc();
	
	while(1){
		if(peek()->id == TokenID::BRACKET_CLOSE) break;
		Node* tmp;
		tmp = parseExpr(0);
		node->push(tmp);
		if(peek()->id == TokenID::COMMA){ inc(); }else{break;}
	}
	
	if(peek()->id != TokenID::BRACKET_CLOSE){
		std::cout<<"Error: Expected Closing Bracket ')'"<<std::endl;
		printLineError(lines->at(peek()->ln),peek()->ln,peek()->cn);
		return node;
	}
	inc();
	
	if(z==0) return node;
	
	if(peek()->id == TokenID::SEMI){
		inc();
	}else{
		std::cout<<"Error: Missing Semicolon ';'"<<std::endl;
		printLineError(lines->at(peek()->ln),peek()->ln,peek()->cn);
	}
	
	return node;
}
Node* Parser::parseAssign(){
	if(peek() == nullptr) return nullptr;
	Node* node = new Node(NodeType::ASSIGN,NodeID::VARIABLE);
	
	Node* tmp = parseIdent(NodeID::VARIABLE);
	if(tmp == nullptr) return node;
	node->push(tmp);
	Node* var = tmp;
	Node* op;
	bool f=0;
	
	switch(peek()->id){
		case TokenID::ASSIGN:{
			inc();
			tmp = parseExpr(0);
			if(tmp == nullptr) return node;
			node->push(tmp);
			f = false;
		}break;
		case TokenID::ASSIGN_AND:{
			op = new Node(NodeType::EXPR,NodeID::BIT_AND);
			op->token = inc();
			f = true;
		}break;
		case TokenID::ASSIGN_DIVIDE:{
			op = new Node(NodeType::EXPR,NodeID::DIVIDE);
			op->token = inc();
			f = true;
		}break;
		case TokenID::ASSIGN_MINUS:{
			op = new Node(NodeType::EXPR,NodeID::SUBTRACT);
			op->token = inc();
			f = true;
		}break;
		case TokenID::ASSIGN_MOD:{
			op = new Node(NodeType::EXPR,NodeID::MODULUS);
			op->token = inc();
			f = true;
		}break;
		case TokenID::ASSIGN_MULTIPLY:{
			op = new Node(NodeType::EXPR,NodeID::MULTIPLY);
			op->token = inc();
			f = true;
		}break;
		case TokenID::ASSIGN_OR:{
			op = new Node(NodeType::EXPR,NodeID::BIT_OR);
			op->token = inc();
			f = true;
		}break;
		case TokenID::ASSIGN_PLUS:{
			op = new Node(NodeType::EXPR,NodeID::ADD);
			op->token = inc();
			f = true;
		}break;
		case TokenID::ASSIGN_XOR:{
			op = new Node(NodeType::EXPR,NodeID::BIT_XOR);
			op->token = inc();
			f = true;
		}break;
		default: return node;
	}
	if(f){
		op->push(var);
		
		tmp = parseExpr(0);
		if(tmp == nullptr) return node;
		op->push(tmp);
		
		node->push(op);
	}
	if(peek()->id == TokenID::SEMI){
		inc();
		return node;
	}
	
	return node;
}
Node* Parser::parseFor(){
	if(peek() == nullptr) return nullptr;
	Node* node = new Node(NodeType::STATEMENT,NodeID::FOR);
	node->token = inc();
	
	if(peek()->id != TokenID::BRACKET_OPEN){
		std::cout<<"Error: Expected Opening Bracket '('"<<std::endl;
		printLineError(lines->at(peek()->ln),peek()->ln,peek()->cn);
		return node;
	}
	inc();
	
	Node* tmp = parseVariable();
	if(tmp == nullptr) return node;
	node->push(tmp);
	
	if(peek()->id == TokenID::SEMI){
		inc();
	}
	
	tmp = parseExpr(0);
	if(tmp == nullptr) return node;
	node->push(tmp);
	
	if(peek()->id == TokenID::SEMI){
		inc();
	}
	
	tmp = parseAssign();
	if(tmp == nullptr) return node;
	node->push(tmp);
	
	if(peek()->id != TokenID::BRACKET_CLOSE){
		std::cout<<"Error: Expected Closing Bracket ')'"<<std::endl;
		printLineError(lines->at(peek()->ln),peek()->ln,peek()->cn);
		return node;
	}
	inc();
	
	tmp = parseBlock();
	if(tmp == nullptr) return node;
	node->push(tmp);
	
	return node;
}
Node* Parser::parseVariable(){
	if(peek() == nullptr) return nullptr;
	Node* node = new Node(NodeType::DECLARE,NodeID::VARIABLE);
	Node* last;
	Node* tmp = parseType();
	if(tmp == nullptr) return node;
	node->push(tmp);
	
	while(1){
		if(peek()->type == TokenType::Ident){
			tmp = parseIdent(NodeID::VARIABLE);
			if(tmp == nullptr) return node;
			node->push(tmp);
			last = tmp;
			if(isAssign(peek()->id)){
				inc();
				tmp = parseExpr(0);
				if(tmp == nullptr) return node;
				last->push(tmp);
			}else if(peek()->id == TokenID::COMMA){
				inc();
				continue;
			}else{ break; }
		}
		break;
	}
	
	if(peek()->id == TokenID::SEMI){
		inc();
		return node;
	}
	
	return node;
}
Node* Parser::parseLoop(){
	if(peek() == nullptr) return nullptr;
	Node* node = new Node(NodeType::STATEMENT,NodeID::LOOP);
	node->token = inc();
	
	if(peek()->id == TokenID::CURLY_OPEN){
		Node* tmp = parseBlock();
		if(tmp == nullptr) return node;
		node->push(tmp);
		return node;
	}
	
	if(peek()->id != TokenID::BRACKET_OPEN){
		std::cout<<"Error: Expected Opening Bracket '('"<<std::endl;
		printLineError(lines->at(peek()->ln),peek()->ln,peek()->cn);
		return node;
	}
	inc();
	
	Node* tmp = parseExpr(0);
	if(tmp == nullptr) return node;
	node->push(tmp);
	
	if(peek()->id == TokenID::BRACKET_CLOSE){
		inc();
		tmp = parseBlock();
		if(tmp == nullptr) return node;
		node->push(tmp);
		return node;
	}
	
	if(peek()->id == TokenID::COLON){
		inc();
		tmp = parseIdent(NodeID::VARIABLE);
		if(tmp == nullptr) return node;
		node->push(tmp);
	}
	
	if(peek()->id == TokenID::COLON){
		inc();
		tmp = parseIdent(NodeID::VARIABLE);
		if(tmp == nullptr) return node;
		node->push(tmp);
	}
	
	if(peek()->id != TokenID::BRACKET_CLOSE){
		std::cout<<"Error: Expected Closing Bracket ')'"<<std::endl;
		printLineError(lines->at(peek()->ln),peek()->ln,peek()->cn);
		return node;
	}
	inc();
	
	tmp = parseBlock();
	if(tmp == nullptr) return node;
	node->push(tmp);
	
	return node;
}
Node* Parser::parseWhile(){
	if(peek() == nullptr) return nullptr;
	Node* node = new Node(NodeType::STATEMENT,NodeID::WHILE);
	node->token = inc();
	
	if(peek()->id != TokenID::BRACKET_OPEN){
		std::cout<<"Error: Expected Opening Bracket '('"<<std::endl;
		printLineError(lines->at(peek()->ln),peek()->ln,peek()->cn);
		return node;
	}
	inc();
	
	Node* tmp = parseExpr(0);
	if(tmp == nullptr) return node;
	node->push(tmp);
	
	if(peek()->id != TokenID::BRACKET_CLOSE){
		std::cout<<"Error: Expected Closing Bracket ')'"<<std::endl;
		printLineError(lines->at(peek()->ln),peek()->ln,peek()->cn);
		return node;
	}
	inc();
	
	tmp = parseBlock();
	if(tmp == nullptr) return node;
	node->push(tmp);
	
	if(peek()->id == TokenID::ELSE){
		tmp = parseElse();
		if(tmp == nullptr) return node;
		node->push(tmp);
	}
	
	return node;
}
Node* Parser::parseMatch(){
	if(peek() == nullptr) return nullptr;
	Node* node = new Node(NodeType::STATEMENT,NodeID::MATCH);
	node->token = inc();
	
	if(peek()->id != TokenID::BRACKET_OPEN){
		std::cout<<"Error: Expected Opening Bracket '('"<<std::endl;
		printLineError(lines->at(peek()->ln),peek()->ln,peek()->cn);
		return node;
	}
	inc();
	
	Node* tmp;
	if(peek()->type == TokenType::Ident){
		tmp = parseIdent(NodeID::VARIABLE);
	}else if(peek()->type == TokenType::Number){
		tmp = parseNumber();
	}else{
		std::cout<<"Error: Expected Variable or Number"<<std::endl;
		printLineError(lines->at(peek()->ln),peek()->ln,peek()->cn);
		return node;
	}
	node->push(tmp);
	
	if(peek()->id != TokenID::BRACKET_CLOSE){
		std::cout<<"Error: Expected Closing Bracket ')'"<<std::endl;
		printLineError(lines->at(peek()->ln),peek()->ln,peek()->cn);
		return node;
	}
	inc();
	
	if(peek()->id != TokenID::CURLY_OPEN){
		std::cout<<"Error: Expected Opening Curly '{'"<<std::endl;
		printLineError(lines->at(peek()->ln),peek()->ln,peek()->cn);
		return node;
	}
	inc();
	
	
	while(1){
		if(peek() == nullptr) return node;
		if(peek()->id == TokenID::CURLY_CLOSE) break;
		
		tmp = nullptr;
		if(peek()->id == TokenID::BRACKET_OPEN){
			tmp = parseMatchCondition();
		}
		
		if(tmp == nullptr) break;
		node->push(tmp);
	}
	
	
	
	if(peek()->id != TokenID::CURLY_CLOSE){
		std::cout<<"Error: Expected Closing Curly '}'"<<std::endl;
		printLineError(lines->at(peek()->ln),peek()->ln,peek()->cn);
		return node;
	}
	inc();
	
	if(peek()->id == TokenID::ELSE){
		tmp = parseElse();
		if(tmp == nullptr) return node;
		node->push(tmp);
	}
	
	return node;
}
Node* Parser::parseMatchCondition(){
	if(peek() == nullptr) return nullptr;
	if(peek()->id != TokenID::BRACKET_OPEN) return nullptr;
	Node* node = new Node(NodeType::ARG,NodeID::CASE);
	node->token = inc();
	
	while(1){
		if(peek() == nullptr) return node;
		if(peek()->id == TokenID::BRACKET_CLOSE) break;
		
		Node* tmp = nullptr;
		if(peek()->id == TokenID::IDENT){
			tmp = parseIdent(NodeID::VARIABLE);
		}else if(peek()->type == TokenType::Number){
			tmp = parseNumber();
		}else if(peek()->id == TokenID::COMMA){
			inc();
			continue;
		}else if(peek()->id == TokenID::TILDA){
			tmp = new Node(NodeType::EXPR,NodeID::RANGE);
			tmp->token = inc();
		}
		
		if(tmp == nullptr) break;
		node->push(tmp);
	}
	
	if(peek()->id != TokenID::BRACKET_CLOSE){
		std::cout<<"Error: Expected Closing Bracket ')'"<<std::endl;
		printLineError(lines->at(peek()->ln),peek()->ln,peek()->cn);
		return node;
	}
	inc();
	
	Node* tmp = parseBlock();
	if(tmp == nullptr) return node;
	node->push(tmp);
	
	return node;
}
Node* Parser::parseFunc(){
	if(peek() == nullptr) return nullptr;
	if(peek()->id != TokenID::FUNC) return nullptr;
	
	Node* node = new Node(NodeType::GLOBAL,NodeID::FUNC);
	node->token = inc();
	
	Node* tmp = parseIdent(NodeID::FUNC);
	if(tmp == nullptr) return node;
	node->push(tmp);
	
	tmp = parseReturnTypes();
	if(tmp == nullptr) return node;
	node->push(tmp);
	
	tmp = parseArgs();
	if(tmp == nullptr) return node;
	node->push(tmp);
	
	tmp = parseBlock();
	if(tmp == nullptr) return node;
	node->push(tmp);
	
	return node;
}
Node* Parser::parseIf(){
	if(peek() == nullptr) return nullptr;
	Node* node = new Node(NodeType::STATEMENT,NodeID::IF);
	node->token = inc();
	
	if(peek()->id != TokenID::BRACKET_OPEN){
		std::cout<<"Error: Expected Opening Bracket '('"<<std::endl;
		printLineError(lines->at(peek()->ln),peek()->ln,peek()->cn);
		return node;
	}
	inc();
	
	Node* tmp = parseExpr(0);
	if(tmp == nullptr) return node;
	node->push(tmp);
	
	if(peek()->id != TokenID::BRACKET_CLOSE){
		std::cout<<"Error: Expected Closing Bracket ')'"<<std::endl;
		printLineError(lines->at(peek()->ln),peek()->ln,peek()->cn);
		return node;
	}
	inc();
	
	tmp = parseBlock();
	if(tmp == nullptr) return node;
	node->push(tmp);
	
	if(peek()->id == TokenID::ELSE){
		tmp = parseElse();
		if(tmp == nullptr) return node;
		node->push(tmp);
	}
	
	return node;
}
Node* Parser::parseElse(){
	if(peek() == nullptr) return nullptr;
	Node* node = new Node(NodeType::STATEMENT,NodeID::ELSE);
	node->token = inc();
	Node* tmp;
	if(peek()->id == TokenID::CURLY_OPEN){
		tmp = parseBlock();
		if(tmp == nullptr) return node;
		node->push(tmp);
		
		if(peek()->id == TokenID::ELSE){
			tmp = parseElse();
			if(tmp == nullptr) return node;
			node->push(tmp);
		}
		return node;
	}else if(peek()->id != TokenID::BRACKET_OPEN){
		std::cout<<"Error: Expected Opening Curly '{'"<<std::endl;
		printLineError(lines->at(peek()->ln),peek()->ln,peek()->cn);
		return node;
	}
	inc();
	
	tmp = parseExpr(0);
	if(tmp == nullptr) return node;
	node->push(tmp);
	
	if(peek()->id != TokenID::BRACKET_CLOSE){
		std::cout<<"Error: Expected Closing Bracket ')'"<<std::endl;
		printLineError(lines->at(peek()->ln),peek()->ln,peek()->cn);
		return node;
	}
	inc();
	
	tmp = parseBlock();
	if(tmp == nullptr) return node;
	node->push(tmp);
	
	if(peek()->id == TokenID::ELSE){
		tmp = parseElse();
		if(tmp == nullptr) return node;
		node->push(tmp);
	}
	
	return node;
}
Node* Parser::parseBlock(){
	if(peek() == nullptr) return nullptr;
	if(peek()->id != TokenID::CURLY_OPEN) return nullptr;
	
	Node* node = new Node(NodeType::BLOCK,NodeID::FUNC);
	node->token = inc();
	
	if(peek()->id == TokenID::CURLY_CLOSE){
		inc();
		return node;
	}
	
	while(true){
		if(peek() == nullptr) return node;
		if(peek()->id == TokenID::CURLY_CLOSE) break;
		
		Node* tmp = nullptr;
		if(peek()->id == TokenID::RET){
			tmp = parseRet();
		}else if(peek()->id == TokenID::IF){
			tmp = parseIf();
		}else if(peek()->id == TokenID::MATCH){
			tmp = parseMatch();
		}else if(peek()->id == TokenID::WHILE){
			tmp = parseWhile();
		}else if(peek()->id == TokenID::LOOP){
			tmp = parseLoop();
		}else if(isType(peek()->id)){
			tmp = parseVariable();
		}else if(peek()->id == TokenID::FOR){
			tmp = parseFor();
		}else if(peek()->id == TokenID::IDENT && isAssign(peek(1)->id)){
			tmp = parseAssign();
		}else if(peek()->id == TokenID::IDENT && peek(1)->id == TokenID::BRACKET_OPEN){
			tmp = parseCall(1);
		}
		
		if(tmp == nullptr) break;
		node->push(tmp);
	}
	
	if(peek()->id == TokenID::CURLY_CLOSE){
		inc();
	}else{
		std::cout<<"Error: Expected Closing Curly '}'"<<std::endl;
		printLineError(lines->at(peek()->ln),peek()->ln,peek()->cn);
	}
	
	return node;
}
Node* Parser::parseRet(){
	if(peek() == nullptr) return nullptr;
	if(peek()->id != TokenID::RET) return nullptr;
	Node* node = new Node(NodeType::RET,NodeID::NONE);
	node->token = inc();
	
	while(true){
		if(peek() == nullptr) return node;
		if(peek()->id == TokenID::SEMI){ inc(); return node; }
		
		Node* tmp = parseExpr(0);
		if(tmp == nullptr) return node;
		node->push(tmp);
		
		if(peek() == nullptr) return node;
		if(peek()->id == TokenID::COMMA){
			inc();
		}else{
			break;
		}
	}
	
	if(peek()->id == TokenID::SEMI){
		inc();
	}else{
		std::cout<<"Error: Missing Semicolon ';'"<<std::endl;
		printLineError(lines->at(peek()->ln),peek()->ln,peek()->cn);
	}
	
	return node;
}
Node* Parser::parseArgs(){
	if(peek() == nullptr) return nullptr;
	if(peek()->id != TokenID::BRACKET_OPEN) return nullptr;
	
	Node* node = new Node(NodeType::ARGS,NodeID::FUNC);
	node->token = inc();
	
	if(peek()->id == TokenID::BRACKET_CLOSE){
		inc();
		return node;
	}
	
	while(true){
		Node* tmp = parseArg();
		if(tmp == nullptr) return node;
		node->push(tmp);
		if(peek() == nullptr) return node;
		if(peek()->id == TokenID::COMMA){
			inc();
		}else{
			break;
		}
	}
	
	if(peek()->id == TokenID::BRACKET_CLOSE){
		inc();
	}else{
		std::cout<<"Error: Expected Closing Bracket ')'"<<std::endl;
		printLineError(lines->at(peek()->ln),peek()->ln,peek()->cn);
	}
	
	return node;
}
Node* Parser::parseArg(){
	if(peek() == nullptr) return nullptr;
	Node* node = new Node(NodeType::ARG,NodeID::FUNC);
	
	Node* tmp = parseType();
	if(tmp == nullptr) return node;
	node->push(tmp);
	
	tmp = parseIdent(NodeID::ARG);
	if(tmp == nullptr) return node;
	node->push(tmp);
	
	return node;
}
Node* Parser::parseReturnTypes(){
	if(peek() == nullptr) return nullptr;
	if(peek()->id != TokenID::SQUARE_BRACKET_OPEN) return nullptr;
	
	Node* node = new Node(NodeType::RET,NodeID::FUNC);
	node->token = inc();
	
	if(peek()->id == TokenID::SQUARE_BRACKET_CLOSE){
		inc();
		return node;
	}
	
	while(true){
		Node* tmp = parseType();
		if(tmp == nullptr) return node;
		node->push(tmp);
		if(peek() == nullptr) return node;
		if(peek()->id == TokenID::COMMA){
			inc();
		}else{
			break;
		}
	}
	
	if(peek()->id == TokenID::SQUARE_BRACKET_CLOSE){
		inc();
	}else{
		std::cout<<"Error: Expected Square Closing Bracket ']'"<<std::endl;
		printLineError(lines->at(peek()->ln),peek()->ln,peek()->cn);
	}
	
	return node;
}
Node* Parser::parseType(){
	if(peek() == nullptr) return nullptr;
	if(!isType(peek()->id)) return nullptr;
	Node* node = new Node(NodeType::TYPE,TokenIDtoNodeID(peek()->id));
	node->token = inc();
	return node;
}

Node* Parser::parseExpr(int prev){
	if(peek() == nullptr) return nullptr;
	Node* left = nullptr;
	Node* right = nullptr;
	
	if(peek()->type == TokenType::Number){
		left = parseNumber();
	}else if(peek()->type == TokenType::String){
		left = parseString();
	}else if(peek()->type == TokenType::Ident){
		if(peek(1)->id == TokenID::BRACKET_OPEN){
			left = parseCall();
		}else{
			left = parseIdent(NodeID::VARIABLE);
		}
	}else if(priority(peek()->id, 1)!=-1){
		left = parseUnary();
	}else if(peek()->id == TokenID::BRACKET_OPEN){
		inc();
		left = parseExpr(0);
		if(peek() == nullptr || peek()->id != TokenID::BRACKET_CLOSE) {
			return left;
		}
		inc();
	}
	if(left   == nullptr) return nullptr;
	if(peek() == nullptr) return left;
	
	while (true) {
		if (peek() == nullptr) break;
		
		const RefinedToken* tk = peek(); // <-- DON'T consume yet
		
		int pr = priority(tk->id, 0);
		
		if(pr == -1 || pr <= prev) break;
		
		inc();
		
		Node* right = parseExpr(pr);
		
		Node* node = new Node(NodeType::EXPR, TokenIDtoNodeID(tk->id, 0));
		node->push(left);
		node->push(right);
		node->token = tk;
		
		left = node;
	}
	
	return left;
}
Node* Parser::parseUnary(){
	Node* node = new Node(NodeType::EXPR,TokenIDtoNodeID(peek()->id, 1));
	node->token = inc();
	
	if(peek() == nullptr) return nullptr;
	
	if(peek()->type == TokenType::Number){
		node->push(parseNumber());
	}else if(peek()->type == TokenType::Ident){
		node->push(parseIdent(NodeID::VARIABLE));
	}else if(peek()->id == TokenID::BRACKET_OPEN){
		node->push(parseExpr(0));
	}
	
	return node;
}
Node* Parser::parseIdent(NodeID id){
	Node* node = nullptr;
	if(peek()->id == TokenID::IDENT){
		node = new Node(NodeType::IDENT,id);
		node->value.S(*peek()->data.s);
		node->token = inc();
	}else{
		std::cout<<"Error: Expected Identifier"<<std::endl;
		printLineError(lines->at(peek()->ln),peek()->ln,peek()->cn);
	}
	return node;
}
Node* Parser::parseNumber(){
	Node* node = nullptr;
	
	if(peek()->id == TokenID::NUMBER_INT){
		node = new Node(NodeType::NUMBER,NodeID::NUM_INT);
		node->value.I(peek()->data.i);
		node->token = inc();
	}else if(peek()->id == TokenID::NUMBER_FLOAT){
		node = new Node(NodeType::NUMBER,NodeID::NUM_FLOAT);
		node->value.F(peek()->data.f);
		node->token = inc();
	}else{
		std::cout<<"Error: Expected Number"<<std::endl;
		printLineError(lines->at(peek()->ln),peek()->ln,peek()->cn);
	}
	
	return node;
}
Node* Parser::parseString(){
	Node* node = nullptr;
	if(peek()->type == TokenType::String){
		node = new Node(NodeType::STRING,NodeID::NONE);
		node->value.S(*peek()->data.s);
		node->token = inc();
	}else{
		std::cout<<"Error: Expected Identifier"<<std::endl;
		printLineError(lines->at(peek()->ln),peek()->ln,peek()->cn);
	}
	return node;
}




































