#include "../tools.h"
#include "../structs.h"
#include <assert.h>


tostring_func_t tostring::tostringFuncs[] = {
		number,
		string,
		sbool,
		table,
		userfunc,
		libfunc,
		nil,
		undef
};

char *tostring::avm_tostring(VM *vm, AVM_memcell *m)
{
	assert(m->type >= 0 && m->type <= AVM_memcell_t::undef_m);

	//tostringFuncs to idio me to_string??
	return (*tostring::tostringFuncs[m->type])(vm, m);
}

char *tostring::number(VM *vm, AVM_memcell *m) {
	char *st = (char *)malloc(strlen((char *)std::to_string(m->data.numVal).c_str()) * sizeof(char));
	strcpy(st, (char *)std::to_string(m->data.numVal).c_str());
	return st;
}

char *tostring::string(VM *vm, AVM_memcell *m) {
	return m->data.strVal;
}

char *tostring::sbool(VM *vm, AVM_memcell *m) {
	char *boolstr = (char *)malloc(6 * sizeof(char));
	memset(boolstr, '\0', 6);
	if (m->data.boolVar == 0) {
		strcpy(boolstr, "false");
	}
	else {
		strcpy(boolstr, "true");
	}
	return boolstr;
}

char *tostring::table(VM *vm, AVM_memcell *m) {
	std::string ts("dunno");
	return (char*)ts.c_str();
}

char *tostring::userfunc(VM *vm, AVM_memcell *m) {
	std::string ts("userfunc");
	return (char *)ts.c_str();
}

char *tostring::libfunc(VM *vm, AVM_memcell *m) {
	std::string ts("userfunc");
	return (char *)ts.c_str();
}

char *tostring::nil(VM *vm, AVM_memcell *m) {
	std::string ts("nil");
	return (char *)ts.c_str();
}

char *tostring::undef(VM *vm, AVM_memcell *m) {
	std::string ts("undef");
	return (char *)ts.c_str();
}