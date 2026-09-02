#include "parser.h"


bool Parser::Eof(SyntaxNode** node,std::vector<std::string> expected){
	if(peek()->id != TokenID::eof) return false;
	SyntaxError* err  = new SyntaxError;
	err->kind = SyntaxErrorKind::UnexpectedEOF;
	err->expected = expected;
	err->found = peek();
	(*node) = new SyntaxNode(SyntaxID::Error);
	(*node)->error = err;
	return true;
}
SyntaxError* GenErr(SyntaxErrorKind kind, const Token* found, std::vector<std::string> expected){
	SyntaxError* err  = new SyntaxError;
	err->found = found;
	err->expected = expected;
	err->kind = kind;
	return err;
}

void Parser::parse(){
	index = 0;
	
	SyntaxNode* cur = new SyntaxNode(SyntaxID::Start);
	RootSyntax = cur;
	while(peek()->id!=TokenID::eof){
		cur->A = Global();
		cur->B = new SyntaxNode(SyntaxID::Global);
		cur = cur->B;
	}
}

SyntaxNode* Parser::Global(){
	SyntaxNode* cur = new SyntaxNode(SyntaxID::Global);
	if(peek()->id == TokenID::PRIVATE){
		cur->B = new SyntaxNode(SyntaxID::Private);
		inc();
	}
	
	cur->A = Function();
	if(cur->A==nullptr){
		cur->A = Variable();
	}
	if(cur->A==nullptr){
		cur->A = Using();
	}
	if(cur->A==nullptr){
		cur->A = Overload();
	}
	if(cur->A==nullptr){
		cur->A = Enum();
	}
	if(cur->A==nullptr){
		cur->A = Struct();
	}
	if(cur->A==nullptr){
		cur->A = Namespace();
	}
	if(cur->A==nullptr){
		del(cur);
		cur = new SyntaxNode(SyntaxID::Error);
		cur->error = GenErr(SyntaxErrorKind::Unexpected,inc(),{});
	}
	return cur;
}

SyntaxNode* Parser::Overload(){
	if(peek()->id != TokenID::OPERATOR) return nullptr;
	SyntaxNode* node = new SyntaxNode(SyntaxID::Operator);
	inc();
	
	if(Eof(&(node->A),{"operator"})){ return node; }
	node->A = BinOp(peek());
	if(node->A==nullptr){ node->A = UnaryOp(); }else{ inc(); }
	if(node->A==nullptr){
		node->A = new SyntaxNode(SyntaxID::Error);
		node->A->error = GenErr(SyntaxErrorKind::Expected,peek(),{"operator"});
		return node;
	}
	
	if(Eof(&(node->B),{"[","("})){ return node; }
	node->B = FuncRet();
	if(Eof(&(node->C),{"("})){ return node; }
	node->C = FuncArguments();
	if(node->B==nullptr&&node->C==nullptr){
		node->B = new SyntaxNode(SyntaxID::Error);
		node->B->error = GenErr(SyntaxErrorKind::Expected,peek(),{"[","("});
		return node;
	}
	if(node->C==nullptr){
		node->C = new SyntaxNode(SyntaxID::Error);
		node->C->error = GenErr(SyntaxErrorKind::Expected,peek(),{"("});
		return node;
	}
	
	if(Eof(&(node->D),{"{",";"})){ return node; }
	if(peek()->id == TokenID::SEMI){
		inc();
		return node;
	}
	node->D = Block();
	if(node->D==nullptr){
		node->D = new SyntaxNode(SyntaxID::Error);
		node->D->error = GenErr(SyntaxErrorKind::Expected,peek(),{"{"});
		return node;
	}
	
	return node;
}
SyntaxNode* Parser::Namespace(){
	if(peek()->id != TokenID::NAMESPACE) return nullptr;
	SyntaxNode* node = new SyntaxNode(SyntaxID::Namespace);
	inc();
	
	if(Eof(&(node->A),{"identifier"})){ return node; }
	node->A = Ident();
	if(node->A==nullptr){
		node->A = new SyntaxNode(SyntaxID::Error);
		node->A->error = GenErr(SyntaxErrorKind::Expected,peek(),{"identifier"});
		return node;
	}
	
	if(Eof(&(node->B),{"{"})){ return node; }
	if(peek()->id!=TokenID::OPENCURLYBRACKET){
		node->B = new SyntaxNode(SyntaxID::Error);
		node->B->error = GenErr(SyntaxErrorKind::Expected,peek(),{"{"});
		return node;
	}
	inc();
	
	SyntaxNode* cur = new SyntaxNode(SyntaxID::Global);
	node->B = cur;
	while(peek()->id!=TokenID::eof&&peek()->id!=TokenID::CLOSEDCURLYBRACKET){
		cur->A = Global();
		cur->B = new SyntaxNode(SyntaxID::Global);
		cur = cur->B;
	}
	if(Eof(&(node->C),{"}"})){ return node; }
	if(peek()->id!=TokenID::CLOSEDCURLYBRACKET){
		node->C = new SyntaxNode(SyntaxID::Error);
		node->C->error = GenErr(SyntaxErrorKind::Expected,peek(),{"}"});
		return node;
	}
	inc();
	return node;
}
SyntaxNode* Parser::Using(){
	if(peek()->id != TokenID::USING) return nullptr;
	SyntaxNode* node = new SyntaxNode(SyntaxID::Using);
	inc();
	
	if(Eof(&(node->A),{"identifier"})){ return node; }
	node->A = AccessingChain(0b10000);
	if(node->A==nullptr){
		node->A = new SyntaxNode(SyntaxID::Error);
		node->A->error = GenErr(SyntaxErrorKind::Expected,peek(),{"identifier"});
		return node;
	}
	
	if(Eof(&(node->B),{";"})){ return node; }
	if(peek()->id == TokenID::SEMI){
		inc();
		return node;
	}
	
	node->B = Assign(0);
	
	if(node->B == nullptr){
		node->B = new SyntaxNode(SyntaxID::Error);
		node->B->error = GenErr(SyntaxErrorKind::Expected,peek(),{";"});
		return node;
	}
	
	node->B->A = Type();
	
	if(Eof(&(node->C),{";"})){ return node; }
	if(peek()->id == TokenID::SEMI){
		inc();
		return node;
	}
	
	node->C = new SyntaxNode(SyntaxID::Error);
	node->C->error = GenErr(SyntaxErrorKind::Expected,peek(),{";"});
	return node;
}

