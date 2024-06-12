#pragma once
#include "chunk.h"
#include "common.h"
#include <iostream>
#include "debug.h"
#include <string>
#include "compiler.h"
#include "value.h"
#include "variant"
#include <unordered_map>

typedef enum {
	INTERPRET_OK,
	INTERPRET_COMPILE_ERROR,
	INTERPRET_RUNTIME_ERROR,
} InterpretResult;

class VM {
public:
	Chunk* chunk;
	std::vector<Value> stack;
	int ip;
	std::unordered_map<std::string, Value> vm_globals;

	InterpretResult interpret(std::string source) {
		Chunk chunk=Chunk(1);
		const char* source_c_str = source.c_str();
		Compiler compiler = Compiler(source_c_str,&chunk);
		bool compilation_result = compiler.compile();
		this->chunk = &chunk;
		this->ip = 0;
		disassembleChunk(&chunk);
		while (ip < this->chunk->opcodes.size()) {
			InterpretResult result = run();
			if (result != INTERPRET_OK) {
				return result;
			}
		}
		return INTERPRET_OK;
	}

	void runtimeError() {
		std::cout << std::endl;
	}
	template<typename T, typename... Args>
	void runtimeError(T first, Args... args) {
		std::cout << first << " "; 
		runtimeError(args...);
	}

