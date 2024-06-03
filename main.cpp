#include <iostream>
#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"
#include <string>
#include "compiler.h"




static void repl(VM *vm) {
    while (1) {
        std::string input;
        std::cout << "> ";
        std::getline(std::cin,input);
        if (input == "exit") {
            break;
        }
        InterpretResult result = vm->interpret(input);
        if (result != INTERPRET_OK) {
            exit(10);
        }
    }
}

int main(int argc , const char*argv[])
{
    VM vm;
    repl(&vm);
    return 0;
}