SyntaxNode* Parser::MultiRetAssign(){
	auto state = State();
	
	SyntaxNode* node = new SyntaxNode(SyntaxID::MultiRetAssign);
	
	while(1){
		node->A = Type();
		if(node->A==nullptr){ State(state); break; }
		
		node->A->C = Ident();
		if(node->A->C == nullptr){ del(node->A); State(state); break; }
		
		if(peek()->id != TokenID::COMMA){ State(state); del(node); return nullptr; }
		inc();
		
		while(1){
			auto state2 = State();
			
			node->B = Type();
			if(node->B==nullptr){ break; }
			
			node->B->C = Ident();
			if(node->B->C == nullptr){ del(node->B); State(state2); break; }
			
			node->C = Assign();
			
			if(Eof(&(node->B),{";"})){ return node; }
			if(peek()->id != TokenID::SEMI){
				node->B = new SyntaxNode(SyntaxID::Error);
				node->B->error = GenErr(SyntaxErrorKind::Expected,peek(),{";"});
				return node;
			}
			inc();
			
			return node;
		}
		
		node->B = Ident();
		if(node->B == nullptr){ del(node); State(state); return nullptr; }
		
		node->C = Assign();
		
		if(Eof(&(node->B),{";"})){ return node; }
		if(peek()->id != TokenID::SEMI){
			node->B = new SyntaxNode(SyntaxID::Error);
			node->B->error = GenErr(SyntaxErrorKind::Expected,peek(),{";"});
			return node;
		}
		inc();
		
		return node;
	}
	
	node->A = Ident();
	if(node->A==nullptr){ State(state); del(node); return nullptr; }
	
	if(peek()->id != TokenID::COMMA){ State(state); del(node); return nullptr; }
	inc();
	
	while(1){
		auto state2 = State();
		
		node->B = Type();
		if(node->B==nullptr){ break; }
		
		node->B->C = Ident();
		if(node->B->C == nullptr){ del(node->B); State(state2); break; }
		
		node->C = Assign();
		
		if(Eof(&(node->B),{";"})){ return node; }
		if(peek()->id != TokenID::SEMI){
			node->B = new SyntaxNode(SyntaxID::Error);
			node->B->error = GenErr(SyntaxErrorKind::Expected,peek(),{";"});
			return node;
		}
		inc();
		
		return node;
	}
	
	node->B = Ident();
	if(node->B == nullptr){ del(node); State(state); return nullptr; }
	
	node->C = Assign();
	
	if(Eof(&(node->B),{";"})){ return node; }
	if(peek()->id != TokenID::SEMI){
		node->B = new SyntaxNode(SyntaxID::Error);
		node->B->error = GenErr(SyntaxErrorKind::Expected,peek(),{";"});
		return node;
	}
	inc();
	
	return node;
}

SyntaxNode* Parser::AccessingChain(int allow){
	SyntaxNode* node = nullptr;
	if(peek()->id == TokenID::OPENBRACKET && 0b1&allow){
		node = Cast(allow);
	}else if(peek()->id != TokenID::Ident){ return nullptr; }else
	if(peek(1)->id == TokenID::OPENBRACKET && 0b10&allow){
		// function call
		node = Call(allow);
	}else if(peek(1)->id == TokenID::OPENSQUAREBRACKET && 0b100&allow){
		// box indexing
		node = BoxIndex(allow);
	}else if(peek(1)->id == TokenID::DOT && 0b1000&allow){
		// member access
		node = MemberAccess(allow);
	}else if(peek(1)->id == TokenID::ACCESSOR && 0b10000&allow){
		// namespace accessor
		node = Accessor(allow);
	}else{
		node = Ident();
	}
	return node;
}
SyntaxNode* Parser::Call(int allow){
	SyntaxNode* node = new SyntaxNode(SyntaxID::Call);
	node->A = Ident();
	inc(); // (
	
	node->B = CallArg();
	
	if(Eof(&(node->C),{")"})){ return node; }
	if(peek()->id != TokenID::CLOSEDBRACKET){
		node->C = new SyntaxNode(SyntaxID::Error);
		node->C->error = GenErr(SyntaxErrorKind::Expected,peek(),{")"});
		return node;
	}
	inc();
	
	node->C = AccessingChain(allow);
	return node;
}
SyntaxNode* Parser::CallArg(){
	SyntaxNode* node = nullptr;
	SyntaxNode* TMP = Expr();
	if(TMP==nullptr) return nullptr;
	node = new SyntaxNode(SyntaxID::CallArg);
	node->A = TMP;
	if(peek()->id == TokenID::COMMA){
		inc();
		node->B = CallArg();
	}
	return node;
}
SyntaxNode* Parser::Accessor(int allow){
	SyntaxNode* node = new SyntaxNode(SyntaxID::Accessor);
	node->A = Ident();
	inc(); // ::
	
	node->B = AccessingChain(allow);
	if(node->B == nullptr){
		node->B = new SyntaxNode(SyntaxID::Error);
		node->B->error = GenErr(SyntaxErrorKind::Expected,peek(),{"identifier"});
	}
	
	return node;
}
SyntaxNode* Parser::MemberAccess(int allow){
	SyntaxNode* node = new SyntaxNode(SyntaxID::MemberAccess);
	node->A = Ident();
	inc(); // .
	
	node->B = AccessingChain(allow);
	if(node->B == nullptr){
		node->B = new SyntaxNode(SyntaxID::Error);
		node->B->error = GenErr(SyntaxErrorKind::Expected,peek(),{"identifier"});
	}
	
	return node;
}
SyntaxNode* Parser::BoxIndex(int allow){
	SyntaxNode* node = new SyntaxNode(SyntaxID::Indexer);
	node->A = Ident();
	inc(); // [
	
	if(Eof(&(node->B),{"expresion"})){ return node; }
	node->B = Expr();
	if(node->B==nullptr){
		node->C = new SyntaxNode(SyntaxID::Error);
		node->C->error = GenErr(SyntaxErrorKind::Expected,peek(),{"expression"});
		return node;
	}
	
	if(Eof(&(node->C),{"]"})){ return node; }
	if(peek()->id != TokenID::CLOSEDSQUAREBRACKET){
		node->C = new SyntaxNode(SyntaxID::Error);
		node->C->error = GenErr(SyntaxErrorKind::Expected,peek(),{"]"});
		return node;
	}
	inc(); // ]
	
	node->C = AccessingChain(allow);
	return node;
}
SyntaxNode* Parser::Cast(int allow){
	inc();
	SyntaxNode* node = Type();
	
	SyntaxNode* TMP = new SyntaxNode(SyntaxID::Cast);
	TMP->A = node;
	node = TMP;
	
	if(peek()->id != TokenID::CLOSEDBRACKET) {
		return node;
	}
	inc();
	
	node->B = AccessingChain(allow);
	
	return node;
}