	InterpretResult run() {
			//stack_trace();	
			int opcode = chunk->opcodes[this->ip];
			switch (opcode)
			{
			case OP_RETURN:
				ip += 1;
				//stack.clear();
				return INTERPRET_OK;
				break;
				
			case OP_CONSTANT:
				
				//std::cout << "EXECUTING LOAD CONSTANT" << "\n";
				stack.push_back(chunk->constants[chunk->opcodes[this->ip + 1]]);
				ip += 2;
				return INTERPRET_OK;
				break;

			case OP_NEGATE:{
				//std::cout << "EXECUTING NEGATE" << "\n";

				if (!check_stack_unary()) {
					runtimeError("Not enough value on stack for operation");
					return INTERPRET_RUNTIME_ERROR;
				}

				if (!std::holds_alternative<double>(stack.back().value)) {
					runtimeError("Operand must be a double");
					return INTERPRET_RUNTIME_ERROR;
				}
				
				double val = -(stack.back().returnDouble());
				stack.pop_back();
				stack.push_back(Value("double",val));
				ip += 1;
				return INTERPRET_OK;
				break;
			}

			case OP_ADD: {
				//std::cout << "EXECUTING ADDITION" << "\n";
				if (!check_stack_bin()) {
					runtimeError("Not enough value on stack for operation");
					ip += 1;
					return INTERPRET_RUNTIME_ERROR;
					break;
				}

				if (stack.back().type == "string" && stack[stack.size()-2].type=="string") {
					std::string val1 = stack.back().returnString(); stack.pop_back();
					std::string val2 = stack.back().returnString(); stack.pop_back();
					stack.push_back(Value(val2 + val1));
					ip += 1;
					return INTERPRET_OK;
					break;
				}
				else if (stack.back().type == "double" && stack[stack.size() - 2].type == "double") {
					double val1 = stack.back().returnDouble();
					stack.pop_back();
					double val2 = stack.back().returnDouble();
					stack.pop_back();
					stack.push_back(Value("double", val1 + val2));
					ip += 1;
					return INTERPRET_OK;
					break;
				}
				else {
					runtimeError("Operation not permitted between given types");
					ip += 1;
					return INTERPRET_RUNTIME_ERROR;
					break;
				}
			}
			case OP_SUB: {
				//std::cout << "EXECUTING SUBTRACTION" << "\n";
				if (!check_stack_bin()) {
					runtimeError("Not enough value on stack for operation");
					ip += 1;
					return INTERPRET_RUNTIME_ERROR;
					break;
				}
				double val1 = -stack.back().returnDouble();
				stack.pop_back();
				double val2 = -stack.back().returnDouble();
				stack.pop_back();
				stack.push_back(Value("double",val1 - val2));
				ip += 1;
				return INTERPRET_OK;
				break;
			}

			case OP_MUL: {
				//std::cout << "EXECUTING MULTIPLICATION" << "\n";
				if (!check_stack_bin()) {
					runtimeError("Not enough value on stack for operation");
					ip += 1;
					return INTERPRET_RUNTIME_ERROR;
					break;
				}
				double val1 = -stack.back().returnDouble();
				stack.pop_back();
				double val2 = -stack.back().returnDouble();
				stack.pop_back();
				stack.push_back(Value("double",val1 * val2));
				ip += 1;
				return INTERPRET_OK;
				break;
			}

			case OP_DIV: {
				//std::cout << "EXECUTING DIVISION" << "\n";
				if (!check_stack_bin()) {
					runtimeError("Not enough value on stack for operation");
					
					ip += 1;
					return INTERPRET_RUNTIME_ERROR;
					break;
				}
				double val1 = -stack.back().returnDouble();
				stack.pop_back();
				double val2 = -stack.back().returnDouble();
				if (val1 != 0) {
					stack.pop_back();
					stack.push_back(Value("double",val2 / val1));
					ip += 1;
					return INTERPRET_OK;
				}
				else {
					std::cout << "Error division by zero at line " << chunk->lines[ip];
					return INTERPRET_RUNTIME_ERROR;
				}
				break;
			}

			case OP_NIL: {
				stack.push_back(Value("nil"));
				ip++;
				return INTERPRET_OK;
				break;
			}

			case OP_TRUE: {
				bool val = 1;
				stack.push_back(Value("bool", val));
				ip++;
				return INTERPRET_OK;
				break;
			}

			case OP_FALSE: {
				bool val = 0;
				stack.push_back(Value("bool", val));
				ip++;
				return INTERPRET_OK;
				break;
			}

			case OP_NOT: {
				if (stack.back().type == "bool"){
					bool val = stack.back().returnBool();
					stack.pop_back();
					stack.push_back(Value("bool", !val));
					ip++;
					return INTERPRET_OK;
				}
				else if (stack.back().type == "double") {
					bool val = stack.back().returnDouble();
					stack.pop_back();
					stack.push_back(Value("bool", !val));
					ip++;
					return INTERPRET_OK;
				}
				else if (stack.back().type == "nil") {
					bool val = 1;
					stack.pop_back();
					stack.push_back(Value("bool", val));
					ip++;
					return INTERPRET_OK;
				}
				else {
					runtimeError("Error encountered in Not operator");
					ip++;
					return INTERPRET_RUNTIME_ERROR;
				}
			}
			case OP_EQUAL:{
				Value a = stack.back(); stack.pop_back();
				Value b = stack.back(); stack.pop_back();
				stack.push_back(Value("bool", a.ValuesEqual(b)));
				ip++;
				return INTERPRET_OK;
				
			}

			case OP_GREATER: {
				Value a = stack.back(); stack.pop_back();
				Value b = stack.back(); stack.pop_back();
				bool val = a.returnDouble() < b.returnDouble();
				stack.push_back(Value("bool", val));
				ip++;
				return INTERPRET_OK;
			}

			case OP_LESS: {
				Value a = stack.back(); stack.pop_back();
				Value b = stack.back(); stack.pop_back();
				bool val = a.returnDouble() > b.returnDouble();
				stack.push_back(Value("bool", val));
				ip++;
				return INTERPRET_OK;
			}

			case OP_PRINT: {
				if (!check_stack_unary()) {
					runtimeError("Not enough values on stack for print operation");
					return INTERPRET_RUNTIME_ERROR;
				}
				Value value = stack.back(); stack.pop_back();
				value.printValue();
				printf("\n");
				ip++;
				return INTERPRET_OK;
				break;
			}

			case OP_POP: {
				stack.pop_back();
				ip++;
				return INTERPRET_OK;
				break;
			}

			case OP_DEFINE_GLOBAL: {
				StringObject name = chunk->constants[chunk->opcodes[this->ip + 1]].returnStringObject();
				//std::cout << name.getString() << "\n";
				vm_globals[name.getString()] = stack.back();
				stack.pop_back();
				ip += 2;
				return INTERPRET_OK;
				break;
			}

			case OP_GET_GLOBAL: {
				
				StringObject name = chunk->constants[chunk->opcodes[this->ip + 1]].returnStringObject();
				Value value;
				if (vm_globals.count(name.getString())==0) {
					runtimeError("Undefined variable" , name.getString());
					ip += 2;
					return INTERPRET_RUNTIME_ERROR;
					break;
				}
				Value element = vm_globals.at(name.getString());
				stack.push_back(element);
				ip+=2;
				return INTERPRET_OK;
				break;
			}

			case OP_SET_GLOBAL: {
				StringObject name = chunk->constants[chunk->opcodes[this->ip + 1]].returnStringObject();
				//std::cout << name.getString() << "\n";
				vm_globals[name.getString()] = stack.back();
				ip += 2;
				return INTERPRET_OK;
				break;
			}

			case OP_GET_LOCAL: {
				int slot = chunk->opcodes[++ip];
				stack.push_back(stack[slot]);
				ip += 1;
				return INTERPRET_OK;
				break;
			}

			case OP_SET_LOCAL: {
				uint8_t slot = chunk->opcodes[++ip];
				this->stack[slot] = stack.back();
				ip += 1;
				return INTERPRET_OK;
				break;
			}

			case OP_JUMP_IF_FALSE: {
				ip += 3;
				uint16_t offset = (uint16_t)((chunk->opcodes[ip-2] << 8) | chunk->opcodes[ip-1]);
				if (stack.back().returnBool() == false) ip += offset;
				return INTERPRET_OK;
				break;
			}
			case OP_JUMP: {
				ip += 3;
				uint16_t offset = (uint16_t)((chunk->opcodes[ip - 2] << 8) | chunk->opcodes[ip - 1]);
				ip += offset;
				return INTERPRET_OK;
				break;
			}
			case OP_LOOP: {
				ip += 3;
				uint16_t offset = (uint16_t)((chunk->opcodes[ip - 2] << 8) | chunk->opcodes[ip - 1]);
				ip -= offset;
				return INTERPRET_OK;
				break;
			}
			default:
				//std::cout << "COMPILE ERROR : UNKNOWN INSTRUCTION ENCOUNTERED" << "\n";
				runtimeError("Unknown Instruction Encountered");
				return INTERPRET_RUNTIME_ERROR;
				break;
				
			}
	}

	void stack_trace() {
		if (stack.size() == 0) {
			std::cout << "Stack Empty" << "\n";
			return;
		}
		for (auto it = stack.begin(); it < stack.end(); ++it) {
			(it->printValue());
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