#ifndef __MEM__
#define __MEM__

#include <vector>
#include <stack>

#define AVM_STACKSIZE   4096
#define AVM_WIPEOUT(m)  memset(&(m), 0, sizeof(m))

enum AVM_memcell_t {
    number_m = 0,
    string_m = 1,
    bool_m = 2,
    table_m = 3,
    userfunc_m = 4,
    libfunc_m = 6,
    nil_m = 6,
    undef_m = 7
};

enum AVM_table;

struct AVM_memcell {
    AVM_memcell_t type;
    union data {
        double numVal;
        char *strVal;
        unsigned char boolVar;
        struct avm_table *tableVal;
        unsigned funcVal;
        char *libfuncVal;
    };
};

class AVMStack {
    std::stack<AVM_memcell> stack;

    AVMStack() {
        
    }
};

#endif