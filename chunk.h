#pragma once
#ifndef clox_chunk_h
#define clox_chunk_h
#include "common.h"
#include <vector>
#include <cinttypes>

typedef enum {
	OP_RETURN,
	OP_CONSTANT,
	OP_NEGATE,
	OP_ADD,
	OP_SUB,
	OP_MUL,
	OP_DIV,
} OpCode;


class Chunk{
public:
	std::vector<int> opcodes;
	std::vector<double> constants;
	std::vector<int> lines;
	int id;

	Chunk(int id) {
		this->id = id;
	}
	
	void WriteChunk(int opcode,int line) {
		opcodes.push_back(opcode);
		lines.push_back(line);
	}

	int AddConstant(double constant) {
		constants.push_back(constant);
		return constants.size()-1;
	}

};
#endif

