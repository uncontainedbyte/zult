#include "parser.h"






void Parser::parse(){
	index = 0;
	
	RootSyntax = Expr();
}
SyntaxNode* Parser::Expr(int p){
	// Expr -> Literal | UnaryOp & Literal | Literal & BinOp & Expr
	
	if(peek() == nullptr) return nullptr;
	
	SyntaxNode* left = Literal();
	if(left==nullptr){
		if(peek()->id == TokenID::OPENBRACKET){
			inc();
			left = Expr(0);
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
	// Literal -> Number | Ident | String
	
	if(peek() == nullptr) return nullptr;
	
	SyntaxNode* node = new SyntaxNode(SyntaxID::Literal);
	if(peek()->id == TokenID::Number){
		node->A = new SyntaxNode(SyntaxID::Number);
		node->A->value = peek()->value;
		node->A->token = inc();
		return node;
	}
	if(peek()->id == TokenID::Ident){
		node->A = new SyntaxNode(SyntaxID::Ident);
		node->A->value = peek()->value;
		node->A->token = inc();
		return node;
	}
	if(peek()->id == TokenID::String){
		node->A = new SyntaxNode(SyntaxID::String);
		node->A->value = peek()->value;
		node->A->token = inc();
		return node;
	}
	
	return nullptr;
}
SyntaxNode* Parser::UnaryOp(){
	//UnaryOp -> STAR | EXCLAMATION | TILDE | PLUS | MINUS
	if(peek() == nullptr) return nullptr;
	
	if(peek()->id == TokenID::STAR){
		SyntaxNode* node = new SyntaxNode(SyntaxID::Dereference);
		node->token = inc();
		node->A = Expr(checkPriority(node->token->id,1));
		return node;
	}
	if(peek()->id == TokenID::EXCLAMATION){
		SyntaxNode* node = new SyntaxNode(SyntaxID::BoolNot);
		node->token = inc();
		node->A = Expr(checkPriority(node->token->id,1));
		return node;
	}
	if(peek()->id == TokenID::TILDE){
		SyntaxNode* node = new SyntaxNode(SyntaxID::BitwiseNot);
		node->token = inc();
		node->A = Expr(checkPriority(node->token->id,1));
		return node;
	}
	if(peek()->id == TokenID::PLUS){
		SyntaxNode* node = new SyntaxNode(SyntaxID::Positive);
		node->token = inc();
		node->A = Expr(checkPriority(node->token->id,1));
		return node;
	}
	if(peek()->id == TokenID::MINUS){
		SyntaxNode* node = new SyntaxNode(SyntaxID::Negitive);
		node->token = inc();
		node->A = Expr(checkPriority(node->token->id,1));
		return node;
	}
	return nullptr;
}
SyntaxNode* Parser::BinOp(const Token* tk){
	// BinOp -> STAR | PLUS | MINUS | PERCENT | SLASH | LSHIFT | RSHIFT | LESSEQUAL | GREATEREQUAL | EQUAL |
	//          NOTEQUAL | OR | AND | ACCESSOR | BILL | TILDE | BITAND | BITOR | BITXOR | LESS | GREATER | DOT
	
	SyntaxNode* node = new SyntaxNode(SyntaxID::Expr);
	node->token = tk;
	
	switch(tk->id){
		case TokenID::GREATEREQUAL: node->id = SyntaxID::GreaterEqual; break;
		case TokenID::LESSEQUAL:    node->id = SyntaxID::LessEqual;    break;
		case TokenID::ACCESSOR:     node->id = SyntaxID::Accessor;     break;
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
		case TokenID::BILL:    node->id = SyntaxID::PointerMath; break;
		case TokenID::TILDE:   node->id = SyntaxID::BitNot;    break;
		case TokenID::BITAND:  node->id = SyntaxID::BitAnd;    break;
		case TokenID::BITOR:   node->id = SyntaxID::BitOr;     break;
		case TokenID::BITXOR:  node->id = SyntaxID::BitXor;    break;
		case TokenID::LESS:    node->id = SyntaxID::Less;      break;
		case TokenID::GREATER: node->id = SyntaxID::Greater;   break;
		case TokenID::DOT:     node->id = SyntaxID::MemberAccess; break;
	};
	
	return node;
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