SyntaxNode* Parser::Struct(){
	SyntaxNode* TMP = nullptr;
	if(peek()->id == TokenID::PACKED){
		TMP = new SyntaxNode(SyntaxID::Packed);
		inc();
	}else if(peek()->id == TokenID::ALIGN){
		TMP = new SyntaxNode(SyntaxID::Align);
		inc();
		inc(); // (
		TMP->A = Number();
		inc(); // )
	}
	
	if(peek()->id != TokenID::STRUCT){ del(TMP); return nullptr; }
	SyntaxNode* node = new SyntaxNode(SyntaxID::Struct);
	inc();
	node->C = TMP;
	
	if(Eof(&(node->A),{"identifier"})){ return node; }
	node->A = Ident();
	if(node->A==nullptr){
		node->A = new SyntaxNode(SyntaxID::Error);
		node->A->error = GenErr(SyntaxErrorKind::Expected,peek(),{"identifier"});
		return node;
	}
	
	node->B = StructBlock();
	if(Eof(&(node->B),{"{"})){ return node; }
	if(node->B == nullptr){
		node->B = new SyntaxNode(SyntaxID::Error);
		node->B->error = GenErr(SyntaxErrorKind::Expected,peek(),{"{"});
		return node;
	}
	
	return node;
}
SyntaxNode* Parser::StructBlock(){
	if(peek()->id != TokenID::OPENCURLYBRACKET) return nullptr;
	SyntaxNode* node = new SyntaxNode(SyntaxID::StructBlock);
	inc();
	
	node->A = StructStmt();
	
	if(Eof(&(node->B),{"}"})){ return node; }
	if(peek()->id != TokenID::CLOSEDCURLYBRACKET){
		node->B = new SyntaxNode(SyntaxID::Error);
		node->B->error = GenErr(SyntaxErrorKind::Expected,peek(),{"}"});
		return node;
	}
	inc();
	
	return node;
}
SyntaxNode* Parser::StructStmt(){
	SyntaxNode* node = new SyntaxNode(SyntaxID::Stmt);
	node->A = Variable();
	if(node->A!=nullptr){
		node->B = Stmt();
		return node;
	}
	node->A = StructConstructor();
	if(node->A!=nullptr){
		node->B = Stmt();
		return node;
	}
	node->A = StructDestructor();
	if(node->A!=nullptr){
		node->B = Stmt();
		return node;
	}
	del(node);
	return nullptr;
}
SyntaxNode* Parser::StructDestructor(){
	if(peek()->id != TokenID::TILDE) return nullptr;
	SyntaxNode* node = new SyntaxNode(SyntaxID::StructDestructor);
	inc();
	
	if(Eof(&(node->A),{"identifier"})){ return node; }
	node->A = Ident();
	if(node->A==nullptr){
		node->A = new SyntaxNode(SyntaxID::Error);
		node->A->error = GenErr(SyntaxErrorKind::Expected,peek(),{"identifier"});
		return node;
	}
	
	if(Eof(&(node->B),{"("})){ return node; }
	node->B = FuncArguments();
	if(node->B==nullptr){
		node->B = new SyntaxNode(SyntaxID::Error);
		node->B->error = GenErr(SyntaxErrorKind::Expected,peek(),{"("});
		return node;
	}
	
	node->C = Block();
	if(Eof(&(node->C),{"{"})){ return node; }
	if(node->C == nullptr){
		node->C = new SyntaxNode(SyntaxID::Error);
		node->C->error = GenErr(SyntaxErrorKind::Expected,peek(),{"{"});
		return node;
	}
	
	return node;
}
SyntaxNode* Parser::StructConstructor(){
	if(peek()->id != TokenID::Ident) return nullptr;
	SyntaxNode* node = new SyntaxNode(SyntaxID::StructConstructor);
	
	if(Eof(&(node->A),{"identifier"})){ return node; }
	node->A = Ident();
	if(node->A==nullptr){
		node->A = new SyntaxNode(SyntaxID::Error);
		node->A->error = GenErr(SyntaxErrorKind::Expected,peek(),{"identifier"});
		return node;
	}
	
	if(Eof(&(node->B),{"("})){ return node; }
	node->B = FuncArguments();
	if(node->B==nullptr){
		node->B = new SyntaxNode(SyntaxID::Error);
		node->B->error = GenErr(SyntaxErrorKind::Expected,peek(),{"("});
		return node;
	}
	
	node->C = Block();
	if(Eof(&(node->C),{"{"})){ return node; }
	if(node->C == nullptr){
		node->C = new SyntaxNode(SyntaxID::Error);
		node->C->error = GenErr(SyntaxErrorKind::Expected,peek(),{"{"});
		return node;
	}
	
	return node;
}

