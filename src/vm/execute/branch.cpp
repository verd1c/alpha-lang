#include "../execute.h"
#include "../tools.h"
#include <assert.h>

void Execute::jeq(VM *vm, Instruction *instr) {
	assert(instr->result.type == VMArg_t::label);

	AVM_memcell *rv1 = vm->translate_operand(&instr->arg1, &vm->ax);
	AVM_memcell *rv2 = vm->translate_operand(&instr->arg2, &vm->bx);

	unsigned char result = 0;

	if (rv1->type == AVM_memcell_t::undef_m || rv2->type == AVM_memcell_t::undef_m)
	{
		vm->vmerr("'undef' involved in equality\n");
		vm->exec_finished = 1;
	}
	else if (rv1->type == AVM_memcell_t::nil_m || rv2->type == AVM_memcell_t::nil_m)
	{
		result = (rv1->type == AVM_memcell_t::nil_m) && (rv2->type == AVM_memcell_t::nil_m);
	}
	else if (rv1->type == AVM_memcell_t::bool_m || rv2->type == AVM_memcell_t::bool_m)
	{
		result = (tobool::tobool(rv1) == tobool::tobool(rv2));
	}
	else if (rv1->type != rv2->type)
	{
		vm->vmerr(" equality check between different types is illegal ");
		vm->exec_finished = 1;
	}
	else
	{

		if (rv1->type == AVM_memcell_t::number_m)
		{
			result = rv1->data.numVal == rv2->data.numVal;
		}
		else if (rv1->type == AVM_memcell_t::string_m)
		{
			result = !strcmp(rv1->data.strVal, rv2->data.strVal);
		}
		else
		{
			result = (tobool::tobool(rv1) == tobool::tobool(rv2));
		}
	}

	if (!vm->exec_finished && result)
		vm->pc = instr->result.val;
}

void Execute::jne(VM *vm, Instruction *instr) {
	assert(instr->result.type == VMArg_t::label);

	AVM_memcell *rv1 = vm->translate_operand(&instr->arg1, &vm->ax);
	AVM_memcell *rv2 = vm->translate_operand(&instr->arg2, &vm->bx);

	unsigned char result = 0;

	if (rv1->type == AVM_memcell_t::undef_m || rv2->type == AVM_memcell_t::undef_m)
	{
		vm->vmerr(" 'undef' involved in equality ");
		vm->exec_finished = 1;
	}
	else if (rv1->type == AVM_memcell_t::nil_m || rv2->type == AVM_memcell_t::nil_m)
	{
		result = (rv1->type == AVM_memcell_t::nil_m) && (rv2->type == AVM_memcell_t::nil_m);
	}
	else if (rv1->type == AVM_memcell_t::bool_m || rv2->type == AVM_memcell_t::bool_m)
	{
		result = (tobool::tobool(rv1) == tobool::tobool(rv2));
	}
	else if (rv1->type != rv2->type)
	{
		vm->vmerr(" equality check between different types is illegal ");
		vm->exec_finished = 1;
	}
	else
	{

		if (rv1->type == AVM_memcell_t::number_m)
		{
			result = rv1->data.numVal == rv2->data.numVal;
		}
		else if (rv1->type == AVM_memcell_t::string_m)
		{
			result = !strcmp(rv1->data.strVal, rv2->data.strVal);
		}
		else
		{
			result = (tobool::tobool(rv1) == tobool::tobool(rv2));
		}
	}

	if (!vm->exec_finished && !result)
		vm->pc = instr->result.val;
}

void Execute::jle(VM *vm, Instruction *instr) {
	assert(instr->result.type == VMArg_t::label);

	AVM_memcell *rv1 = vm->translate_operand(&instr->arg1, &vm->ax);
	AVM_memcell *rv2 = vm->translate_operand(&instr->arg2, &vm->bx);

	unsigned char result = 0;

	if (rv1->type == AVM_memcell_t::undef_m || rv2->type == AVM_memcell_t::undef_m)
	{
		vm->vmerr(" 'undef' involved in equality ");
		vm->exec_finished = 1;
	}
	else if (rv1->type == AVM_memcell_t::nil_m || rv2->type == AVM_memcell_t::nil_m)
	{
		result = (rv1->type == AVM_memcell_t::nil_m) && (rv2->type == AVM_memcell_t::nil_m);
	}
	else if (rv1->type == AVM_memcell_t::bool_m || rv2->type == AVM_memcell_t::bool_m)
	{
		result = (tobool::tobool(rv1) == tobool::tobool(rv2));
	}
	else if (rv1->type != rv2->type)
	{
		vm->vmerr(" equality check between different types is illegal ");
		vm->exec_finished = 1;
	}
	else
	{

		if (rv1->type != AVM_memcell_t::number_m)
		{
			vm->vmerr(" not an arithmetic constant ");
		}
		else if (rv1->type == AVM_memcell_t::string_m)
		{
			vm->vmerr(" not an arithmetic constant ");
		}
		else
		{
			result = (rv1->data.numVal <= rv2->data.numVal);
		}
	}

	if (!vm->exec_finished && result)
		vm->pc = instr->result.val;
}

