#include <iostream>
#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"

int main(int argv , char*argc[])
{
	Chunk chunk=Chunk(1);
	VM vm;
	int offset = chunk.AddConstant(100);
	int offset2 = chunk.AddConstant(12.67);
	chunk.WriteChunk(1,1);
	chunk.WriteChunk(offset,1);
	chunk.WriteChunk(1,2);
	chunk.WriteChunk(offset2,2);
	//chunk.WriteChunk(100,3);
	chunk.WriteChunk(1,4);
	chunk.WriteChunk(offset,4);
	chunk.WriteChunk(2, 5);
	chunk.WriteChunk(6, 5);
	chunk.WriteChunk(6, 5);
	chunk.WriteChunk(0,5);
	vm.interpret(&chunk);
	
}

