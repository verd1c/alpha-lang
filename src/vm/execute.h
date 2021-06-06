#ifndef __EXECUTE__
#define __EXECUTE__

#include "vmarg.h"

typedef void	(*exec_func_t)(Instruction *instr);

namespace Execute {

	// assign.cpp
	extern void assign(Instruction *instr);

	// arithmetic.cpp
	extern void add(Instruction *instr);
	extern void sub(Instruction *instr);
	extern void mul(Instruction *instr);
	extern void div(Instruction *instr);
	extern void mod(Instruction *instr);
	extern void minus(Instruction *instr);

	// boolean.cpp
	extern void eand(Instruction *instr);
	extern void eor(Instruction *instr);
	extern void enot(Instruction *instr);

	// jump.cpp
	extern void jeq(Instruction *instr);
	extern void jne(Instruction *instr);
	extern void jle(Instruction *instr);
	extern void jge(Instruction *instr);
	extern void jlt(Instruction *instr);
	extern void jgt(Instruction *instr);

	// func.cpp
	extern void call(Instruction *instr);
	extern void pusharg(Instruction *instr);
	extern void funcenter(Instruction *instr);
	extern void funcexit(Instruction *instr);

	// table.cpp
	extern void newtable(Instruction *instr);
	extern void tablegetelem(Instruction *instr);
	extern void tablesetelem(Instruction *instr);

	extern void jump(Instruction *inst);

	// nop.cpp
	extern void nop(Instruction *instr);
}

extern exec_func_t exec_funcs[];

#endif