SyntaxNode* Parser::Enum(){
	if(peek()->id != TokenID::ENUM) return nullptr;
	SyntaxNode* node = new SyntaxNode(SyntaxID::Enum);
	inc();
	
	node->A = Type();
	if(Eof(&(node->B),{"identifier"})){ return node; }
	node->B = Ident();
	if(node->B==nullptr){
		node->B = new SyntaxNode(SyntaxID::Error);
		node->B->error = GenErr(SyntaxErrorKind::Expected,peek(),{"identifier"});
		return node;
	}
	node->C = EnumBlock();
	if(Eof(&(node->C),{"{"})){ return node; }
	if(node->C == nullptr){
		node->C = new SyntaxNode(SyntaxID::Error);
		node->C->error = GenErr(SyntaxErrorKind::Expected,peek(),{"{"});
		return node;
	}
	return node;
}
SyntaxNode* Parser::EnumBlock(){
	if(peek()->id != TokenID::OPENCURLYBRACKET) return nullptr;
	SyntaxNode* node = new SyntaxNode(SyntaxID::EnumBlock);
	inc();
	
	node->A = EnumValue();
	
	if(Eof(&(node->B),{"}"})){ return node; }
	if(peek()->id != TokenID::CLOSEDCURLYBRACKET){
		node->B = new SyntaxNode(SyntaxID::Error);
		node->B->error = GenErr(SyntaxErrorKind::Expected,peek(),{"}"});
		return node;
	}
	inc();
	
	return node;
}
SyntaxNode* Parser::EnumValue(){
	if(peek()->id != TokenID::Ident) return nullptr;
	SyntaxNode* node = new SyntaxNode(SyntaxID::EnumValue);
	node->A = Ident();
	
	if(peek()->id == TokenID::COMMA){
		inc();
		node->B = EnumValue();
	}
	return node;
}

