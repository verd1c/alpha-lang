#ifndef __VM__
#define __VM__

#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>
#include "vmarg.h"
#include "structs.h"

extern int times;

class VM {
public:
	/* Meta Data */
	unsigned int magic_number;
	unsigned int instruction_len;

	/* Constants */
	std::vector<Instruction> instructions;
	std::vector<double> nums;
	std::vector<std::string> strings;
	std::vector<userfunc> userfuncs;
	std::vector<std::string> libfuncs;
	unsigned totalGlobals;

	/* Dispatcher */
	bool exec_finished;
	unsigned pc;
	unsigned curr_line;
	unsigned code_size;
	//Instruction *code = 0;

	/* Memory */
	AVM_memcell ax;
	AVM_memcell bx;
	AVM_memcell cx;
	AVM_memcell retval;
	unsigned top;
	unsigned topsp;
	unsigned totalActuals;
	AVMStack stack;

	AVM_memcell *translate_operand(VMArg *arg, AVM_memcell *reg);
	void vmerr(std::string err);

	VM();

	int parse(const char *fname);
	unsigned int getMagicNumber();
	void printInstructions();
	std::vector<Instruction> getInstructions();

	void execute_cycle(void);
};

#endif