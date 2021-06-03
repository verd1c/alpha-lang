#ifndef __VMARG__
#define __VMARG__

#include <vector>

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

enum VMArg_t {
    label_a = 0,
    global_a = 1,
    formal_a = 2,
    local_a = 3,
    number_a = 4,
    string_a = 5,
    bool_a = 6,
    nil_a = 7,
    userfunc_a = 8,
    libfunc_a = 9,
    retval_a = 10,
    invalid_a = 11
};

struct VMArg {
    VMArg_t type;
    unsigned int val;
};

struct Instruction {
    VMOpcode_t op;
    VMArg result;
    VMArg arg1;
    VMArg arg2;
    int src_line;
};

class Compiler {

    unsigned int magic_number;
    unsigned int instruction_len;

    std::vector<Instruction> instructions;

public:
    int parse(const char *fname);
    unsigned int getMagicNumber();
    void printInstructions();
    std::vector<Instruction> getInstructions();
};

#endif