SyntaxNode* Parser::Stmt(){
	SyntaxNode* node = new SyntaxNode(SyntaxID::Stmt);
	node->A = MultiRetAssign();
	if(node->A!=nullptr){
		node->B = Stmt();
		return node;
	}
	node->A = Overload();
	if(node->A!=nullptr){
		node->B = Stmt();
		return node;
	}
	node->A = Defer();
	if(node->A!=nullptr){
		node->B = Stmt();
		return node;
	}
	node->A = Variable();
	if(node->A!=nullptr){
		node->B = Stmt();
		return node;
	}
	node->A = Assignment();
	if(node->A!=nullptr){
		node->B = Stmt();
		return node;
	}
	node->A = If();
	if(node->A!=nullptr){
		node->B = Stmt();
		return node;
	}
	node->A = While();
	if(node->A!=nullptr){
		node->B = Stmt();
		return node;
	}
	node->A = For();
	if(node->A!=nullptr){
		node->B = Stmt();
		return node;
	}
	node->A = Return();
	if(node->A!=nullptr){
		node->B = Stmt();
		return node;
	}
	node->A = Switch();
	if(node->A!=nullptr){
		node->B = Stmt();
		return node;
	}
	node->A = Block();
	if(node->A!=nullptr){
		node->B = Stmt();
		return node;
	}
	del(node);
	return nullptr;
}
SyntaxNode* Parser::Assignment(){
	auto state = State();
	SyntaxNode* tmp = AccessingChain();
	if(tmp==nullptr) return nullptr;
	SyntaxNode* node = new SyntaxNode(SyntaxID::Assignment);
	node->A = tmp;
	node->B = Assign();
	if(node->B == nullptr){ State(state); del(node); return nullptr; }
	if(Eof(&(node->B),{";"})){ return node; }
	if(peek()->id != TokenID::SEMI){
		node->B = new SyntaxNode(SyntaxID::Error);
		node->B->error = GenErr(SyntaxErrorKind::Expected,peek(),{";"});
		return node;
	}
	inc();
	return node;
}
SyntaxNode* Parser::If(){
	if(peek()->id != TokenID::IF) return nullptr;
	SyntaxNode* node = new SyntaxNode(SyntaxID::If);
	inc();
	
	node->A = Condition();
	node->B = Block();
	if(node->B==nullptr){
		node->B = new SyntaxNode(SyntaxID::Error);
		node->B->error = GenErr(SyntaxErrorKind::Expected,peek(),{"{"});
		return node;
	}
	node->C = Else();
	return node;
}
SyntaxNode* Parser::Condition(){
	SyntaxNode* node = new SyntaxNode(SyntaxID::Condition);
	if(Eof(&(node->A),{"("})){ return node; }
	if(peek()->id != TokenID::OPENBRACKET){
		node->A = new SyntaxNode(SyntaxID::Error);
		node->A->error = GenErr(SyntaxErrorKind::Expected,peek(),{"("});
		return node;
	}
	inc();
	
	node->A = Expr();
	
	if(Eof(&(node->B),{")"})){ return node; }
	if(peek()->id != TokenID::CLOSEDBRACKET){
		node->B = new SyntaxNode(SyntaxID::Error);
		node->B->error = GenErr(SyntaxErrorKind::Expected,peek(),{")"});
		return node;
	}
	inc();
	return node;
}
SyntaxNode* Parser::Else(){
	if(peek()->id != TokenID::ELSE) return nullptr;
	SyntaxNode* node = new SyntaxNode(SyntaxID::Else);
	inc();
	
	if(peek()->id == TokenID::OPENBRACKET){
		node->B = Condition();
	}
	node->A = Block();
	if(node->A==nullptr){
		node->A = new SyntaxNode(SyntaxID::Error);
		node->A->error = GenErr(SyntaxErrorKind::Expected,peek(),{"{"});
		return node;
	}
	node->C = Else();
	return node;
}
SyntaxNode* Parser::While(){
	if(peek()->id != TokenID::WHILE) return nullptr;
	SyntaxNode* node = new SyntaxNode(SyntaxID::While);
	inc();
	
	node->A = Condition();
	node->B = Block();
	if(node->B==nullptr){
		node->B = new SyntaxNode(SyntaxID::Error);
		node->B->error = GenErr(SyntaxErrorKind::Expected,peek(),{"{"});
		return node;
	}
	return node;
}
SyntaxNode* Parser::For(){
	if(peek()->id != TokenID::FOR) return nullptr;
	SyntaxNode* node = new SyntaxNode(SyntaxID::For);
	inc();
	
	if(Eof(&(node->B),{"("})){ return node; }
	if(peek()->id != TokenID::OPENBRACKET){
		node->B = new SyntaxNode(SyntaxID::Error);
		node->B->error = GenErr(SyntaxErrorKind::Expected,peek(),{"("});
		return node;
	}
	inc();
	
	node->A = ForInit();
	if(Eof(&(node->B),{";"})){ return node; }
	if(peek()->id != TokenID::SEMI){
		node->B = new SyntaxNode(SyntaxID::Error);
		node->B->error = GenErr(SyntaxErrorKind::Expected,peek(),{";"});
		return node;
	}
	inc();
	
	node->B = Expr();
	
	if(Eof(&(node->C),{";"})){ return node; }
	if(peek()->id != TokenID::SEMI){
		node->C = new SyntaxNode(SyntaxID::Error);
		node->C->error = GenErr(SyntaxErrorKind::Expected,peek(),{";"});
		return node;
	}
	inc();
	
	node->C = ForUpdate();
	
	if(peek()->id != TokenID::CLOSEDBRACKET) return node;
	if(Eof(&(node->B),{")"})){ return node; }
	if(peek()->id != TokenID::CLOSEDBRACKET){
		node->B = new SyntaxNode(SyntaxID::Error);
		node->B->error = GenErr(SyntaxErrorKind::Expected,peek(),{")"});
		return node;
	}
	inc();
	
	node->D = Block();
	if(node->D==nullptr){
		node->D = new SyntaxNode(SyntaxID::Error);
		node->D->error = GenErr(SyntaxErrorKind::Expected,peek(),{"{"});
		return node;
	}
	
	return node;
}
SyntaxNode* Parser::ForInit(){
	SyntaxNode* base = new SyntaxNode(SyntaxID::ForInit);
	
	auto state = State();
	
	while(1){
		SyntaxNode* tmp = Ident();
		if(tmp==nullptr) break;
		SyntaxNode* node = new SyntaxNode(SyntaxID::Assignment);
		node->A = tmp;
		node->B = Assign();
		base->A = node;
		return base;
	}
	
	SyntaxNode* node = new SyntaxNode(SyntaxID::Variable);
	base->A = node;
	SyntaxNode* TMP = Type();
	if(TMP!=nullptr){
		node->A = TMP;
	}else{
		State(state);
		del(node);
		return base;
	}
	
	node->B = Ident();
	if(node->B == nullptr){
		del(node);
		State(state);
		return base;
	}
	
	node->C = Assign();
	
	return base;
}
SyntaxNode* Parser::ForUpdate(){
	SyntaxNode* base = new SyntaxNode(SyntaxID::ForUpdate);
	
	SyntaxNode* tmp = Ident();
	if(tmp==nullptr) return base;
	SyntaxNode* node = new SyntaxNode(SyntaxID::Assignment);
	node->A = tmp;
	node->B = Assign();
	base->A = node;
	if(node->B == nullptr){
		node->B = new SyntaxNode(SyntaxID::Error);
		node->B->error = GenErr(SyntaxErrorKind::Expected,peek(),{"assignment"});
		return base;
	}
	return base;
}
SyntaxNode* Parser::Return(){
	if(peek()->id != TokenID::RETURN) return nullptr;
	SyntaxNode* node = new SyntaxNode(SyntaxID::Return);
	inc();
	
	if(Eof(&(node->A),{"expresion"})){ return node; }
	node->A = Expr();
	if(node->A==nullptr){
		node->A = new SyntaxNode(SyntaxID::Error);
		node->A->error = GenErr(SyntaxErrorKind::Expected,peek(),{"expression"});
		return node;
	}
	if(peek()->id != TokenID::COMMA){
		if(peek()->id != TokenID::SEMI){
			node->B = new SyntaxNode(SyntaxID::Error);
			node->B->error = GenErr(SyntaxErrorKind::Expected,peek(),{";"});
			return node;
		}
		inc();
		return node;
	}
	inc();
	
	if(Eof(&(node->A),{"expresion"})){ return node; }
	node->B = Expr();
	if(node->B==nullptr){
		node->B = new SyntaxNode(SyntaxID::Error);
		node->B->error = GenErr(SyntaxErrorKind::Expected,peek(),{"expression"});
		return node;
	}
	
	if(peek()->id != TokenID::SEMI){
		node->C = new SyntaxNode(SyntaxID::Error);
		node->C->error = GenErr(SyntaxErrorKind::Expected,peek(),{";"});
		return node;
	}
	inc();
	return node;
}
SyntaxNode* Parser::Switch(){
	if(peek()->id != TokenID::SWITCH) return nullptr;
	SyntaxNode* node = new SyntaxNode(SyntaxID::Switch);
	inc();
	
	node->A = Condition();
	
	if(Eof(&(node->B),{"{"})){ return node; }
	if(peek()->id != TokenID::OPENCURLYBRACKET){
		node->B = new SyntaxNode(SyntaxID::Error);
		node->B->error = GenErr(SyntaxErrorKind::Expected,peek(),{"{"});
		return node;
	}
	inc();
	
	node->B = Case();
	
	if(Eof(&(node->C),{"}"})){ return node; }
	if(peek()->id != TokenID::CLOSEDCURLYBRACKET){
		node->C = new SyntaxNode(SyntaxID::Error);
		node->C->error = GenErr(SyntaxErrorKind::Expected,peek(),{"}"});
		return node;
	}
	inc();
	
	node->C = Else();
	
	return node;
}
SyntaxNode* Parser::Case(){
	if(peek()->id != TokenID::OPENBRACKET) return nullptr;
	SyntaxNode* node = new SyntaxNode(SyntaxID::Case);
	inc();
	
	node->A = Pattern();
	
	if(Eof(&(node->B),{")"})){ return node; }
	if(peek()->id != TokenID::CLOSEDBRACKET){
		node->B = new SyntaxNode(SyntaxID::Error);
		node->B->error = GenErr(SyntaxErrorKind::Expected,peek(),{")"});
		return node;
	}
	inc();
	
	node->B = Block();
	if(node->B==nullptr){
		node->B = new SyntaxNode(SyntaxID::Error);
		node->B->error = GenErr(SyntaxErrorKind::Expected,peek(),{"{"});
		return node;
	}
	
	node->C = Case();
	return node;
}
SyntaxNode* Parser::Pattern(){
	SyntaxNode* node = new SyntaxNode(SyntaxID::Pattern);
	node->A = Number();
	if(node->A==nullptr){
		if(peek()->id != TokenID::STAR){
			node->A = new SyntaxNode(SyntaxID::Error);
			node->A->error = GenErr(SyntaxErrorKind::Expected,peek(),{"number","*"});
			return node;
		}
		node->A = new SyntaxNode(SyntaxID::Wild);
		inc();
		return node;
	}
	
	if(peek()->id == TokenID::COMMA){
		inc();
		node->B = Pattern();
		return node;
	}else if(peek()->id == TokenID::TILDE){
		inc();
		node->B = Number();
		if(node->B==nullptr){
			node->B = new SyntaxNode(SyntaxID::Error);
			node->B->error = GenErr(SyntaxErrorKind::Expected,peek(),{"number"});
			return node;
		}
		if(peek()->id == TokenID::COMMA){
			inc();
			node->C = Pattern();
		}
		return node;
	}
	return node;
}
SyntaxNode* Parser::Number(){
	if(peek()->id != TokenID::Number) return nullptr;
	SyntaxNode* node = new SyntaxNode(SyntaxID::Number);
	node->value = peek()->value;
	node->token = inc();
	return node;
}
SyntaxNode* Parser::Defer(){
	if(peek()->id != TokenID::DEFER) return nullptr;
	SyntaxNode* node = new SyntaxNode(SyntaxID::Defer);
	inc();
	
	node->A = AccessingChain();
	
	if(Eof(&(node->B),{";"})){ return node; }
	if(peek()->id != TokenID::SEMI){
		node->B = new SyntaxNode(SyntaxID::Error);
		node->B->error = GenErr(SyntaxErrorKind::Expected,peek(),{";"});
		return node;
	}
	inc();
	
	return node;
}

