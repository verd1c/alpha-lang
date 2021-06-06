#include "mem.h"

AVMStack::AVMStack() {
	for (unsigned i = 0; i < AVM_STACKSIZE; i++) {
		AVM_WIPEOUT(this->stack[i]);
		this->stack[i].type = AVM_memcell_t::undef_m;
	}
}

memclear::func_t memclear::funcs[] = {
		0,
		memclear::clstring,
		0,
		memclear::cltable,
		0,
		0,
		0,
		0
};

void memclear::clstring(AVM_memcell *mc) {
	free(mc->data.strVal);
}

void memclear::cltable(AVM_memcell *mc) {

}

void memclear::clcell(AVM_memcell *mc) {
	if (mc->type != AVM_memcell_t::undef_m) {
		memclear::func_t f = memclear::funcs[mc->type];
		if (f)
			(*f)(mc);
		mc->type = AVM_memcell_t::undef_m;
	}
}

void mem::assign(AVM_memcell *lv, AVM_memcell *rv) {
	if (lv == rv)
		return;

	if (lv->type == AVM_memcell_t::table_m &&
		rv->type == AVM_memcell_t::table_m &&
		lv->data.tableVal == rv->data.tableVal) {
		return;
	}

	if (rv->type == AVM_memcell_t::undef_m)
		printf("BUGGAROO\n");

	memclear::clcell(lv);

	memcpy(lv, rv, sizeof(AVM_memcell));

	if (lv->type == AVM_memcell_t::string_m) {
		lv->data.strVal = strdup(rv->data.strVal);
	}
}