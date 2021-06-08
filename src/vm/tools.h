#ifndef __TOOLS__
#define __TOOLS__

#include "structs.h"
#include "vm.h"

typedef char *(*tostring_func_t)(VM *, AVM_memcell *);

namespace tostring
{

	extern char *avm_tostring(VM *vm, AVM_memcell *m);
	extern char *number(VM *vm, AVM_memcell *m);
	extern char *string(VM *vm, AVM_memcell *m);
	extern char *sbool(VM *vm, AVM_memcell *m);
	extern char *table(VM *vm, AVM_memcell *m);
	extern char *userfunc(VM *vm, AVM_memcell *m);
	extern char *libfunc(VM *vm, AVM_memcell *m);
	extern char *nil(VM *vm, AVM_memcell *m);
	extern char *undef(VM *vm, AVM_memcell *m);

	tostring_func_t tostringFuncs[];
}

typedef double (*arithmetic_func_t)(double x, double y);

namespace impl
{
	double add(double x, double y);
	double sub(double x, double y);
	double mul(double x, double y);
	double div(double x, double y);
	double mod(double x, double y);
	double minus(double x, double y);

	arithmetic_func_t arithmeticFuncs[];
}

typedef bool (*tobool_func_t)(AVM_memcell *);

namespace tobool
{
	unsigned char tobool(AVM_memcell *m);

	bool number(AVM_memcell *m);
	bool string(AVM_memcell *m);
	bool bbool(AVM_memcell *m);
	bool table(AVM_memcell *m);
	bool userfunc(AVM_memcell *m);
	bool libfunc(AVM_memcell *m);
	bool nil(AVM_memcell *m);
	bool undef(AVM_memcell *m);

	tobool_func_t toboolFuncs[];
}

namespace tables
{
	AVM_table *new_table(void);
	void init_table_buckets(AVM_table_bucket **p);
	void refcounter_increase(AVM_table *t);
	AVM_memcell *tablegetelem(AVM_table *table, AVM_memcell *index);
	void tablesetelem(AVM_table *table, AVM_memcell *index, AVM_memcell *value);

}

#endif