SyntaxNode* Parser::Function(){
	if(peek()->id != TokenID::FUNC) return nullptr;
	inc();
	
	SyntaxNode* node = new SyntaxNode(SyntaxID::Function);
	
	if(Eof(&(node->A),{"identifier"})){ return node; }
	node->A = FuncName();
	if(node->A==nullptr){
		node->A = new SyntaxNode(SyntaxID::Error);
		node->A->error = GenErr(SyntaxErrorKind::Expected,peek(),{"identifier"});
		return node;
	}
	
	if(Eof(&(node->B),{"[","("})){ return node; }
	node->B = FuncRet();
	
	if(Eof(&(node->C),{"("})){ return node; }
	node->C = FuncArguments();
	if(node->B==nullptr&&node->C==nullptr){
		node->B = new SyntaxNode(SyntaxID::Error);
		node->B->error = GenErr(SyntaxErrorKind::Expected,peek(),{"[","("});
		return node;
	}
	if(node->C==nullptr){
		node->C = new SyntaxNode(SyntaxID::Error);
		node->C->error = GenErr(SyntaxErrorKind::Expected,peek(),{"("});
		return node;
	}
	
	if(Eof(&(node->D),{"{",";"})){ return node; }
	if(peek()->id == TokenID::SEMI){
		inc();
		return node;
	}
	node->D = Block();
	if(node->D==nullptr){
		node->D = new SyntaxNode(SyntaxID::Error);
		node->D->error = GenErr(SyntaxErrorKind::Expected,peek(),{"{"});
		return node;
	}
	
	return node;
}
SyntaxNode* Parser::FuncRet(){
	if(peek()->id != TokenID::OPENSQUAREBRACKET) return nullptr;
	inc();
	
	SyntaxNode* node = new SyntaxNode(SyntaxID::FuncRet);
	
	if(Eof(&(node->A),{"]"})){ return node; }
	if(peek()->id == TokenID::CLOSEDSQUAREBRACKET){ inc(); return node; }
	
	if(peek()->id == TokenID::COLON){
		inc();
		node->A = new SyntaxNode(SyntaxID::ErrRet);
		node->A->A = RetType();
	}else{
		node->A = RetType();
		if(peek()->id == TokenID::COLON){
			inc();
			node->B = new SyntaxNode(SyntaxID::ErrRet);
			node->B->A = RetType();
		}
	}
	
	if(Eof(&(node->C),{"]"})){ return node; }
	if(peek()->id != TokenID::CLOSEDSQUAREBRACKET){
		node->C = new SyntaxNode(SyntaxID::Error);
		node->C->error = GenErr(SyntaxErrorKind::Expected,peek(),{"]"});
		return node;
	}
	inc();
	return node;
}
SyntaxNode* Parser::RetType(){
	if(peek()->id!=TokenID::EXCLAMATION){ return Type(); }
	
	const Token* tk = inc();
	SyntaxNode* node = Type();
	if(node==nullptr){
		node = new SyntaxNode(SyntaxID::Error);
		node->error = GenErr(SyntaxErrorKind::Expected,peek(),{"TYPE"});
		return node;
	}
	node->B = new SyntaxNode(SyntaxID::Discardable);
	node->B->token = tk;
	
	return node;
}
SyntaxNode* Parser::FuncArguments(){
	if(peek()->id != TokenID::OPENBRACKET) return nullptr;
	inc();
	
	SyntaxNode* node = new SyntaxNode(SyntaxID::FuncArg);
	node->A = FuncArg();
	
	if(Eof(&(node->B),{")"})){ return node; }
	if(peek()->id != TokenID::CLOSEDBRACKET){
		node->B = new SyntaxNode(SyntaxID::Error);
		node->B->error = GenErr(SyntaxErrorKind::Expected,peek(),{")"});
		return node;
	}
	inc();
	return node;
}
SyntaxNode* Parser::FuncArg(){
	SyntaxNode* node = new SyntaxNode(SyntaxID::FuncArg);
	node->A = Type();
	if(node->A==nullptr) return nullptr;
	
	if(Eof(&(node->C),{"identifier"})){ return node; }
	node->B = Ident();
	if(node->B==nullptr){
		node->C = new SyntaxNode(SyntaxID::Error);
		node->C->error = GenErr(SyntaxErrorKind::Expected,peek(),{"identifier"});
		return node;
	}
	
	if(peek()->id == TokenID::COMMA){
		inc();
		node->C = FuncArg();
		return node;
	}
	
	node->C = Assign();
	if(node->C==nullptr) return node;
	
	if(peek()->id == TokenID::COMMA){
		inc();
		node->C = FuncArg();
		return node;
	}
	
	return node;
}
SyntaxNode* Parser::Ident(){
	if(peek()->id != TokenID::Ident) return nullptr;
	SyntaxNode* node = new SyntaxNode(SyntaxID::Ident);
	node->value = peek()->value;
	node->token = inc();
	return node;
}
SyntaxNode* Parser::Block(){
	if(peek()->id != TokenID::OPENCURLYBRACKET) return nullptr;
	inc();
	
	SyntaxNode* node = new SyntaxNode(SyntaxID::Block);
	node->A = Stmt();
	
	if(Eof(&(node->B),{"}"})){ return node; }
	if(peek()->id != TokenID::CLOSEDCURLYBRACKET){
		node->B = new SyntaxNode(SyntaxID::Error);
		node->B->error = GenErr(SyntaxErrorKind::Expected,peek(),{"}"});
		return node;
	}
	inc();
	return node;
}
SyntaxNode* Parser::FuncName(){
	SyntaxNode* node = nullptr;
	node = Type();
	if(peek()->id != TokenID::DOT){
		return node;
	}
	inc();
	SyntaxNode* TMP = node;
	node = new SyntaxNode(SyntaxID::MemberAccess);
	node->A = TMP;
	node->B = Ident();
	return node;
}

