#include "../execute.h"
#include "mem.h"
#include "assert.h"
#include "../tools.h"



void Execute::newtable(VM *vm, Instruction *instr)
{
	AVM_memcell *lv = vm->translate_operand(&instr->result, (AVM_memcell *)0);
	assert(lv && (&vm->stack.stack[AVM_STACKSIZE - 1] >= lv && lv > &vm->stack.stack[vm->top]) || (lv == &vm->retval));

	memclear::clcell(lv);

	lv->type = AVM_memcell_t::table_m;
	lv->data.tableVal = tables::new_table();
	tables::refcounter_increase(lv->data.tableVal);
}

void Execute::tablegetelem(VM *vm, Instruction *instr)
{
	AVM_memcell *lv = vm->translate_operand(&instr->result, (AVM_memcell *)0);
	AVM_memcell *t = vm->translate_operand(&instr->arg1, (AVM_memcell *)0);
	AVM_memcell *i = vm->translate_operand(&instr->arg2, &vm->ax);

	assert(lv && (&vm->stack.stack[AVM_STACKSIZE - 1] >= lv && lv > &vm->stack.stack[vm->top]) || (lv == &vm->retval));
	assert(t && (&vm->stack.stack[AVM_STACKSIZE-1]) >= t && t > (&vm->stack.stack[vm->top]));
	assert(i);

	memclear::clcell(lv);
	lv->type = AVM_memcell_t::nil_m;

	if(t->type != AVM_memcell_t::table_m)
		printf("Illegal use of type %s as table!\n", tostring::tostringFuncs[t->type]);
	else {
		AVM_memcell *content = tables::tablegetelem(t->data.tableVal, i);
		if (content)
			mem::assign(lv, content);
		else
		{
			char *ts = tostring::avm_tostring(vm,t);
			char *is = tostring::avm_tostring(vm,i);
			printf("%s[%s] not found!\n", ts, is);
			free(ts);
			free(is);
		}
	}
}

void Execute::tablesetelem(VM *vm, Instruction *instr)
{
	AVM_memcell *t = vm->translate_operand(&instr->result, (AVM_memcell *)0);
	AVM_memcell *i = vm->translate_operand(&instr->arg1, &vm->ax);
	AVM_memcell *c = vm->translate_operand(&instr->arg2, &vm->bx);

	assert(t && &vm->stack.stack[AVM_STACKSIZE - 1] >= t && &vm->stack.stack[vm->top]);
	assert(i && c);

	if (t->type != AVM_memcell_t::table_m)
		printf("Illegal use of type %s as table!\n",  tostring::tostringFuncs[t->type]);
	else
		tables::tablesetelem(t->data.tableVal, i, c);
}