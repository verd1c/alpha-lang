#include "../execute.h"

void Execute::nop(VM *vm, Instruction *instr) {
	 vm->exec_finished = 1;
}