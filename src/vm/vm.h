#ifndef __VM__
#define __VM__

#include <iostream>
#include "mem.h"
#include "vmarg.h"

using namespace std;

class VM {
	/* Meta Data */
	unsigned int magic_number;
	unsigned int instruction_len;

	/* Constants */
	vector<Instruction> instructions;
	vector<double> nums;
	vector<string> strings;
	vector<string> libfuncs;

	/* Memory */
	AVM_memcell ax, bx, cx, retval;
	unsigned top, topsp;
	AVMStack stack;
	vector<double> nums;
	vector<string> strings;

	AVM_memcell *translate_operand(VMArg *arg, AVM_memcell *reg);

public:
	VM();

	int parse(const char *fname);
	unsigned int getMagicNumber();
	void printInstructions();
	vector<Instruction> getInstructions();

	void execution_cycle();
};

#endif