SyntaxNode* Parser::Variable(){
	auto state = State();
	
	SyntaxNode* node = new SyntaxNode(SyntaxID::Variable);
	SyntaxNode* TMP = Type();
	if(TMP!=nullptr){
		node->A = TMP;
	}else{
		State(state);
		del(node);
		return nullptr;
	}
	
	node->B = Ident();
	if(node->B == nullptr){
		del(node);
		State(state);
		return nullptr;
	}
	
	if(Eof(&(node->C),{";"})){ return node; }
	if(peek()->id == TokenID::SEMI){
		inc();
		return node;
	}
	
	node->C = Assign();
	
	if(node->C == nullptr){
		node->C = new SyntaxNode(SyntaxID::Error);
		node->C->error = GenErr(SyntaxErrorKind::Expected,peek(),{";"});
		return node;
	}
	
	if(Eof(&(node->D),{";"})){ return node; }
	if(peek()->id == TokenID::SEMI){
		inc();
		return node;
	}
	
	node->D = new SyntaxNode(SyntaxID::Error);
	node->D->error = GenErr(SyntaxErrorKind::Expected,peek(),{";"});
	return node;
}
SyntaxNode* Parser::RawType(){
	SyntaxID id = SyntaxID::Error;
	
	switch(peek()->id){
		case TokenID::INT: id = SyntaxID::Int; break;
		case TokenID::UINT:id = SyntaxID::Uint;break;
		case TokenID::U8:  id = SyntaxID::U8;  break;
		case TokenID::U16: id = SyntaxID::U16; break;
		case TokenID::U32: id = SyntaxID::U32; break;
		case TokenID::U64: id = SyntaxID::U64; break;
		case TokenID::I8:  id = SyntaxID::I8;  break;
		case TokenID::I16: id = SyntaxID::I16; break;
		case TokenID::I32: id = SyntaxID::I32; break;
		case TokenID::I64: id = SyntaxID::I64; break;
		case TokenID::F32: id = SyntaxID::F32; break;
		case TokenID::F64: id = SyntaxID::F64; break;
		case TokenID::FLOAT:  id = SyntaxID::Float;  break;
		case TokenID::DOUBLE: id = SyntaxID::Double; break;
		case TokenID::CHAR: id = SyntaxID::Char; break;
		case TokenID::VOID: id = SyntaxID::Void; break;
	};
	
	if(id != SyntaxID::Error){
		SyntaxNode* node = new SyntaxNode(id);
		node->token = inc();
		return node;
	}
	
	return nullptr;
}
SyntaxNode* Parser::TypeModifier(){
	SyntaxNode* node = nullptr;
	SyntaxNode* TMP = nullptr;
	if(peek()->id == TokenID::CONST){
		node = new SyntaxNode(SyntaxID::Const);
		node->token = inc();
	}
	if(peek()->id == TokenID::STAR){
		node = new SyntaxNode(SyntaxID::Pointer);
		node->token = inc();
	}
	if(peek()->id == TokenID::OPENSQUAREBRACKET){
		inc();
		TMP = Expr();
		if(TMP==nullptr) return nullptr;
		if(peek()->id != TokenID::CLOSEDSQUAREBRACKET){
			return nullptr;
		}
		inc();
		node = new SyntaxNode(SyntaxID::Box);
		node->B = TMP;
	}
	
	if(node==nullptr) return nullptr;
	TMP = TypeModifier();
	if(TMP==nullptr) return node;
	node->A = TMP;
	return node;
}
SyntaxNode* Parser::Type(){
	auto state = State();
	SyntaxNode* TMP_B = TypeModifier();
	SyntaxNode* TMP_A = RawType();
	
	if(TMP_A != nullptr && TMP_B == nullptr){
		SyntaxNode* node = new SyntaxNode(SyntaxID::Type);
		node->A = TMP_A;
		return node;
	}
	if(TMP_A != nullptr && TMP_B != nullptr){
		SyntaxNode* node = new SyntaxNode(SyntaxID::Type);
		node->A = TMP_A;
		node->B = TMP_B;
		return node;
	}
	TMP_A = AccessingChain(0b10000);
	if(TMP_A!=nullptr){
		SyntaxNode* node = new SyntaxNode(SyntaxID::Type);
		node->A = TMP_A;
		node->B = TMP_B;
		return node;
	}
	State(state);
	del(TMP_B);
	return nullptr;
}
SyntaxNode* Parser::Assign(int allow){
	SyntaxID id = SyntaxID::Error;
	switch(peek()->id){
		case TokenID::ADDASSIGN: id = SyntaxID::AddAssign; break;
		case TokenID::SUBASSIGN: id = SyntaxID::SubAssign; break;
		case TokenID::MULASSIGN: id = SyntaxID::MulAssign; break;
		case TokenID::DIVASSIGN: id = SyntaxID::DivAssign; break;
		case TokenID::MODASSIGN: id = SyntaxID::ModAssign; break;
		case TokenID::ASSIGN:    id = SyntaxID::Assign;    break;
		case TokenID::ANDASSIGN: id = SyntaxID::AndAssign; break;
		case TokenID::ORASSIGN:  id = SyntaxID::OrAssign;  break;
		case TokenID::XORASSIGN: id = SyntaxID::XorAssign; break;
		case TokenID::SPECIALASSIGN: id = SyntaxID::SpecialAssign; break;
	}
	if(id==SyntaxID::Error) return nullptr;
	SyntaxNode* node = new SyntaxNode(id);
	node->token = inc();
	if(allow==0) return node;
	if(Eof(&(node->A),{"expresion"})){ return node; }
	node->A = Expr();
	if(node->A==nullptr){
		node->A = new SyntaxNode(SyntaxID::Error);
		node->A->error = GenErr(SyntaxErrorKind::Expected,peek(),{"expression"});
		return node;
	}
	return node;
}

