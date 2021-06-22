#ifndef __EXECUTE__
#define __EXECUTE__

#include <stdio.h>
#include <iostream>
#include "vmarg.h"
#include "vm.h"

class VM;

namespace Execute {

	// assign.cpp
	extern void assign(VM *vm, Instruction *instr);

	// arithmetic.cpp
	extern void add(VM *vm, Instruction *instr);
	extern void sub(VM *vm, Instruction *instr);
	extern void mul(VM *vm, Instruction *instr);
	extern void div(VM *vm, Instruction *instr);
	extern void mod(VM *vm, Instruction *instr);
	extern void minus(VM *vm, Instruction *instr);

	// boolean.cpp
	extern void eand(VM *vm, Instruction *instr);
	extern void eor(VM *vm, Instruction *instr);
	extern void enot(VM *vm, Instruction *instr);

	// jump.cpp
	extern void jeq(VM *vm, Instruction *instr);
	extern void jne(VM *vm, Instruction *instr);
	extern void jle(VM *vm, Instruction *instr);
	extern void jge(VM *vm, Instruction *instr);
	extern void jlt(VM *vm, Instruction *instr);
	extern void jgt(VM *vm, Instruction *instr);

	// func.cpp
	extern void call(VM *vm, Instruction *instr);
	extern void pusharg(VM *vm, Instruction *instr);
	extern void funcenter(VM *vm, Instruction *instr);
	extern void funcexit(VM *vm, Instruction *instr);

	// table.cpp
	extern void newtable(VM *vm, Instruction *instr);
	extern void tablegetelem(VM *vm, Instruction *instr);
	extern void tablesetelem(VM *vm, Instruction *instr);

	extern void jump(VM *vm, Instruction *inst);

	// nop.cpp
	extern void nop(VM *vm, Instruction *instr);
}

extern void (*exec_funcs[])(VM *vm, Instruction *instr);

#endif