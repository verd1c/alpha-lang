#include "../tools.h"
#include <assert.h>

tobool_func_t tobool::toboolFuncs[] = {
	tobool::number,
	tobool::string,
	tobool::bbool,
	tobool::table,
	tobool::userfunc,
	tobool::libfunc,
	tobool::nil,
	tobool::undef
};

unsigned char tobool::tobool(AVM_memcell *m) {
	assert(m->type >= 0 && m->type < AVM_memcell_t::undef_m);
	return (*toboolFuncs[m->type])(m);
}

bool tobool::number(AVM_memcell *m) {
	return m->data.numVal != 0;
}

bool tobool::string(AVM_memcell *m) {
	return m->data.strVal[0] != 0;
}

bool tobool::bbool(AVM_memcell *m) {
	return m->data.boolVar != 0;
}

bool tobool::table(AVM_memcell *m) {
	return true;
}

bool tobool::userfunc(AVM_memcell *m) {
	return true;
}

bool tobool::libfunc(AVM_memcell *m) {
	return true;
}

bool tobool::nil(AVM_memcell *m) {
	return false;
}

bool tobool::undef(AVM_memcell *m) {
	return false;
}