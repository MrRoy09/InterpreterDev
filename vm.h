#pragma once
#include "chunk.h"
#include "common.h"
#include <iostream>
#include "debug.h"
#include <string>
#include "compiler.h"


typedef enum {
	INTERPRET_OK,
	INTERPRET_COMPILE_ERROR,
	INTERPRET_RUNTIME_ERROR,
} InterpretResult;

class VM {
public:
	Chunk* chunk;
	std::vector<double> stack;
	int ip;

	InterpretResult interpret(std::string source) {
		Chunk chunk=Chunk(1);
		const char* source_c_str = source.c_str();
		Compiler compiler = Compiler(source_c_str,&chunk);
		bool compilation_result = compiler.compile();
		this->chunk = &chunk;
		this->ip = 0;
		while (ip < this->chunk->opcodes.size()) {
			InterpretResult result = run();
		}
		
		return INTERPRET_OK;
	}

	InterpretResult run() {
			stack_trace();	
			int opcode = chunk->opcodes[this->ip];
			switch (opcode)
			{
			case OP_RETURN:
				
				std::cout << "EXECUTING RETURN" << "\n";
				ip += 1;
				stack.clear();
				return INTERPRET_OK;
				break;
				
			case OP_CONSTANT:
				
				std::cout << "EXECUTING LOAD CONSTANT" << "\n";
				stack.push_back(chunk->constants[chunk->opcodes[this->ip + 1]]);
				ip += 2;
				return INTERPRET_OK;
				break;

			case OP_NEGATE:{
				std::cout << "EXECUTING NEGATE" << "\n";
				double val = -stack.back();
				stack.pop_back();
				stack.push_back(val);
				ip += 1;
				return INTERPRET_OK;
				break;
			}
			case OP_ADD: {
				std::cout << "EXECUTING ADDITION" << "\n";
				if (!check_stack_bin()) {
					std::cout << "STACK CHECK FAIL" << "\n";
					ip += 1;
					return INTERPRET_COMPILE_ERROR;
					break;
				}
				double val1 = stack.back();
				stack.pop_back();
				double val2 = stack.back();
				stack.pop_back();
				stack.push_back(val1 + val2);
				ip += 1;
				return INTERPRET_OK;
				break;
			}
			case OP_SUB: {
				std::cout << "EXECUTING SUBTRACTION" << "\n";
				if (!check_stack_bin()) {
					std::cout << "STACK CHECK FAIL" << "\n";
					ip += 1;
					return INTERPRET_COMPILE_ERROR;
					break;
				}
				double val1 = -stack.back();
				stack.pop_back();
				double val2 = -stack.back();
				stack.pop_back();
				stack.push_back(val1 - val2);
				ip += 1;
				return INTERPRET_OK;
				break;
			}

			case OP_MUL: {
				std::cout << "EXECUTING MULTIPLICATION" << "\n";
				if (!check_stack_bin()) {
					std::cout << "STACK CHECK FAIL" << "\n";
					ip += 1;
					return INTERPRET_COMPILE_ERROR;
					break;
				}
				double val1 = -stack.back();
				stack.pop_back();
				double val2 = -stack.back();
				stack.pop_back();
				stack.push_back(val1 * val2);
				ip += 1;
				return INTERPRET_OK;
				break;
			}

			case OP_DIV: {
				std::cout << "EXECUTING DIVISION" << "\n";
				if (!check_stack_bin()) {
					std::cout << "STACK CHECK FAIL" << "\n";
					ip += 1;
					return INTERPRET_COMPILE_ERROR;
					break;
				}
				double val1 = -stack.back();
				stack.pop_back();
				double val2 = -stack.back();
				if (val2 != 0) {
					stack.pop_back();
					stack.push_back(val1 / val2);
					ip += 1;
					return INTERPRET_OK;
				}
				else {
					return INTERPRET_COMPILE_ERROR;
				}
				
				break;
			}

		
				
			default:
				std::cout << "COMPILE ERROR : UNKNOWN INSTRUCTION ENCOUNTERED" << "\n";
				return INTERPRET_COMPILE_ERROR;
				break;
				
			}
	}

	void stack_trace() {
		for (auto it = stack.begin(); it < stack.end(); ++it) {
			std::cout << *it << " ";
		}
		std::cout << "\n";
	}

	bool check_stack_unary() {
		if (stack.size() > 0) {
			return 1;
		}
		return 0;
	}

	bool check_stack_bin() {
		if (stack.size() < 2) {
			return 0;
		}
		else {
			return 1;
		}
	}

};