#include "execute.h"

exec_func_t exec_funcs[] = {
	// assign.cpp
	Execute::assign,

	// arithmetic.cpp
	Execute::add,
	Execute::sub,
	Execute::mul,
	Execute::div,
	Execute::mod,
	Execute::minus,

	// boolean.cpp
	Execute::eand,
	Execute::eor,
	Execute::enot,

	// jump.cpp
	Execute::jeq,
	Execute::jne,
	Execute::jle,
	Execute::jge,
	Execute::jlt,
	Execute::jgt,

	// func.cpp
	Execute::call,
	Execute::pusharg,
	Execute::funcenter,
	Execute::funcexit,

	// table.cpp
	Execute::newtable,
	Execute::tablegetelem,
	Execute::tablesetelem,

	Execute::jump,

	// nop.cpp
	Execute::nop
};