void Execute::jge(VM *vm, Instruction *instr) {
	assert(instr->result.type == VMArg_t::label);

	AVM_memcell *rv1 = vm->translate_operand(&instr->arg1, &vm->ax);
	AVM_memcell *rv2 = vm->translate_operand(&instr->arg2, &vm->bx);

	unsigned char result = 0;

	if (rv1->type == AVM_memcell_t::undef_m || rv2->type == AVM_memcell_t::undef_m)
	{
		vm->vmerr(" 'undef' involved in equality ");
		vm->exec_finished = 1;
	}
	else if (rv1->type == AVM_memcell_t::nil_m || rv2->type == AVM_memcell_t::nil_m)
	{
		result = (rv1->type == AVM_memcell_t::nil_m) && (rv2->type == AVM_memcell_t::nil_m);
	}
	else if (rv1->type == AVM_memcell_t::bool_m || rv2->type == AVM_memcell_t::bool_m)
	{
		result = (tobool::tobool(rv1) == tobool::tobool(rv2));
	}
	else if (rv1->type != rv2->type)
	{
		vm->vmerr(" equality check between different types is illegal ");
		vm->exec_finished = 1;
	}
	else
	{

		if (rv1->type != AVM_memcell_t::number_m)
		{
			vm->vmerr(" error:Not an arithmetic constant ");
		}
		else if (rv1->type == AVM_memcell_t::string_m)
		{
			vm->vmerr(" error:Not an arithmetic constant ");
		}
		else
		{
			result = (rv1->data.numVal >= rv2->data.numVal);
		}
	}

	if (!vm->exec_finished && result)
		vm->pc = instr->result.val;
}

void Execute::jlt(VM *vm, Instruction *instr) {
	assert(instr->result.type == VMArg_t::label);

	AVM_memcell *rv1 = vm->translate_operand(&instr->arg1, &vm->ax);
	AVM_memcell *rv2 = vm->translate_operand(&instr->arg2, &vm->bx);

	unsigned char result = 0;

	if (rv1->type == AVM_memcell_t::undef_m || rv2->type == AVM_memcell_t::undef_m)
	{
		vm->vmerr(" 'undef' involved in equality ");
		vm->exec_finished = 1;
	}
	else if (rv1->type == AVM_memcell_t::nil_m || rv2->type == AVM_memcell_t::nil_m)
	{
		result = (rv1->type == AVM_memcell_t::nil_m) && (rv2->type == AVM_memcell_t::nil_m);
	}
	else if (rv1->type == AVM_memcell_t::bool_m || rv2->type == AVM_memcell_t::bool_m)
	{
		result = (tobool::tobool(rv1) == tobool::tobool(rv2));
	}
	else if (rv1->type != rv2->type)
	{
		vm->vmerr(" equality check between different types is illegal ");
		vm->exec_finished = 1;
	}
	else
	{

		if (rv1->type != AVM_memcell_t::number_m)
		{
			vm->vmerr(" not an arithmetic constant ");
		}
		else if (rv1->type == AVM_memcell_t::string_m)
		{
			vm->vmerr(" not an arithmetic constant ");
		}
		else
		{
			result = (rv1->data.numVal < rv2->data.numVal);
		}
	}

	if (!vm->exec_finished && result)
		vm->pc = instr->result.val;
}

void Execute::jgt(VM *vm, Instruction *instr) {
	assert(instr->result.type == VMArg_t::label);

	AVM_memcell *rv1 = vm->translate_operand(&instr->arg1, &vm->ax);
	AVM_memcell *rv2 = vm->translate_operand(&instr->arg2, &vm->bx);

	unsigned char result = 0;

	if (rv1->type == AVM_memcell_t::undef_m || rv2->type == AVM_memcell_t::undef_m)
	{
		vm->vmerr(" 'undef' involved in equality ");
		vm->exec_finished = 1;
	}
	else if (rv1->type == AVM_memcell_t::nil_m || rv2->type == AVM_memcell_t::nil_m)
	{
		result = (rv1->type == AVM_memcell_t::nil_m) && (rv2->type == AVM_memcell_t::nil_m);
	}
	else if (rv1->type == AVM_memcell_t::bool_m || rv2->type == AVM_memcell_t::bool_m)
	{
		result = (tobool::tobool(rv1) == tobool::tobool(rv2));
	}
	else if (rv1->type != rv2->type)
	{
		vm->vmerr(" equality check between different types is illegal ");
		vm->exec_finished = 1;
	}
	else
	{

		if (rv1->type != AVM_memcell_t::number_m)
		{
			vm->vmerr(" not an arithmetic constant ");
		}
		else if (rv1->type == AVM_memcell_t::string_m)
		{
			vm->vmerr(" not an arithmetic constant ");
		}
		else
		{
			result = (rv1->data.numVal > rv2->data.numVal);
		}
	}

	if (!vm->exec_finished && result)
		vm->pc = instr->result.val;
}

void Execute::jump(VM *vm, Instruction *instr) {
	assert(instr->result.type == VMArg_t::label);

	if (!vm->exec_finished)
		vm->pc = instr->result.val;
}