#ifndef __VM__
#define __VM__

#include <iostream>
#include "vmarg.h"
#include "structs.h"


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
	unsigned totalGlobals = 0;

	/* Dispatcher */
	bool exec_finished = false;
	unsigned pc = 0;
	unsigned curr_line = 0;
	unsigned code_size = 0;
	//Instruction *code = 0;

	/* Memory */
	AVM_memcell ax;
	AVM_memcell bx;
	AVM_memcell cx;
	AVM_memcell retval;
	unsigned top;
	unsigned topsp;
	unsigned totalActuals = 0;
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