#pragma once
#include <string>
#include <vector>
#include "chunk.h"
#include <map>
#include <functional>
#include "common.h"
#include "value.h"
#include "objects.h"


typedef enum {
	// Single-character tokens.
	TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,
	TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,
	TOKEN_COMMA, TOKEN_DOT, TOKEN_MINUS, TOKEN_PLUS,
	TOKEN_SEMICOLON, TOKEN_SLASH, TOKEN_STAR,
	// One or two character tokens.
	TOKEN_BANG, TOKEN_BANG_EQUAL,
	TOKEN_EQUAL, TOKEN_EQUAL_EQUAL,
	TOKEN_GREATER, TOKEN_GREATER_EQUAL,
	TOKEN_LESS, TOKEN_LESS_EQUAL,
	// Literals.
	TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_NUMBER,
	// Keywords.
	TOKEN_AND, TOKEN_CLASS, TOKEN_ELSE, TOKEN_FALSE,
	TOKEN_FOR, TOKEN_FUN, TOKEN_IF, TOKEN_NIL, TOKEN_OR,
	TOKEN_PRINT, TOKEN_RETURN, TOKEN_SUPER, TOKEN_THIS,
	TOKEN_TRUE, TOKEN_VAR, TOKEN_WHILE,

	TOKEN_ERROR, TOKEN_EOF, TOKEN_NONE,
} TokenType;

typedef enum {
	PREC_NONE,
	PREC_ASSIGNMENT,  // =
	PREC_OR,          // or
	PREC_AND,         // and
	PREC_EQUALITY,    // == !=
	PREC_COMPARISON,  // < > <= >=
	PREC_TERM,        // + -
	PREC_FACTOR,      // * /
	PREC_UNARY,       // ! -
	PREC_CALL,        // . ()
	PREC_PRIMARY
} Precedence;

typedef struct {
	std::function<void()> prefix;
	std::function<void()> infix;
	Precedence precedence;
} ParseRule;


class Token {
public:
	TokenType type;
	const char* start;
	int length;
	int line;
	Token(TokenType type, const char* start, int length, int line) {
		this->type = type;
		this->start = start;
		this->length = length;
		this->line = line;
	}
	Token() {
		this->type = TOKEN_NONE;
		this->start = " ";
		this->length = 0;
		this->line = 0;
	}
};

class Local {
public:
	Token name;
	int depth;

	Local(Token name,int depth) {
		this->name = name;
		this->depth = depth;
	}
};



class Scanner {
public:
	const char* start;
	const char* current;
	int line;
	bool isAtEnd() {
		return *current == '\0';
	}

	bool isDigit(char c) {
		if (c >= '0' && c <= '9') {
			return 1;
		}
		return 0;
	}

	bool isAlpha(char c) {
		return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_');
	}

	char advance() {
		current++;
		return current[-1];
	}

	bool match(char expected) {
		if (isAtEnd()) return false;
		if (*current != expected) return false;
		current++;
		return true;
	}

