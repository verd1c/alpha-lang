#ifndef __STRUCTS__
#define __STRUCTS__

#define AVM_STACKSIZE   4096

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

#endif