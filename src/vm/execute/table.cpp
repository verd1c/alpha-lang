#include "../execute.h"
#include "../mem.h"
#include "../tools.h"

void Execute::newtable(VM *vm, Instruction *instr) {
	AVM_memcell *lv = vm->translate_operand(&instr->result, (AVM_memcell*)0);

	memclear::clcell(lv);

	lv->type = AVM_memcell_t::table_m;
	lv->data.tableVal = new Table();
	lv->data.tableVal->incRefCounter();
}

void Execute::tablegetelem(VM *vm, Instruction *instr) {
	AVM_memcell *lv = vm->translate_operand(&instr->result, (AVM_memcell*)0);
	AVM_memcell *t = vm->translate_operand(&instr->arg1, (AVM_memcell*)0);
	AVM_memcell *i = vm->translate_operand(&instr->arg2, &vm->ax);

	memclear::clcell(lv);
	lv->type = AVM_memcell_t::nil_m;

	if (t->type != AVM_memcell_t::table_m) {
 		printf("Illegal use of type as table\n");
	}
	else {
		AVM_memcell *content = t->data.tableVal->getElem(i);
		if (content) {
			mem::assign(lv, content);
		}
		else {
			char *ts = tostring::avm_tostring(vm, t);
			char *is = tostring::avm_tostring(vm, i);
			printf("%s[%s] not found!\n", ts, is);
		}
	}
}

void Execute::tablesetelem(VM *vm, Instruction *instr) {
	AVM_memcell *t = vm->translate_operand(&instr->result, (AVM_memcell *)0);
	AVM_memcell *i = vm->translate_operand(&instr->arg1, &vm->ax);
	AVM_memcell *c = vm->translate_operand(&instr->arg2, &vm->bx);

	if (t->type != AVM_memcell_t::table_m) {
		printf("Illegal use of !\n");
	}
	else {
		t->data.tableVal->setElem(i, c);
	}
}