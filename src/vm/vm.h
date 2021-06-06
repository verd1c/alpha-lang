#ifndef __VM__
#define __VM__

#include <iostream>
#include "mem.h"
#include "vmarg.h"
#include "execute.h"

using namespace std;

class VM {
public:
	/* Meta Data */
	unsigned int magic_number;
	unsigned int instruction_len;

	/* Constants */
	vector<Instruction> instructions;
	vector<double> nums;
	vector<string> strings;
	vector<userfunc> userfuncs;
	vector<string> libfuncs;

	/* Dispatcher */
	bool exec_finished = false;
	unsigned pc = 0;
	unsigned curr_line = 0;
	unsigned code_size = 0;
	Instruction *code = (Instruction *)nullptr;



	/* Memory */
	AVM_memcell ax, bx, cx, retval;
	unsigned top, topsp;
	AVMStack stack;

	AVM_memcell *translate_operand(VMArg *arg, AVM_memcell *reg);

	VM();

	int parse(const char *fname);
	unsigned int getMagicNumber();
	void printInstructions();
	vector<Instruction> getInstructions();

	void execute_cycle(void);
};

#endif