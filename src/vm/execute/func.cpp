#include "../execute.h"
#include "../mem.h"
#include <assert.h>

void Execute::call(VM *vm, Instruction *instr) {
	AVM_memcell *func = vm->translate_operand(&instr->arg1, &vm->ax);

	mem::call_save_environment(vm);

	switch (func->type) {
	case AVM_memcell_t::userfunc_m:
		vm->pc = func->data.funcVal;
		break;
	case AVM_memcell_t::string_m:
		mem::calllibfunc(vm, func->data.strVal);
		break;
	case AVM_memcell_t::libfunc_m:
		mem::calllibfunc(vm, func->data.libfuncVal);
		break;
	default:
		vm->vmerr(" runtime error: attempting to call a non function ");
		vm->exec_finished = true;
		return;
	}

}

void Execute::pusharg(VM *vm, Instruction *instr) {
	AVM_memcell *arg = vm->translate_operand(&instr->arg1, &vm->ax);

	mem::assign(&vm->stack.stack[vm->top], arg);
	vm->totalActuals++;
	mem::dec_top(vm);
}

void Execute::funcenter(VM *vm, Instruction *instr) {
	AVM_memcell *func = vm->translate_operand(&instr->result, &vm->ax);
	assert(func);
	assert(vm->pc == func->data.funcVal);

	vm->totalActuals = 0;
	userfunc *funcInfo = mem::getfuncinfo(vm, vm->pc);
	vm->topsp = vm->top;
	vm->top = vm->top - funcInfo->local_size;

}

void Execute::funcexit(VM *vm, Instruction *instr) {
	unsigned old_top = vm->top;
	vm->top = mem::get_envvalue(vm, vm->topsp + AVM_SAVEDTOP_OFFSET);
	vm->pc = mem::get_envvalue(vm, vm->topsp + AVM_SAVEDPC_OFFSET);
	vm->topsp = mem::get_envvalue(vm, vm->topsp + AVM_SAVEDTOPSP_OFFSET);

	while (++old_top < vm->top) {
		memclear::clcell(&vm->stack.stack[old_top]);
	}
}