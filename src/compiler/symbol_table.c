#include "symbol_table.h"

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

    symTable->length = 0;
    symTable->scopeChain = NULL;

    init_lib_funcs(symTable);

    return symTable;
}

void setNextScopes(SymTable *t, int scope, SymTableEntry *e){
    SymTableEntry *iter;
    int iterScope;

    //asm __volatile__ (".byte 0xc3");

    iter = t->scopeChain;

    // Get iter scope
    if(iter->type == LOCAL_VAR || iter->type == GLOBAL_VAR || iter->type == ARGUMENT_VAR)
        iterScope = iter->value.varValue->scope;
    else
        iterScope = iter->value.funcValue->scope;

    while(iter != NULL && iterScope < scope){

        iter = iter->nextScope;
        
        if(iter != NULL){
            // Get iter scope
            if(iter->type == LOCAL_VAR || iter->type == GLOBAL_VAR || iter->type == ARGUMENT_VAR)
                iterScope = iter->value.varValue->scope;
            else
                iterScope = iter->value.funcValue->scope;
        }
    }

    if(iter == NULL){
        printf("Wtf?\n");
        exit(0);
    }

    // Loop all and set nextScope
    iter->nextScope = e;
    while(iter->nextInScope != NULL){
        iter->nextScope = e;
        iter = iter->nextInScope;
    }

    return;
}

void addToScopeChain(SymTable *t, SymTableEntry *e){
    SymTableEntry *iter, *prev;
    int entryScope, iterScope, prevScope = -1;

    if(t->scopeChain == NULL){
        t->scopeChain = e;
        //asm __volatile__ (".byte 0xc3");
        return;
    }

    // Get entry scope
    if(e->type == LOCAL_VAR || e->type == GLOBAL_VAR || e->type == ARGUMENT_VAR)
        entryScope = e->value.varValue->scope;
    else
        entryScope = e->value.funcValue->scope;
    

    iter = t->scopeChain;

    // Get iter scope
    if(iter->type == LOCAL_VAR || iter->type == GLOBAL_VAR || iter->type == ARGUMENT_VAR)
        iterScope = iter->value.varValue->scope;
    else
        iterScope = iter->value.funcValue->scope;

    prev = NULL;
    while(iter != NULL && iterScope < entryScope){
        prev = iter;
        iter = iter->nextScope;
        // Get iter scope
        if(iter != NULL){
            if(iter->type == LOCAL_VAR || iter->type == GLOBAL_VAR || iter->type == ARGUMENT_VAR)
                iterScope = iter->value.varValue->scope;
            else
                iterScope = iter->value.funcValue->scope;
        }
            
    }

    // Get previous scope
    if(prev != NULL){
        if(prev->type == LOCAL_VAR || prev->type == GLOBAL_VAR || prev->type == ARGUMENT_VAR)
            prevScope = prev->value.varValue->scope;
        else
            prevScope = prev->value.funcValue->scope;
    }
    if(iterScope > entryScope){
        if(prevScope != -1){
            setNextScopes(t, prevScope, e);
            e->nextScope = iter;
            return;
        }
    }

    if(iter == NULL){
        // Create scope
        setNextScopes(t, iterScope, e);
        return;
    }


    while(iter->nextInScope != NULL){
        iter = iter->nextInScope;
    }

    iter->nextInScope = e;
    e->nextScope = iter->nextScope;
    return;

}

/*
* 0 fail 1 successs
*
*/
SymTableEntry *insert(SymTable *t, char *name, int scope, int line, enum EntryType type){
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
        t->table[h]->isActive = 1;
        addToScopeChain(t, t->table[h]);
        return t->table[h];
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
        iter->nextEntry->isActive = 1;
        addToScopeChain(t, iter->nextEntry);
        return iter->nextEntry;
    }

    return NULL;
}


SymTableEntry* lookup(SymTable *t, char *name, int scope, enum EntryType type){
    SymTableEntry *iter;
    int h;

    h = hash(name);

    iter = t->table[h];

    while(iter != NULL){

        if(iter->type == LOCAL_VAR || iter->type == GLOBAL_VAR || iter->type == ARGUMENT_VAR){
            if(strcmp(iter->value.varValue->name, name) == 0 && iter->value.varValue->scope == scope && iter->type == type)
                return iter;
        }else{
            if(strcmp(iter->value.funcValue->name, name) == 0 && iter->value.funcValue->scope == scope && iter->type == type)
                return iter;
        }

        iter = iter->nextEntry;
    }

    return NULL;
}

SymTableEntry* lookup_no_type(SymTable *t, char *name, int scope){
    SymTableEntry *iter;
    int h;

    h = hash(name);

    iter = t->table[h];

    while(iter != NULL){

        if(iter->type == LOCAL_VAR || iter->type == GLOBAL_VAR || iter->type == ARGUMENT_VAR){
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

SymTableEntry* lookup_active(SymTable *t, char *name, int scope){
    SymTableEntry *iter;
    int h;

    h = hash(name);

    iter = t->table[h];

    while(iter != NULL){

        if(iter->type == LOCAL_VAR || iter->type == GLOBAL_VAR || iter->type == ARGUMENT_VAR){
            if(strcmp(iter->value.varValue->name, name) == 0 && iter->value.varValue->scope == scope && iter->isActive)
                return iter;
        }else{
            if(strcmp(iter->value.funcValue->name, name) == 0 && iter->value.funcValue->scope == scope && iter->isActive)
                return iter;
        }

        iter = iter->nextEntry;
    }

    return NULL;
}

SymTableEntry* lookup_variable(SymTable *t, char *name, int scope){
    SymTableEntry *iter;
    int curScope;

    for(curScope = scope; curScope >= 0; curScope--){
        if((iter = lookup_active(t, name, curScope)) != NULL)
            return iter;
    }

    return NULL;
}

void hide(SymTable *t, int scope){
    SymTableEntry *iter;
    int iterScope;

    iter = t->scopeChain;

    // Get iter scope
    if(iter->type == LOCAL_VAR || iter->type == GLOBAL_VAR || iter->type == ARGUMENT_VAR)
        iterScope = iter->value.varValue->scope;
    else
        iterScope = iter->value.funcValue->scope;

    while(iter != NULL && iterScope != scope){
        iter = iter->nextScope;
        // Get iter scope
        if(iter != NULL){
            if(iter->type == LOCAL_VAR || iter->type == GLOBAL_VAR || iter->type == ARGUMENT_VAR)
                iterScope = iter->value.varValue->scope;
            else
                iterScope = iter->value.funcValue->scope;
        }
    }


    while(iter != NULL){
        if(iter->isActive != 0)
            iter->isActive = 0;
        
        iter = iter->nextInScope;
    }

    return;
}

void scope_down(SymTable *t){
    hide(t, scope);
    scope--;
    return;    
}

SymTableEntry *function_lookup(SymTable *t, char *name, int scope){
    SymTableEntry *iter;
    int h;

    h = hash(name);
    iter = t->table[h];
    while(iter != NULL){
        if(iter->type == LOCAL_VAR || iter->type == GLOBAL_VAR || iter->type == ARGUMENT_VAR){
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

void alpha_error(char *error, int line){
    fprintf(stderr, "input:%d: error: %s\n", line, error);
    return;
}