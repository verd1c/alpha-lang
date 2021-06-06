#ifndef __MEM__
#define __MEM__

#include <vector>
#include <stack>
#include "vmarg.h"

#define AVM_STACKSIZE   4096
#define AVM_WIPEOUT(m)  memset(&(m), 0, sizeof(m))
#define AVM_STACKENV_SIZE   4

namespace AVM_memcell_t {
    enum AVM_memcell_t {
        number_m,
        string_m,
        bool_m,
        table_m,
        userfunc_m,
        libfunc_m,
        nil_m,
        undef_m
    };
}

enum AVM_table;

union data_t {
    double numVal;
    char *strVal;
    unsigned char boolVar;
    struct avm_table *tableVal;
    unsigned funcVal;
    char *libfuncVal;
};

struct AVM_memcell {
    AVM_memcell_t::AVM_memcell_t type;

    data_t data;
};

class AVMStack {
public:
    AVM_memcell stack[AVM_STACKSIZE];
    AVMStack();
};

namespace memclear {
    typedef void (*func_t)(AVM_memcell *);

    void clstring(AVM_memcell *mc);
    void cltable(AVM_memcell *mc);
    void clcell(AVM_memcell *mc);

    extern func_t funcs[];
}

namespace mem {
    void assign(AVM_memcell *lv, AVM_memcell *rv);
}

#endif