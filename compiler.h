#pragma once
#include <string>
#include <vector>
#include "chunk.h"
#include <map>
#include <functional>

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
		errorAtCurrent(message);
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

	typedef struct {
		std::function<void()> prefix;
		std::function<void()> infix;
		Precedence precedence;
	} ParseRule;

	const char* source;
	Chunk * compiling_chunk;
	Scanner scanner;
	Parser parser;
	bool had_error=0;
	std::map<TokenType, ParseRule> parser_rules_map;

	
	Compiler(const char *source, Chunk *chunk):parser(source,&scanner){
		this->source = source;
		this->compiling_chunk = chunk;
		this->scanner.start = source;
		this->scanner.current = source;
		this->scanner.line = 0;
		parser.advance();
		parser_rules_map[TOKEN_LEFT_PAREN] = { std::bind(&Compiler::grouping, this), NULL, PREC_NONE};
		parser_rules_map[TOKEN_RIGHT_PAREN] = { NULL,NULL,PREC_NONE };
		parser_rules_map[TOKEN_LEFT_BRACE] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_RIGHT_BRACE] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_COMMA] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_DOT] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_MINUS] = {std::bind(&Compiler::unary,this), std::bind(&Compiler::binary,this), PREC_TERM};
		parser_rules_map[TOKEN_PLUS] = { NULL, std::bind(&Compiler::binary,this), PREC_TERM};
		parser_rules_map[TOKEN_SEMICOLON] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_SLASH] = { NULL, std::bind(&Compiler::binary,this), PREC_FACTOR};
		parser_rules_map[TOKEN_STAR] = { NULL, std::bind(&Compiler::binary,this), PREC_FACTOR};
		parser_rules_map[TOKEN_BANG] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_BANG_EQUAL] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_EQUAL] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_EQUAL_EQUAL] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_GREATER] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_GREATER_EQUAL] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_LESS] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_LESS_EQUAL] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_IDENTIFIER] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_STRING] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_NUMBER] = {std::bind(&Compiler::number,this),   NULL,   PREC_NONE};
		parser_rules_map[TOKEN_AND] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_CLASS] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_ELSE] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_FALSE] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_FOR] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_FUN] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_IF] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_NIL] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_OR] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_PRINT] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_RETURN] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_SUPER] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_THIS] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_TRUE] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_VAR] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_WHILE] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_ERROR] = { NULL,     NULL,   PREC_NONE };
		parser_rules_map[TOKEN_EOF] = { NULL,     NULL,   PREC_NONE };
	}

	bool compile() {
		expression();
		parser.consume(TOKEN_EOF, "Expected end of file");
		emitByte(OP_RETURN);
		return !(this->parser.had_error);
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
		double value = strtod(parser.previous.start, NULL);
		emitConstant(value);
	}

	void emitConstant(double value) {
		emitBytes(OP_CONSTANT, makeConstant(value));
	}

	int makeConstant(double value) {
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
		expression();
		switch (operatorType) {
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
		default: return;
		}
	}

	

	

};

