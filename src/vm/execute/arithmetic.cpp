#include "../execute.h"
#include "../tools.h"
#include "../mem.h"
#include <assert.h>
#include <iostream>

void Execute::add(VM *vm, Instruction *instr) {
	AVM_memcell *lv = vm->translate_operand(&instr->result, (AVM_memcell *)0);
	AVM_memcell *rv1 = vm->translate_operand(&instr->arg1, &vm->ax);
	AVM_memcell *rv2 = vm->translate_operand(&instr->arg2, &vm->bx);

	//assert(lv && ((&vm->stack.stack[AVM_STACKSIZE] >= lv && &vm->stack.stack[0] < lv) || lv == &vm->retval));
	assert(rv1 && rv2);

	if (rv1->type != AVM_memcell_t::number_m || rv2->type != AVM_memcell_t::number_m)
	{
		vm->vmerr(" addition between non number arithmetics is not allowed ");
		vm->exec_finished = 1;
	}
	else
	{
		arithmetic_func_t op = impl::arithmeticFuncs[0];
		memclear::clcell(lv);
		lv->type = AVM_memcell_t::number_m;
		lv->data.numVal = (*op)(rv1->data.numVal, rv2->data.numVal);
	}
}

void Execute::sub(VM *vm, Instruction *instr) {
	AVM_memcell *lv = vm->translate_operand(&instr->result, (AVM_memcell *)0);
	AVM_memcell *rv1 = vm->translate_operand(&instr->arg1, &vm->ax);
	AVM_memcell *rv2 = vm->translate_operand(&instr->arg2, &vm->bx);

	assert(lv && ((&vm->stack.stack[AVM_STACKSIZE] >= lv && &vm->stack.stack[0] < lv) || lv == &vm->retval));
	assert(rv1 && rv2);

	if (rv1->type != AVM_memcell_t::number_m || rv2->type != AVM_memcell_t::number_m)
	{
		vm->vmerr(" sbustitution between non number arithmetics is not allowed ");
		vm->exec_finished = 1;
	}
	else
	{
		arithmetic_func_t op = impl::arithmeticFuncs[1];
		memclear::clcell(lv);
		lv->type = AVM_memcell_t::number_m;
		lv->data.numVal = (*op)(rv1->data.numVal, rv2->data.numVal);
	}
}

void Execute::mul(VM *vm, Instruction *instr) {
	AVM_memcell *lv = vm->translate_operand(&instr->result, (AVM_memcell *)0);
	AVM_memcell *rv1 = vm->translate_operand(&instr->arg1, &vm->ax);
	AVM_memcell *rv2 = vm->translate_operand(&instr->arg2, &vm->bx);

	//assert(lv && ((&vm->stack.stack[AVM_STACKSIZE] >= lv && &vm->stack.stack[0] < lv) || lv == &vm->retval));
	//assert(rv1 && rv2);

	if (rv1->type != AVM_memcell_t::number_m || rv2->type != AVM_memcell_t::number_m)
	{
		vm->vmerr(" multiplication between non number arithmetics is not allowed ");
		vm->exec_finished = 1;
	}
	else
	{
		arithmetic_func_t op = impl::arithmeticFuncs[2];
		memclear::clcell(lv);
		lv->type = AVM_memcell_t::number_m;
		lv->data.numVal = (*op)(rv1->data.numVal, rv2->data.numVal);
	}
}

void Execute::div(VM *vm, Instruction *instr) {
	AVM_memcell *lv = vm->translate_operand(&instr->result, (AVM_memcell *)0);
	AVM_memcell *rv1 = vm->translate_operand(&instr->arg1, &vm->ax);
	AVM_memcell *rv2 = vm->translate_operand(&instr->arg2, &vm->bx);

	assert(lv && ((&vm->stack.stack[AVM_STACKSIZE] >= lv && &vm->stack.stack[0] < lv) || lv == &vm->retval));
	assert(rv1 && rv2);

	if (rv1->type != AVM_memcell_t::number_m || rv2->type != AVM_memcell_t::number_m)
	{
		vm->vmerr(" division between non number arithmetics is not allowed ");
		vm->exec_finished = 1;
	}
	else
	{
		arithmetic_func_t op = impl::arithmeticFuncs[3];
		memclear::clcell(lv);
		lv->type = AVM_memcell_t::number_m;
		lv->data.numVal = (*op)(rv1->data.numVal, rv2->data.numVal);
	}
}

void Execute::mod(VM *vm, Instruction *instr) {
	AVM_memcell *lv = vm->translate_operand(&instr->result, (AVM_memcell *)0);
	AVM_memcell *rv1 = vm->translate_operand(&instr->arg1, &vm->ax);
	AVM_memcell *rv2 = vm->translate_operand(&instr->arg2, &vm->bx);

	assert(lv && ((&vm->stack.stack[AVM_STACKSIZE] >= lv && &vm->stack.stack[0] < lv) || lv == &vm->retval));
	assert(rv1 && rv2);

	if (rv1->type != AVM_memcell_t::number_m || rv2->type != AVM_memcell_t::number_m)
	{
		vm->vmerr(" modulo between non number arithmetics is not allowed ");
		vm->exec_finished = 1;
	}
	else
	{
		arithmetic_func_t op = impl::arithmeticFuncs[4];
		memclear::clcell(lv);
		lv->type = AVM_memcell_t::number_m;
		lv->data.numVal = (*op)(rv1->data.numVal, rv2->data.numVal);
	}
}

void Execute::minus(VM *vm, Instruction *instr) {
	AVM_memcell *lv = vm->translate_operand(&instr->result, (AVM_memcell *)0);
	AVM_memcell *rv1 = vm->translate_operand(&instr->arg1, &vm->ax);
	AVM_memcell *rv2 = vm->translate_operand(&instr->arg2, &vm->bx);

	assert(lv && ((&vm->stack.stack[AVM_STACKSIZE] >= lv && &vm->stack.stack[0] < lv) || lv == &vm->retval));
	assert(rv1 && rv2);

	if (rv1->type != AVM_memcell_t::number_m || rv2->type != AVM_memcell_t::number_m)
	{
		vm->vmerr(" unary minus between non number arithmetics is not allowed ");
		vm->exec_finished = 1;
	}
	else
	{
		arithmetic_func_t op = impl::arithmeticFuncs[5];
		memclear::clcell(lv);
		lv->type = AVM_memcell_t::number_m;
		lv->data.numVal = (*op)(rv1->data.numVal, -1);
	}
}
