#include "../execute.h"
#include "../mem.h"
#include "../vm.h"

void Execute::assign(VM *vm, Instruction *instr) {
	AVM_memcell *lv = vm->translate_operand(&instr->result, nullptr);
	AVM_memcell *rv = vm->translate_operand(&instr->arg1, &vm->ax);

	mem::assign(lv, rv);
}