/*
* Structs used in bison
*/
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#define SIZE 512
#define HASH_MULTIPLIER 65599

extern int scope;

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

    struct SymTableEntry *nextEntry;    // next entry

    struct SymTableEntry *nextInScope;  // next in linked list by scope
    struct SymTableEntry *nextScope;    // next scope list
} SymTableEntry;

typedef struct SymTable_t{
    unsigned length;                    // sym table length
    
    SymTableEntry *table[SIZE];         // hash table
    SymTableEntry *scopeChain;          // linked row lists by scope

} SymTable;

SymTable* init_sym_table();
SymTableEntry* lookup(SymTable *t, char *text, int scope, enum EntryType type);
SymTableEntry* lookup_no_type(SymTable *t, char *name, int scope);
int insert(SymTable *t, char *name, int scope, int line, enum EntryType type);
void scope_down(SymTable *t);