#include "struct.h"

static int hash(char *text){
    size_t i;
    unsigned int sum = 0;

    for(i = 0; i < strlen(text); i++){
        sum = sum * HASH_MULTIPLIER + text[i];
    }

    return sum % SIZE;
}

/*
* Initialize library functions
*/
void init_lib_funcs(SymTable *symTable){
    insert(symTable, "print", 0, 0, LIB_FUNC);
    insert(symTable, "input", 0, 0, LIB_FUNC);
    insert(symTable, "objectmemberkeys", 0, 0, LIB_FUNC);
    insert(symTable, "objecttotalmembers", 0, 0, LIB_FUNC);
    insert(symTable, "objectcopy", 0, 0, LIB_FUNC);
    insert(symTable, "totalarguments", 0, 0, LIB_FUNC);
    insert(symTable, "argument", 0, 0, LIB_FUNC);
    insert(symTable, "typeof", 0, 0, LIB_FUNC);
    insert(symTable, "strtonum", 0, 0, LIB_FUNC);
    insert(symTable, "sqrt", 0, 0, LIB_FUNC);
    insert(symTable, "cos", 0, 0, LIB_FUNC);
    insert(symTable, "sin", 0, 0, LIB_FUNC);
    return;
}

SymTable* init_sym_table(){
    SymTable *symTable;
    int i;

    symTable = (struct SymTable_t*)malloc(sizeof(struct SymTable_t));
    for(i = 0; i < SIZE; i++){
        symTable->table[i] = NULL;
    }
    
    init_lib_funcs(symTable);

    symTable->length = 0;

    return symTable;
}

/*
* 0 fail 1 successs
*
*/
int insert(SymTable *t, char *name, int scope, int line, enum EntryType type){
    SymTableEntry *iter;
    int h;

    h = hash(name);

    if(t->table[h] == NULL){
        t->table[h] = (SymTableEntry*)malloc(sizeof(SymTableEntry));

        if(type == LOCAL_VAR || type == GLOBAL_VAR || type == ARGUMENT_VAR){
            t->table[h]->value.varValue = (Variable*)malloc(sizeof(Variable));
            t->table[h]->value.varValue->name = name;
            t->table[h]->value.varValue->scope = scope;
            t->table[h]->value.varValue->line = line;
        }else{
            t->table[h]->value.funcValue = (Function*)malloc(sizeof(Function));
            t->table[h]->value.funcValue->name = name;
            t->table[h]->value.funcValue->scope = scope;
            t->table[h]->value.funcValue->line = line;
        }

        t->table[h]->type = type;

        t->table[h]->nextEntry = NULL;
    }else{
        iter = t->table[h];

        while(iter->nextEntry != NULL){
            iter = iter->nextEntry;
        }

        iter->nextEntry = (SymTableEntry*)malloc(sizeof(SymTableEntry));

        if(type == LOCAL_VAR || type == GLOBAL_VAR || type == ARGUMENT_VAR){
            iter->nextEntry->value.varValue = (Variable*)malloc(sizeof(Variable));
            iter->nextEntry->value.varValue->name = name;
            iter->nextEntry->value.varValue->scope = scope;
            iter->nextEntry->value.varValue->line = line;
        }else{
            iter->nextEntry->value.funcValue = (Function*)malloc(sizeof(Function));
            iter->nextEntry->value.funcValue->name = name;
            iter->nextEntry->value.funcValue->scope = scope;
            iter->nextEntry->value.funcValue->line = line;
        }

        iter->nextEntry->type = type;

        iter->nextEntry->nextEntry = NULL;
    }

    return 1;
}


SymTableEntry* lookup(SymTable *t, char *name, int scope, enum Type type){
    SymTableEntry *iter;
    int h;

    h = hash(name);

    iter = t->table[h];

    while(iter != NULL){

        if(type == VAR){
            if(strcmp(iter->value.varValue->name, name) == 0 && iter->value.varValue->scope == scope)
                return iter;
        }else{
            if(strcmp(iter->value.funcValue->name, name) == 0 && iter->value.funcValue->scope == scope)
                return iter;
        }

        iter = iter->nextEntry;
    }

    return NULL;
}