#ifndef __MEM__
#define __MEM__

#include <vector>
#include <stack>
#include "vmarg.h"
#include "vm.h"
#include "structs.h"


class VM;

#define AVM_WIPEOUT(m)  memset(&(m), 0, sizeof(m))
#define AVM_STACKENV_SIZE   4
#define AVM_NUMACTUALS_OFFSET 4
#define AVM_SAVEDPC_OFFSET 3
#define AVM_SAVEDTOP_OFFSET 2
#define AVM_SAVEDTOPSP_OFFSET 1

namespace memclear {
    typedef void (*func_t)(AVM_memcell *);

    void clstring(AVM_memcell *mc);
    void cltable(AVM_memcell *mc);
    void clcell(AVM_memcell *mc);

    extern func_t funcs[];
}

namespace mem {
    void assign(AVM_memcell *lv, AVM_memcell *rv);

    void dec_top(VM *vm);
    unsigned get_envvalue(VM *vm, unsigned i);
    void push_envvalue(VM *vm, unsigned val);
    void call_save_environment(VM *vm);
    void calllibfunc(VM *vm, char *id);
    unsigned totalactuals(VM *vm);
    AVM_memcell *getactual(VM *vm, unsigned i);
}

#endif