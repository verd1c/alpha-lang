#ifndef __VMARG__
#define __VMARG__

enum VMOpcode_t {
    assign_v, add_v, sub_v,
    mul_v, div_v, mod_v,
    uminus_v, and_v, or_v,
    not_v, jeq_v, jne_v,
    jle_v, jge_v, jlt_v,
    jgt_v, call_v, pusharg_v,
    funcenter_v, funcexit_v, newtable_v,
    tablegetelem_v, tablesetelem_v, jump_v, nop_v
};

namespace VMArg_t {
    enum VMArg_t {
        label,
        global,
        formal,
        local,
        number,
        string,
        bool_a,
        nil,
        userfunc,
        libfunc,
        retval,
        invalid
    };
}

struct VMArg {
    VMArg_t::VMArg_t type;
    unsigned int val;
};

struct userfunc {
    unsigned    address;
    unsigned    local_size;
    char *id;
};

struct Instruction {
    VMOpcode_t op;
    VMArg result;
    VMArg arg1;
    VMArg arg2;
    int src_line;
};

#endif