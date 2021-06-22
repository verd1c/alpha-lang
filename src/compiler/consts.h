#ifndef __VM_CONSTS__
#define __VM_CONSTS__

#include "symbol_table.h"

typedef struct userfunc userfunc;
typedef struct vmarg VMArg;
typedef struct instruction Instruction;
typedef struct incomplete_jump incomplete_jump;

extern unsigned int currQuad;

int *debug;

enum expand_t {
    EXPAND_NUM,
    EXPAND_STRING,
    EXPAND_LIB,
    EXPAND_USER,
    EXPAND_INSTR,
    EXPAND_IJ
};

enum vmopcode {
    assign_v, add_v, sub_v,
    mul_v, div_v, mod_v, 
    uminus_v, and_v, or_v, 
    not_v, jeq_v, jne_v, 
    jle_v, jge_v, jlt_v, 
    jgt_v, call_v, pusharg_v, 
    funcenter_v, funcexit_v, newtable_v, 
    tablegetelem_v, tablesetelem_v, jump_v, nop_v
};

enum vmarg_t {
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

struct vmarg {
    enum vmarg_t type;
    unsigned val;
};

struct instruction {
    enum vmopcode    opcode;
    VMArg       result;
    VMArg       arg1;
    VMArg       arg2;
    unsigned    src_line;
};

struct userfunc {
    unsigned    address;
    unsigned    local_size;
    char       *id;
};

struct incomplete_jump{
    unsigned instrNo;
    unsigned iaddress;
    incomplete_jump *next;
};

void add_incomplete_jump(unsigned instrNo, unsigned iaddress);

#define EXPAND_SIZE 1024

double *numConsts;
unsigned totalNumConsts;
unsigned currNum;
#define CURR_SIZE_NUMS   (totalNumConsts*sizeof(double))
#define NEW_SIZE_NUMS    (EXPAND_SIZE*sizeof(double)+CURR_SIZE_NUMS)

char **stringConsts;
unsigned totalStringConsts;
unsigned currString;
#define CURR_SIZE_STRINGS   (totalStringConsts*sizeof(char*))
#define NEW_SIZE_STRINGS    (EXPAND_SIZE*sizeof(char*)+CURR_SIZE_STRINGS)

char **namedLibfuncs;
unsigned totalNamedLibfuncs;
unsigned currLib;
#define CURR_SIZE_LIBS   (totalNamedLibfuncs*sizeof(char*))
#define NEW_SIZE_LIBS    (EXPAND_SIZE*sizeof(char*)+CURR_SIZE_LIBS)

userfunc *userFuncs;
unsigned totalUserFuncs;
unsigned currUser;
#define CURR_SIZE_USERS   (totalUserFuncs*sizeof(userfunc))
#define NEW_SIZE_USERS    (EXPAND_SIZE*sizeof(userfunc)+CURR_SIZE_USERS)

Instruction *instructions;
unsigned totalInstr;
unsigned currInstr;
#define CURR_SIZE_INSTR   (totalInstr*sizeof(Instruction))
#define NEW_SIZE_INSTR    (EXPAND_SIZE*sizeof(Instruction)+CURR_SIZE_INSTR)

incomplete_jump *ij_head;
unsigned totalIJ;
unsigned currIJ;
#define CURR_SIZE_IJ   (totalIJ*sizeof(incomplete_jump))
#define NEW_SIZE_IJ    (EXPAND_SIZE*sizeof(incomplete_jump)+CURR_SIZE_IJ)

unsigned consts_newstring(char *s);
unsigned consts_newnumber(double n);
unsigned libfuncs_newused(char *s);
unsigned userfuncs_newfunc(SymTableEntry *sym);


void parse_target_code();

#endif