#ifndef __SYM_TABLE__
#define __SYM_TABLE__

#include "stack.h"

/*
* Structs used in bison
*/

#define SIZE 512
#define CALL_STACK_SIZE 64
#define HASH_MULTIPLIER 65599

extern int scope;

typedef enum scopespace_t {
    programvar,
    functionlocal,
    formalarg
}scopespace_t;

typedef struct OffsetStack_t{
    struct offset *next;
    scopespace_t scspace_off;
}OffsetStack;

enum Type{
    VAR,
    FUN
};

enum EntryType{
    LOCAL_VAR,
    GLOBAL_VAR,
    ARGUMENT_VAR,

    USER_FUNC,
    LIB_FUNC
};

typedef struct Variable {
    char *name;
    int scope;
    int line;
} Variable;

typedef struct Function {
    char *name;
    char **argv;
    int scope;
    int line;
} Function;

typedef struct SymTableEntry {
    int isActive;                       // wether entry is active or not

    enum EntryType type;                // entry type
    union{
        Variable *varValue;
        Function *funcValue;
    } value;

    scopespace_t scspace;
    unsigned offset;
    unsigned scope;

    struct SymTableEntry *nextEntry;    // next entry

    struct SymTableEntry *nextInScope;  // next in linked list by scope
    struct SymTableEntry *nextScope;    // next scope list
} SymTableEntry;

typedef struct SymTable_t{
    unsigned int length;                    // sym table length
    
    SymTableEntry *table[SIZE];         // hash table
    SymTableEntry *scopeChain;          // linked row lists by scope

} SymTable;

typedef struct CallStack_t{
    int size;
    int top;

    SymTableEntry *stack[CALL_STACK_SIZE];
} CallStack;

SymTable* init_sym_table();
SymTableEntry* lookup(SymTable *t, char *text, int scope, enum EntryType type);
SymTableEntry* lookup_no_type(SymTable *t, char *name, int scope);
SymTableEntry* insert(SymTable *t, char *name, int scope, int line, enum EntryType type);
void scope_down(SymTable *t);
SymTableEntry *function_lookup(SymTable *t, char *name, int scope);
SymTableEntry* lookup_variable(SymTable *t, char *name, int scope);
SymTableEntry* lookup_active(SymTable *t, char *name, int scope);
SymTableEntry *lookup_temp(SymTable *t, int scope);

// Stack
// CallStack *init_call_stack();
// void push(CallStack *s, SymTableEntry *e);
// void pop(CallStack *s);
// SymTableEntry *top(CallStack *s);
void printCallStack(CallStack *s, int line);
int is_valid(Stack *s, SymTableEntry *target, int curScope);
scopespace_t push_offset(OffsetStack *s, scopespace_t scspace);
scopespace_t pop_and_top_offset(OffsetStack *s);
OffsetStack * init_offset_stack();

#endif