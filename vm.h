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

class StackFrame {
public:
	std::string function_name;
	int stack_start_offset;
	int ip_offset;
	StackFrame(std::string name, int offset,int ipoffset) {
		function_name = name;
		stack_start_offset = offset;
		ip_offset = ipoffset;
	}
};

class VM {
public:
	Chunk* chunk;
	std::vector<Value> stack;
	int ip;
	std::unordered_map<std::string, Value> vm_globals;
	std::unordered_map<std::string, std::shared_ptr<Chunk>> vm_functions;
	std::vector<std::shared_ptr<StackFrame>> vm_stackFrames;
	

	InterpretResult interpret(std::string source) {
		Chunk chunk = Chunk(1);
		vm_functions["main"]= std::make_shared<Chunk>(0);
		const char* source_c_str = source.c_str();
		Compiler compiler = Compiler(source_c_str, &vm_functions);
		bool compilation_result = compiler.compile();
		this->chunk = vm_functions["main"].get();
		this->chunk->function.funcName="main";
		this->ip = 0;
		vm_stackFrames.push_back(std::make_shared<StackFrame>("main",this->stack.size(), 0));
		//disassembleChunk(vm_functions["myFunction"].get());
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

	void destroyStackFrame(std::string name) {
		int stack_offset = (vm_stackFrames.end() - 1)->get()->stack_start_offset-vm_stackFrames.size()+2;
		int ip_offset = (vm_stackFrames.end() - 1)->get()->ip_offset;
		vm_stackFrames.pop_back();
		if (stack_offset != 0) {
			stack.erase(stack.end() - stack_offset , stack.end());
		}
		this->ip = ip_offset;
	}

	InterpretResult run() {
			//stack_trace();	
			int opcode = chunk->opcodes[this->ip];
			switch (opcode)
			{
			case OP_RETURN:
				ip += 1;
				if (chunk->function.funcName != "main") {
					destroyStackFrame(chunk->function.funcName);
					stack.push_back(Value());
					this->chunk = vm_functions[(vm_stackFrames.end() - 1)->get()->function_name].get();
					this->chunk->function.funcName = (vm_stackFrames.end() - 1)->get()->function_name;
				}
				//stack.clear();
				return INTERPRET_OK;
				break;

			case OP_RETURN_VALUE: {
				ip += 1;
				Value returnValue = stack.back();
				destroyStackFrame(chunk->function.funcName);
				this->chunk = vm_functions[(vm_stackFrames.end() - 1)->get()->function_name].get();
				this->chunk->function.funcName = (vm_stackFrames.end() - 1)->get()->function_name;
				stack.push_back(returnValue);
				return INTERPRET_OK;
				break;
			}

				
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
				int slot = chunk->opcodes[++ip]+(vm_stackFrames.end()-1)->get()->stack_start_offset;
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
			case OP_CALL: {
				int offset = chunk->opcodes[ip + 1];
				vm_stackFrames.push_back(std::make_shared<StackFrame>(this->chunk->constants[offset].returnString(), this->stack.size()+vm_stackFrames.size()-1, ip + 2));
				// does nothing for now
				ip = 0;
				this->chunk = vm_functions[this->chunk->constants[offset].returnString()].get();
				return INTERPRET_OK;
				break;
			}
			default:
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