/*
* Structs used in bison
*/
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#define SIZE 512
#define HASH_MULTIPLIER 65599

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

    int active;

    enum EntryType type;
    union{
        Variable *varValue;
        Function *funcValue;
    } value;

    struct SymTableEntry *nextEntry;
} SymTableEntry;

typedef struct SymTable_t{
    unsigned length;
    SymTableEntry *table[SIZE];
} SymTable;

SymTable* init_sym_table();
SymTableEntry* lookup(SymTable *t, char *text, int scope, enum Type type);
int insert(SymTable *t, char *name, int scope, int line, enum EntryType type);