SyntaxNode* Parser::Expr(int p){
	SyntaxNode* left = Literal();
	if(left==nullptr){
		if(peek()->id == TokenID::OPENBRACKET){
			inc();
			left = Type();
			if(left==nullptr){
				left = Expr(0);
			}else{
				SyntaxNode* TMP = new SyntaxNode(SyntaxID::Cast);
				TMP->A = left;
				left = TMP;
				
				if(peek() == nullptr || peek()->id != TokenID::CLOSEDBRACKET) {
					return left;
				}
				inc();
				
				TMP->B = Expr(12);
				
				return left;
			}
			if(peek() == nullptr || peek()->id != TokenID::CLOSEDBRACKET) {
				return left;
			}
			inc();
		}else if(checkPriority(peek()->id, 1)!=-1){
			left = UnaryOp();
		}
	}
	if(left==nullptr) return nullptr;
	if(peek()==nullptr) return nullptr;
	
	while(true){
		if(peek()==nullptr) break;
		
		const Token* tk = peek();
		int priority = checkPriority(tk->id,0);
		if(priority == -1 || priority <= p) break;
		
		inc();
		
		SyntaxNode* right = Expr(priority);
		
		SyntaxNode* node = BinOp(tk);
		node->A = left;
		node->B = right;
		
		left = node;
	}
	
	return left;
}
SyntaxNode* Parser::Literal(){
	SyntaxNode* node = new SyntaxNode(SyntaxID::Literal);
	if(peek()->id == TokenID::Number){
		node->A = new SyntaxNode(SyntaxID::Number);
		node->A->value = peek()->value;
		node->A->token = inc();
		return node;
	}
	if(peek()->id == TokenID::Ident){
		node->A = AccessingChain();
		return node;
	}
	if(peek()->id == TokenID::String){
		node->A = new SyntaxNode(SyntaxID::String);
		node->A->value = peek()->value;
		node->A->token = inc();
		return node;
	}
	if(peek()->id == TokenID::SELF){
		node->A = new SyntaxNode(SyntaxID::Self);
		inc();
		return node;
	}
	del(node);
	return nullptr;
}
SyntaxNode* Parser::UnaryOp(){
	SyntaxNode* node = nullptr;
	switch(peek()->id){
		case TokenID::BILL:        node = new SyntaxNode(SyntaxID::PointerMath); break;
		case TokenID::STAR:        node = new SyntaxNode(SyntaxID::Dereference); break;
		case TokenID::EXCLAMATION: node = new SyntaxNode(SyntaxID::BoolNot);     break;
		case TokenID::TILDE:       node = new SyntaxNode(SyntaxID::BitwiseNot);  break;
		case TokenID::PLUS:        node = new SyntaxNode(SyntaxID::Positive);    break;
		case TokenID::MINUS:       node = new SyntaxNode(SyntaxID::Negitive);    break;
	};
	
	if(node==nullptr) return nullptr;
	
	node->token = inc();
	node->A = Expr(checkPriority(node->token->id,1));
	return node;
}
SyntaxNode* Parser::BinOp(const Token* tk){
	SyntaxNode* node = new SyntaxNode(SyntaxID::Error);
	node->token = tk;
	
	switch(tk->id){
		case TokenID::GREATEREQUAL: node->id = SyntaxID::GreaterEqual; break;
		case TokenID::LESSEQUAL:    node->id = SyntaxID::LessEqual;    break;
		case TokenID::NOTEQUAL:     node->id = SyntaxID::NotEqual;     break;
		case TokenID::STAR:    node->id = SyntaxID::Multiply;  break;
		case TokenID::PLUS:    node->id = SyntaxID::Add;       break;
		case TokenID::MINUS:   node->id = SyntaxID::Subtract;  break;
		case TokenID::PERCENT: node->id = SyntaxID::Modulos;   break;
		case TokenID::SLASH:   node->id = SyntaxID::Divide;    break;
		case TokenID::LSHIFT:  node->id = SyntaxID::LeftShift; break;
		case TokenID::RSHIFT:  node->id = SyntaxID::RightShift; break;
		case TokenID::EQUAL:   node->id = SyntaxID::Equal;     break;
		case TokenID::OR:      node->id = SyntaxID::BoolOr;    break;
		case TokenID::AND:     node->id = SyntaxID::BoolAnd;   break;
		case TokenID::TILDE:   node->id = SyntaxID::BitNot;    break;
		case TokenID::BITAND:  node->id = SyntaxID::BitAnd;    break;
		case TokenID::BITOR:   node->id = SyntaxID::BitOr;     break;
		case TokenID::BITXOR:  node->id = SyntaxID::BitXor;    break;
		case TokenID::LESS:    node->id = SyntaxID::Less;      break;
		case TokenID::GREATER: node->id = SyntaxID::Greater;   break;
	};
	
	if(node->id==SyntaxID::Error){
		node->error = GenErr(SyntaxErrorKind::Expected,inc(),{"operator"});
	}
	
	return node;
}

int Parser::State(int v){
	if(v!=-1){ index=v; return 0; }
	return index;
}
void Parser::del(SyntaxNode* node){
	if(node==nullptr) return;
	del(node->A);
	del(node->B);
	del(node->C);
	del(node->D);
	del(node->E);
	del(node->F);
	delete node;
}
const Token* Parser::peek(int offset) const{
	if(tokens.size() > index+offset){
		return &tokens.at(index+offset);
	}
	return nullptr;
}
const Token* Parser::inc(){
	const Token* tok = peek();
	index++;
	return tok;
}