	Token scanToken() {
		start = current;
		// Handle for whitespaces 
		if (isAtEnd()) return makeToken(TOKEN_EOF);
		if (*current == ' ' || *current == '\r' || *current == '\t') {
			advance();
			start = current;
		}
		if (*current == '\n') {
			line++;
			advance();
		}

		// Handle for comments
		if (*current == '/' && *current + 1 == '/') {
			while (*current != '\n' && !isAtEnd()) advance();
		}

		char c = advance();
		if (isDigit(c)) return number();
		if (isAlpha(c)) return identifier();
		switch (c) {
		case '(': return makeToken(TOKEN_LEFT_PAREN);
		case ')': return makeToken(TOKEN_RIGHT_PAREN);
		case '{': return makeToken(TOKEN_LEFT_BRACE);
		case '}': return makeToken(TOKEN_RIGHT_BRACE);
		case ';': return makeToken(TOKEN_SEMICOLON);
		case ',': return makeToken(TOKEN_COMMA);
		case '.': return makeToken(TOKEN_DOT);
		case '-': return makeToken(TOKEN_MINUS);
		case '+': return makeToken(TOKEN_PLUS);
		case '/': return makeToken(TOKEN_SLASH);
		case '*': return makeToken(TOKEN_STAR);

		case '!':
			return makeToken(
				match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
		case '=':
			return makeToken(
				match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
		case '<':
			return makeToken(
				match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
		case '>':
			return makeToken(
				match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);

		case '"':
			return String();

		}
		return errorToken("Unexpected character.");
	}

	Token makeToken(TokenType type) {
		Token token = Token(type, start, (int)(current - start),line);
		//std::cout << token.type << "\n";
		return token;
	}

	Token errorToken(const char* message) {
		Token token = Token(TOKEN_ERROR, start, (int)(current - start),line);
		return token;
	}

	Token String() {
		while (*current != '"' && !isAtEnd()) {
			if (*current == '\n') line++;
			advance();
		}
		if (isAtEnd()) {
			return errorToken("Unterminated String");
		}
		advance();
		return makeToken(TOKEN_STRING);
	}

	Token number() {
		while (isDigit(*current) && !isAtEnd()) {
			advance();
		}
		if (*current == '.') {
			advance();
			while (isDigit(*current)) {
				advance();
			}
		}
		return makeToken(TOKEN_NUMBER);
	}

	Token identifier() {
		while ((isAlpha(*current) || isDigit(*current)) && !isAtEnd()) {
			advance();
		}
		return makeToken(TokenTypeIdentifier());
	}

	TokenType TokenTypeIdentifier() {

		switch (*start) {
		case 'a': return checkKeyword(1, 2, "nd", TOKEN_AND);
		case 'c': return checkKeyword(1, 4, "lass", TOKEN_CLASS);
		case 'e': return checkKeyword(1, 3, "lse", TOKEN_ELSE);
		case 'f': {
			if (current - start > 1) {
				switch (start[1]) {
					std::cout << start[1] << "\n";
				case 'a': return checkKeyword(2, 3, "lse", TOKEN_FALSE);
				case 'u': return checkKeyword(2, 1, "n", TOKEN_FUN);
				case 'o': return checkKeyword(2, 1, "r", TOKEN_FOR);
				}

			}
			break;
		}
		case 'i': return checkKeyword(1, 1, "f", TOKEN_IF);
		case 'n': return checkKeyword(1, 2, "il", TOKEN_NIL);
		case 'o': return checkKeyword(1, 1, "r", TOKEN_OR);
		case 'p': return checkKeyword(1, 4, "rint", TOKEN_PRINT);
		case 'r': return checkKeyword(1, 5, "eturn", TOKEN_RETURN);
		case 's': return checkKeyword(1, 4, "uper", TOKEN_SUPER);
		case 't': {
			if (current - start > 1) {
				switch (start[1]) {
				case 'h': return checkKeyword(2, 2, "is", TOKEN_THIS);
				case 'r': return checkKeyword(2, 2, "ue", TOKEN_TRUE);
				}
			}
			break;
		}
		case 'v': return checkKeyword(1, 2, "ar", TOKEN_VAR);
		case 'w': return checkKeyword(1, 4, "hile", TOKEN_WHILE);

		}
		return TOKEN_IDENTIFIER;
	}

	TokenType checkKeyword(int startC, int length, const char* check_str, TokenType checkToken) {
		//std::cout << "CHECK KEYWORD CALLED WITH " << check_str << "\n";
		if (current - start == startC + length &&
			memcmp(start + startC, check_str, length) == 0) {
			return checkToken;
		}

		return TOKEN_IDENTIFIER;
	}
};

class Parser {
public:
	Token current;
	Token previous; 
	Scanner *scanner;
	bool had_error = 0;
	const char* source;

	Parser(const char* src, Scanner* scanner) : source(src), current(TOKEN_NONE, 0, 0, 0), previous(TOKEN_NONE, 0, 0, 0) {
		this->scanner = scanner;
	}

	void advance() {
		previous = current;
		current = scanner->scanToken();
		//std::cout << previous.type <<" " << current.type<<"\n";
	}

	void consume(TokenType type, const char* message) {

		if (current.type == type) {
			advance();
			return;
		}
		else {
			std::cout << "Current token type is " << current.type << " Expected type is " << type << "\n";
			errorAtCurrent(message);
		}
	}

	void errorAt(Token* token, const char* message) {
		std::cerr << "Error at " << token->line << "\n";
		std::cerr << message << "\n";
		had_error = 1;
	}

	void errorAtCurrent(const char* message) {
		errorAt(&current, message);
	}

	void error(const char* message) {
		errorAt(&previous, message);
	}
	
};

class Compiler {
public:

	const char* source;
	Chunk* compiling_chunk;
	Scanner scanner;
	Parser parser;
	bool had_error = 0;
	std::map<TokenType, ParseRule> parser_rules_map;
	std::vector<std::unique_ptr<Local>>locals;
	int localCount;
	int scopeDepth;


	Compiler(const char* source, Chunk* chunk) :parser(source, &scanner) {
		this->source = source;
		this->compiling_chunk = chunk;
		this->scanner.start = source;
		this->scanner.current = source;
		this->scanner.line = 0;
		this->localCount = 0;
		this->scopeDepth = 0;
		parser_rules_map[TOKEN_LEFT_PAREN] = { std::bind(&Compiler::grouping, this), NULL, PREC_NONE };
		parser_rules_map[TOKEN_RIGHT_PAREN] = { NULL,NULL,PREC_NONE };
		parser_rules_map[TOKEN_LEFT_BRACE] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_RIGHT_BRACE] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_COMMA] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_DOT] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_MINUS] = { std::bind(&Compiler::unary,this), std::bind(&Compiler::binary,this), PREC_TERM };
		parser_rules_map[TOKEN_PLUS] = { NULL, std::bind(&Compiler::binary,this), PREC_TERM };
		parser_rules_map[TOKEN_SEMICOLON] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_SLASH] = { NULL, std::bind(&Compiler::binary,this), PREC_FACTOR };
		parser_rules_map[TOKEN_STAR] = { NULL, std::bind(&Compiler::binary,this), PREC_FACTOR };
		parser_rules_map[TOKEN_BANG] = { std::bind(&Compiler::unary,this),     NULL,   PREC_NONE};
		parser_rules_map[TOKEN_BANG_EQUAL] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_EQUAL] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_EQUAL_EQUAL] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_GREATER] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_GREATER_EQUAL] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_LESS] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_LESS_EQUAL] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_IDENTIFIER] = { std::bind(&Compiler::variable,this),     NULL,   PREC_NONE};
		parser_rules_map[TOKEN_STRING] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_NUMBER] = { std::bind(&Compiler::number,this),   NULL,   PREC_NONE };
		parser_rules_map[TOKEN_AND] = { NULL,std::bind(&Compiler::and_,this),PREC_AND};
		parser_rules_map[TOKEN_CLASS] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_ELSE] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_FALSE] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_FOR] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_FUN] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_IF] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_NIL] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_OR] = { NULL,     std::bind(&Compiler::or_,this),    PREC_OR };
		parser_rules_map[TOKEN_PRINT] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_RETURN] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_SUPER] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_THIS] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_TRUE] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_VAR] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_WHILE] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_ERROR] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_EOF] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_TRUE] = { std::bind(&Compiler::literal,this),  NULL,   PREC_NONE };
		parser_rules_map[TOKEN_FALSE] = { std::bind(&Compiler::literal,this),  NULL,   PREC_NONE };
		parser_rules_map[TOKEN_NIL] = { std::bind(&Compiler::literal,this),  NULL,   PREC_NONE };
		parser_rules_map[TOKEN_BANG_EQUAL] = { NULL, std::bind(&Compiler::binary,this), PREC_EQUALITY };
		parser_rules_map[TOKEN_EQUAL_EQUAL] = { NULL,      std::bind(&Compiler::binary,this), PREC_EQUALITY };
		parser_rules_map[TOKEN_GREATER] = { NULL,     std::bind(&Compiler::binary,this), PREC_COMPARISON };
		parser_rules_map[TOKEN_GREATER_EQUAL] = { NULL,      std::bind(&Compiler::binary,this), PREC_COMPARISON };
		parser_rules_map[TOKEN_LESS] = { NULL, std::bind(&Compiler::binary,this), PREC_COMPARISON };
		parser_rules_map[TOKEN_LESS_EQUAL] = { NULL, std::bind(&Compiler::binary,this), PREC_COMPARISON };
		parser_rules_map[TOKEN_STRING] = { std::bind(&Compiler::string, this), NULL, PREC_NONE };
	}


	bool compile() {
		parser.advance();
		while (!match(TOKEN_EOF)) {
			declaration();
		}
		emitByte(OP_RETURN);
		return !(this->parser.had_error);
	}

	void declaration() {
		if (match(TOKEN_VAR)) {
			varDeclaration();
		}
		else {
			statement();
		}
	}

	void varDeclaration() {
		uint8_t global = parseVariable("Expect variable name.");

		if (match(TOKEN_EQUAL)) {
			expression();
		}
		else {
			emitByte(OP_NIL);
		}
		parser.consume(TOKEN_SEMICOLON,"Expect ';' after variable declaration.");

		defineVariable(global);
	}

	uint8_t parseVariable(const char* errorMessage) {
		parser.consume(TOKEN_IDENTIFIER, errorMessage);
		declareVariable();
		if (scopeDepth > 0) return 0;
		return identifierConstant(&parser.previous);
	}

	uint8_t identifierConstant(Token* name) {
		std::string identifierName = (parser.previous.start);
		identifierName = identifierName.substr(0, parser.previous.length);
		Value value = Value(identifierName);
		return makeConstant(value);
	}

	void defineVariable(uint8_t global) {
		if (scopeDepth > 0) {
			markInitialized();
			return;
		}
		emitBytes(OP_DEFINE_GLOBAL, global);
	}

	void declareVariable() {
		if (scopeDepth == 0) return;

		Token* name = &parser.previous;
		if (locals.size() > localCount) {
			for (int i = localCount; i >= 0; i--) {
				Local* local = locals[i].get();
				if (local->depth != -1 && local->depth < this->scopeDepth) {
					break;
				}
				if (identifiersEqual(name, &local->name)) {
					std::cout << "Same variable name exists in this scope. " << "\n";
					break;
				}
			}
		}
		
		addLocal(*name);
	}

	void addLocal(Token name) {
		locals.push_back(std::make_unique<Local>(name, -1));
		localCount++;
	}

	void markInitialized() {
		locals[localCount - 1]->depth =scopeDepth;
	}

	void statement() {
		if (match(TOKEN_PRINT)) {
			printStatement();
		}
		else if (match(TOKEN_IF)) {
			ifStatement();
		}

		else if (match(TOKEN_WHILE)) {
			whileStatement();
		}

		else if (match(TOKEN_FOR)) {
			forStatement();
		}
		else if (match(TOKEN_LEFT_BRACE)) {
			beginScope();
			block();
			endScope();
		}
		else {
			expressionStatement();
		}
	}

	bool match(TokenType type) {
		if (!check(type)) return false;
		parser.advance();
		return true;
	}

	bool check(TokenType type) {
		return parser.current.type == type;
	}

	void printStatement() {
		expression();
		parser.consume(TOKEN_SEMICOLON, "Expect ';' after value.");
		emitByte(OP_PRINT);
		return;
	}

	void expressionStatement() {
		expression();
		parser.consume(TOKEN_SEMICOLON, "EXPECT ';' after expression.");
		emitByte(OP_POP);
		return;
	}

	void emitByte(int byte) {
		compiling_chunk->WriteChunk(byte, parser.previous.line);
	}

	void emitBytes(int byte1, int byte2) {
		emitByte(byte1);
		emitByte(byte2);
	}

	void expression() {
		parsePrecedence(PREC_ASSIGNMENT);
	}

	void parsePrecedence(Precedence precedence) {
		parser.advance();
		std::function<void()> function1 = parser_rules_map[parser.previous.type].prefix;
		if (function1 == NULL) {
			parser.error("Expect expression.");
			return;
		}
		function1();

		while (precedence <= parser_rules_map[parser.current.type].precedence) {
			parser.advance();
			std::function<void()> infixRule = parser_rules_map[parser.previous.type].infix;
			infixRule();
		}
	}

	void number() {
		Value value =Value("double",strtod(parser.previous.start, NULL));
		emitConstant(value);
	}

	void string() {
		std::string string = (parser.previous.start + 1);
		string = string.substr(0, parser.previous.length - 2);
		Value value = Value(string);
		emitConstant(value);
	}

	void variable() {
		namedVariable(parser.previous);
	}

	void namedVariable(Token name) {
		uint8_t getOp, setOp;
		int arg = resolveLocal(&name);
		if (arg != -1) {
			getOp = OP_GET_LOCAL;
			setOp = OP_SET_LOCAL;
		}
		else {
			arg = identifierConstant(&name);
			getOp = OP_GET_GLOBAL;
			setOp = OP_SET_GLOBAL;
		}

		if (match(TOKEN_EQUAL)) {
			expression();
			emitBytes(setOp, (int)arg);
		}
		else {
			emitBytes(getOp, (int)arg);
		}
	}

	int resolveLocal(Token* name) {
		for (int i = this->localCount - 1; i >= 0; i--) {
			Local* local = this->locals[i].get();
			if (identifiersEqual(name, &local->name)) {
				if (local->depth == -1) {
					continue;
				}
				return i;
			}
		}
		return -1;
	}

	void ifStatement() {
		parser.consume(TOKEN_LEFT_PAREN, "Expect '(' after 'if'.");
		expression();
		parser.consume(TOKEN_RIGHT_PAREN, "Expect ')' after condition.");

		int thenJump = emitJump(OP_JUMP_IF_FALSE);
		emitByte(OP_POP);
		statement();
		int elseJump = emitJump(OP_JUMP);
		patchJump(thenJump);
		emitByte(OP_POP);
		if (match(TOKEN_ELSE)) statement();
		patchJump(elseJump);
		
	}

	void whileStatement() {
		int loopStart = compiling_chunk->opcodes.size();
		parser.consume(TOKEN_LEFT_PAREN, "Expect '(' after 'while'.");
		expression();
		parser.consume(TOKEN_RIGHT_PAREN, "Expect ')' after condition.");
		int exitJump = emitJump(OP_JUMP_IF_FALSE);
		emitByte(OP_POP);
		statement();
		emitLoop(loopStart);
		patchJump(exitJump);
		emitByte(OP_POP);
	}

	void forStatement() {
		beginScope();
		parser.consume(TOKEN_LEFT_PAREN, "Expect '(' after 'for'.");
		if (match(TOKEN_SEMICOLON)) {
			// No initializer.
		}
		else if (match(TOKEN_VAR)) {
			varDeclaration();
		}
		else {
			expressionStatement();
		}
		int loopStart = compiling_chunk->opcodes.size();
		int exitJump = -1;

		if (!match(TOKEN_SEMICOLON)) {
			expression();
			parser.consume(TOKEN_SEMICOLON, "Expect ';' after loop condition.");
			exitJump = emitJump(OP_JUMP_IF_FALSE);
			emitByte(OP_POP);
		}

		if (!match(TOKEN_RIGHT_PAREN)) {
			int bodyJump = emitJump(OP_JUMP);
			int incrementStart = compiling_chunk->opcodes.size();
			expression();
			emitByte(OP_POP);
			parser.consume(TOKEN_RIGHT_PAREN, "Expect ')' after for clauses.");

			emitLoop(loopStart);
			loopStart = incrementStart;
			patchJump(bodyJump);
		}

		statement();
		emitLoop(loopStart);
		if (exitJump != -1) {
			patchJump(exitJump);
			emitByte(OP_POP); // Condition.
		}
		endScope();
	}

	void emitLoop(int start) {
		emitByte(OP_LOOP);
		int offset = compiling_chunk->opcodes.size() - start + 2;
		emitByte((offset >> 8) & 0xff);
		emitByte(offset & 0xff);
	}

	int emitJump(uint8_t instruction) {
		emitByte(instruction);
		emitByte(0xff);
		emitByte(0xff);
		return compiling_chunk->opcodes.size() - 2;
	}

	void patchJump(int offset) {
		int jump = compiling_chunk->opcodes.size() - offset-2;

		compiling_chunk->opcodes[offset] = (jump >> 8) & 0xff;
		compiling_chunk->opcodes[offset + 1] = jump & 0xff;

	}
	void and_() {
		int endJump = emitJump(OP_JUMP_IF_FALSE);

		emitByte(OP_POP);
		parsePrecedence(PREC_AND);

		patchJump(endJump);
	}

	void or_() {
		int elseJump = emitJump(OP_JUMP_IF_FALSE);
		int endJump = emitJump(OP_JUMP);

		patchJump(elseJump);
		emitByte(OP_POP);

		parsePrecedence(PREC_OR);
		patchJump(endJump);
	}

	void emitConstant(Value value) {
		emitBytes(OP_CONSTANT, makeConstant(value));
	}

	int makeConstant(Value value) {
		int constant_offset = this->compiling_chunk->AddConstant(value);
		return constant_offset;
	}

	void grouping() {
		expression();
		parser.consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
	}

	void unary() {
		TokenType operatorType = parser.previous.type;
		parsePrecedence(PREC_UNARY);
		switch (operatorType) {
		case TOKEN_BANG: emitByte(OP_NOT); break;
		case TOKEN_MINUS: emitByte(OP_NEGATE); break;
		default: return; 
		}
	}
	void binary() {
		TokenType operator_type = parser.previous.type;
		ParseRule* rule = &parser_rules_map[operator_type];
		parsePrecedence((Precedence)(rule->precedence + 1));
		switch (operator_type) {
		case TOKEN_PLUS:          emitByte(OP_ADD); break;
		case TOKEN_MINUS:         emitByte(OP_SUB); break;
		case TOKEN_STAR:          emitByte(OP_MUL); break;
		case TOKEN_SLASH:         emitByte(OP_DIV); break;
		case TOKEN_BANG_EQUAL:    emitBytes(OP_EQUAL, OP_NOT); break;
		case TOKEN_EQUAL_EQUAL:   emitByte(OP_EQUAL); break;
		case TOKEN_GREATER:       emitByte(OP_GREATER); break;
		case TOKEN_GREATER_EQUAL: emitBytes(OP_LESS, OP_NOT); break;
		case TOKEN_LESS:          emitByte(OP_LESS); break;
		case TOKEN_LESS_EQUAL:    emitBytes(OP_GREATER, OP_NOT); break;
		default: return;
		}
	}
	void literal() {
		switch (parser.previous.type) {
		case TOKEN_FALSE: emitByte(OP_FALSE); break;
		case TOKEN_NIL: emitByte(OP_NIL); break;
		case TOKEN_TRUE: emitByte(OP_TRUE); break;
		default: return; 
		}
	}

	void block() {
		while (!check(TOKEN_RIGHT_BRACE) && !check(TOKEN_EOF)) {
			declaration();
		}

		parser.consume(TOKEN_RIGHT_BRACE, "Expect '}' after block.");
	}

	void beginScope() {
		this->scopeDepth++;
	}

	void endScope() {
		this->scopeDepth--;
		while (localCount > 0 && locals[localCount - 1]->depth >scopeDepth) {
			emitByte(OP_POP);
			localCount--;
		}
	}

	bool identifiersEqual(Token* a, Token* b) {
		if (a->length != b->length) return false;
		return memcmp(a->start, b->start, a->length) == 0;
	}
};

