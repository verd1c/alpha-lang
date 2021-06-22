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

char *tostring::getTypeString(VM *vm, AVM_memcell *m) {
	switch (m->type) {
	case AVM_memcell_t::number_m:
		return strdup("number");
		break;
	case AVM_memcell_t::string_m:
		return strdup("string");
		break;
	case AVM_memcell_t::bool_m:
		return strdup("bool");
		break;
	case AVM_memcell_t::table_m:
		return strdup("table");
		break;
	case AVM_memcell_t::userfunc_m:
		return strdup("userfunc");
		break;
	case AVM_memcell_t::libfunc_m:
		return strdup("libfunc");
		break;
	case AVM_memcell_t::nil_m:
		return strdup("nil");
		break;
	case AVM_memcell_t::undef_m:
		return strdup("undefined");
		break;
	default:
		assert(0);
	}
}

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
	return strdup(m->data.strVal);
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
	Table *table = m->data.tableVal;
	
	std::string res("[");

	for (int i = 0; i < AVM_TABLE_HASHSIZE; i++) {
		if (m->data.tableVal->num_indexed[i]) {
			res = res + "{";
			res = res + tostring::avm_tostring(vm, &m->data.tableVal->num_indexed[i]->key);
			res = res + " : ";
			res = res + tostring::avm_tostring(vm, &m->data.tableVal->num_indexed[i]->value);
			res = res + "}";
		}
		if (m->data.tableVal->str_indexed[i]) {
			res = res + "{";
			res = res + tostring::avm_tostring(vm, &m->data.tableVal->str_indexed[i]->key);
			res = res + " : ";
			res = res + tostring::avm_tostring(vm, &m->data.tableVal->str_indexed[i]->value);
			res = res + "}";
		}
		if (m->data.tableVal->bol_indexed[i]) {
			res = res + "{";
			res = res + tostring::avm_tostring(vm, &m->data.tableVal->bol_indexed[i]->key);
			res = res + " : ";
			res = res + tostring::avm_tostring(vm, &m->data.tableVal->bol_indexed[i]->value);
			res = res + "}";
		}
		if (m->data.tableVal->usr_indexed[i]) {
			res = res + "{";
			res = res + tostring::avm_tostring(vm, &m->data.tableVal->usr_indexed[i]->key);
			res = res + " : ";
			res = res + tostring::avm_tostring(vm, &m->data.tableVal->usr_indexed[i]->value);
			res = res + "}";
		}
		if (m->data.tableVal->lib_indexed[i]) {
			res = res + "{";
			res = res + tostring::avm_tostring(vm, &m->data.tableVal->lib_indexed[i]->key);
			res = res + " : ";
			res = res + tostring::avm_tostring(vm, &m->data.tableVal->lib_indexed[i]->value);
			res = res + "}";
		}
		if (m->data.tableVal->tab_indexed[i]) {
			res = res + "{";
			res = res + tostring::avm_tostring(vm, &m->data.tableVal->tab_indexed[i]->key);
			res = res + " : ";
			res = res + tostring::avm_tostring(vm, &m->data.tableVal->tab_indexed[i]->value);
			res = res + "}";
		}
	}
	return strdup(res.c_str());
}

char *tostring::userfunc(VM *vm, AVM_memcell *m) {
	if (vm->userfuncs.size() < m->data.funcVal - 1) {
		vm->vmerr(" runtime error, attempting to overflow ");
		vm->exec_finished = true;
		return (char *)nullptr;
	}

	return strdup(vm->userfuncs.at(m->data.funcVal - 1).id);
}

char *tostring::libfunc(VM *vm, AVM_memcell *m) {
	return strdup(m->data.libfuncVal);
}

char *tostring::nil(VM *vm, AVM_memcell *m) {
	return strdup("nil");
}

char *tostring::undef(VM *vm, AVM_memcell *m) {
	return strdup("undef");
}