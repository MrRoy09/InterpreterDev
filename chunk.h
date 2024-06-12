#pragma once

#include "common.h"
#include "value.h"
#include <vector>
#include <cinttypes>

typedef enum {
	OP_RETURN,
	OP_CONSTANT,
	OP_NIL,
	OP_TRUE,
	OP_FALSE,
	OP_NOT,
	OP_EQUAL,
	OP_GREATER,
	OP_LESS,
	OP_NEGATE,
	OP_ADD,
	OP_SUB,
	OP_MUL,
	OP_DIV,
	OP_PRINT,
	OP_POP,
	OP_DEFINE_GLOBAL,
	OP_GET_GLOBAL,
	OP_SET_GLOBAL,
	OP_GET_LOCAL,
	OP_SET_LOCAL,
	OP_JUMP_IF_FALSE,
	OP_JUMP,
	OP_LOOP,
} OpCode;


class Chunk{
public:
	std::vector<int> opcodes;
	std::vector<Value> constants;
	std::vector<int> lines;
	int id;

	Chunk(int id) {
		this->id = id;
	}
	
	void WriteChunk(int opcode,int line) {
		opcodes.push_back(opcode);
		lines.push_back(line);
	}

	int AddConstant(Value constant) {
		constants.push_back(constant);
		return constants.size()-1;
	}
};

