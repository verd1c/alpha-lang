#include <math.h>
#include "mem.h"
#include "tools.h"

void callcheck(VM *vm, unsigned req) {
	unsigned n = mem::totalactuals(vm);
	if (n != req) {
		vm->exec_finished = true;
		vm->vmerr(" wrong number of arguments passed to library func ");
	}
	return;
}

void lib::print(VM *vm) {
	unsigned n = mem::totalactuals(vm);
	for (unsigned i = 0; i < n; i++) {
		char *str = tostring::avm_tostring(vm, mem::getactual(vm, i));
		printf("%s", str);
	}
}

void lib::input(VM *vm) {
	callcheck(vm, 0);

	char *inp = (char *)malloc(1024 * sizeof(char));
	fgets(inp, 1024, stdin);

	vm->retval.type = AVM_memcell_t::string_m;
	vm->retval.data.strVal = inp;
}

void lib::objectmemberkeys(VM *vm) {

}

void lib::objecttotalmembers(VM *vm) {
	callcheck(vm, 1);

	AVM_memcell *arg = mem::getactual(vm, 0);

	if (arg->type != AVM_memcell_t::table_m || !arg->data.tableVal) {
		vm->vmerr(" argument passed to objecttotalmembers was not of type table ");
		return;
	}

	vm->retval.type = AVM_memcell_t::number_m;
	vm->retval.data.numVal = arg->data.tableVal->total;
	return;
}

void lib::objectcopy(VM *vm) {
	callcheck(vm, 1);

	AVM_memcell *actual = mem::getactual(vm, 0);
	
	memcpy(&vm->retval, actual, sizeof(AVM_memcell));
}

void lib::totalarguments(VM *vm) {
	callcheck(vm, 0);

	unsigned topsp = mem::get_envvalue(vm, vm->topsp + AVM_SAVEDTOPSP_OFFSET);
	if (!topsp) {
		vm->vmerr(" totalarguments called outside a function ");
		vm->exec_finished = true;
		return;
	}

	vm->retval.type = AVM_memcell_t::number_m;
	vm->retval.data.numVal = mem::get_envvalue(vm, topsp + AVM_NUMACTUALS_OFFSET);

}

void lib::argument(VM *vm) {
	callcheck(vm, 1);

	AVM_memcell *actual = mem::getactual(vm, 0);

	if (actual->type != AVM_memcell_t::number_m) {
		vm->vmerr(" argument passed to argument was not of type number ");
		vm->exec_finished = true;
		return;
	}

	memcpy(&vm->retval, actual, sizeof(AVM_memcell));
	return;
}

void lib::typeoff(VM *vm) {
	callcheck(vm, 1);

	vm->retval.type = AVM_memcell_t::string_m;
	vm->retval.data.strVal = tostring::getTypeString(vm, mem::getactual(vm, 0));
}

void lib::strtonum(VM *vm) {
	callcheck(vm, 1);

	AVM_memcell *actual = mem::getactual(vm, 0);

	if (actual->type != AVM_memcell_t::string_m) {
		vm->vmerr(" wrong usage of strtonum ");
		vm->exec_finished = true;
		return;
	}

	vm->retval.type = AVM_memcell_t::number_m;
	vm->retval.data.numVal = std::atoi(actual->data.strVal);
	return;
}

void lib::sqrtt(VM *vm) {
	callcheck(vm, 1);

	AVM_memcell *actual = mem::getactual(vm, 0);

	if (actual->type != AVM_memcell_t::number_m) {
		vm->vmerr(" wrong usage of sqrt ");
		vm->exec_finished = true;
		return;
	}

	vm->retval.type = AVM_memcell_t::number_m;
	vm->retval.data.numVal = sqrt(actual->data.numVal);
	return;
}

void lib::coss(VM *vm) {
	callcheck(vm, 1);

	AVM_memcell *actual = mem::getactual(vm, 0);

	if (actual->type != AVM_memcell_t::number_m) {
		vm->vmerr(" wrong usage of cos ");
		vm->exec_finished = true;
		return;
	}

	vm->retval.type = AVM_memcell_t::number_m;
	vm->retval.data.numVal = cos(actual->data.numVal);
	return;
}

void lib::sinn(VM *vm) {
	callcheck(vm, 1);

	AVM_memcell *actual = mem::getactual(vm, 0);

	if (actual->type != AVM_memcell_t::number_m) {
		vm->vmerr(" wrong usage of sin ");
		vm->exec_finished = true;
		return;
	}

	vm->retval.type = AVM_memcell_t::number_m;
	vm->retval.data.numVal = sin(actual->data.numVal);
	return;
}