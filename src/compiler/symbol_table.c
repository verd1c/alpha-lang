#include "symbol_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "stack.h"

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
        e->nextInScope = NULL;
        e->nextScope = NULL;
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

        if (t->table[h] == NULL) {
            printf("Memory error\n");
            exit(0);
        }

        if(type == LOCAL_VAR || type == GLOBAL_VAR || type == ARGUMENT_VAR){
            t->table[h]->value.varValue = (Variable*)malloc(sizeof(Variable));

            if (!t->table[h]->value.varValue) {
                printf("Memory error\n");
                exit(0);
            }

            t->table[h]->value.varValue->name = name;
            t->table[h]->value.varValue->scope = scope;
            t->table[h]->value.varValue->line = line;
        }else{
            t->table[h]->value.funcValue = (Function*)malloc(sizeof(Function));

            if (!t->table[h]->value.funcValue) {

            }

            t->table[h]->value.funcValue->name = name;
            t->table[h]->value.funcValue->scope = scope;
            t->table[h]->value.funcValue->line = line;
        }

        t->table[h]->type = type;

        t->table[h]->nextEntry = NULL;
        t->table[h]->isActive = 1;
        // Set nulls (win)
        t->table[h]->nextInScope = NULL;
        t->table[h]->nextScope = NULL;
        addToScopeChain(t, t->table[h]);
        return t->table[h];
    }else{
        iter = t->table[h];

        while(iter->nextEntry != NULL){
            iter = iter->nextEntry;
        }

        iter->nextEntry = (SymTableEntry*)malloc(sizeof(SymTableEntry));

        if (iter->nextEntry == NULL) {
            printf("Memory error\n");
            exit(0);
        }

        if(type == LOCAL_VAR || type == GLOBAL_VAR || type == ARGUMENT_VAR){
            iter->nextEntry->value.varValue = (Variable*)malloc(sizeof(Variable));

            if (!iter->nextEntry->value.varValue) {
                printf("Memory error\n");
                exit(0);
            }

            iter->nextEntry->value.varValue->name = name;
            iter->nextEntry->value.varValue->scope = scope;
            iter->nextEntry->value.varValue->line = line;
        }else{
            iter->nextEntry->value.funcValue = (Function*)malloc(sizeof(Function));

            if (!iter->nextEntry->value.funcValue) {
                printf("Memory error\n");
                exit(0);
            }

            iter->nextEntry->value.funcValue->name = name;
            iter->nextEntry->value.funcValue->scope = scope;
            iter->nextEntry->value.funcValue->line = line;
        }

        iter->nextEntry->type = type;

        iter->nextEntry->nextEntry = NULL;
        iter->nextEntry->isActive = 1;
        iter->nextEntry->nextInScope = NULL;
        iter->nextEntry->nextScope = NULL;
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

SymTableEntry* lookup_temp(SymTable *t, int scope) {
    SymTableEntry *iter, *maxTemp;
    int iterScope, max, num;
    char *name;

    iter = t->scopeChain;

    // Get iter scope
    if (iter->type == LOCAL_VAR || iter->type == GLOBAL_VAR || iter->type == ARGUMENT_VAR)
        iterScope = iter->value.varValue->scope;
    else
        iterScope = iter->value.funcValue->scope;

    while (iter && iterScope < scope) {
        iter = iter->nextScope;

        // Get iter scope
        if (iter->type == LOCAL_VAR || iter->type == GLOBAL_VAR || iter->type == ARGUMENT_VAR)
            iterScope = iter->value.varValue->scope;
        else
            iterScope = iter->value.funcValue->scope;
    }

    if (!iter)
        return NULL;

    maxTemp = NULL;
    max = -1;
    while (iter) {

        if (iter->type == LOCAL_VAR || iter->type == GLOBAL_VAR || iter->type == ARGUMENT_VAR)
            name = iter->value.varValue->name;
        else
            name = iter->value.funcValue->name;
        
        if (strlen(name) > 1 && name[0] == '^') {
            num = atoi(name + 1);

            // ^1 -> ^2 -> ^3 
            if (num > max) {
                max = num;
                maxTemp = iter;
            }
        }

        iter = iter->nextInScope;
    }
    
    return maxTemp;
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

int is_valid(Stack *s, SymTableEntry *target, int curScope){
    int targetScope, i, j;

    if(s->top == -1)
        return 1;

    if(target->type == LOCAL_VAR || target->type == GLOBAL_VAR || target->type == ARGUMENT_VAR)
        targetScope = target->value.varValue->scope;
    else
        targetScope = target->value.funcValue->scope;
    
    for(i = targetScope; i < curScope; i++){
        for(j = s->size - 1; j >= s->top; j--){
            if(((SymTableEntry*)s->stack[j])->value.funcValue->scope == i)
                return 0;
        }
    }
    
    return 1;
}

void alpha_error(char *error, int line){
    fprintf(stderr, "input:%d: error: %s\n", line, error);
    return;
}

CallStack *init_call_stack(){
    CallStack *stack;
    
    stack = (CallStack*)malloc(sizeof(CallStack));
    stack->size = CALL_STACK_SIZE;
    stack->top = -1;

    return stack;
}

void push_c(CallStack *s, SymTableEntry *e){
    if(s->top == -1){ // Empty
        s->stack[s->size - 1] = e;
        s->top = s->size - 1;
    }else if(s->top == 0){ // Full
        printf("Error: Call Stack Full\n");
    }else{
        s->stack[s->top - 1] = e;
        s->top = s->top - 1;
    }
    return;
}

void pop_c(CallStack *s){
    if(s->top == -1){
        printf("Empty stack\n");
    }else{
        if(s->top == s->size - 1){
            s->top = -1;
        }else{
            s->top = s->top + 1;
        }
    }
}

OffsetStack * init_offset_stack(){
	OffsetStack *node = malloc(sizeof(OffsetStack));
	node->next = NULL;

	return node;
}

SymTableEntry *top_c(CallStack *s){
    if (s->top == -1) {
        printf("Empty stack\n");
        return (void*)0;
    }else {
        if (s->top == s->size - 1) {

            SymTableEntry *tmp = s->stack[s->top];
            s->top = -1;
            return tmp;
        }else {

            SymTableEntry *tmp = s->stack[s->top];
            s->top = s->top + 1;
            return tmp;
        }
    }
}

scopespace_t push_offset(OffsetStack *s, scopespace_t scspace){
    OffsetStack *tmp, *new = malloc(sizeof(OffsetStack));

    tmp = s;
    while(tmp->next) {
        tmp = tmp->next;
    }
    tmp->next = new;
    new->next = NULL;
    new->scspace_off = scspace;

    return s->scspace_off;
}

scopespace_t pop_and_top_offset(OffsetStack *s){
    if(s->next = NULL) printf("Empty Offset Stack\n");
    else {
        OffsetStack *tmp, *prev;
        tmp = s;
        prev = tmp;

        while(tmp->next){
            prev = tmp;
            tmp = tmp->next;
        }
        prev->next = NULL;
        return tmp->scspace_off;
    }
}

char *typeToStringA[] = {
        "LOCAL_VAR",
        "GLOBAL_VAR",
        "ARGUMENT",

        "USER_FUNC",
        "LIB_FUNC"
    };

void printCallStack(CallStack *s, int line){
    int i;
    SymTableEntry *e;
    printf("---------------------------------------------------\nSTACK AT LINE %d\n", line);
    printf("Size: %d\n", s->size);
    printf("Top: %d\n", s->top);
    if(s->top == -1)
        printf("Stack is empty\n");
    for(i = s->size - 1; i >= s->top; i--){
        e = s->stack[i];
        if(e->type == LOCAL_VAR || e->type == GLOBAL_VAR || e->type == ARGUMENT_VAR){
            printf("SymTableEntry [%-20s] [%-4d] [%-5d] [%-10s]\n", e->value.varValue->name, e->value.varValue->line, e->value.varValue->scope, typeToStringA[e->type]);
        }else{
            printf("SymTableEntry [%-20s] [%-4d] [%-5d] [%-10s]\n", e->value.funcValue->name, e->value.funcValue->line, e->value.funcValue->scope, typeToStringA[e->type]);
        }
    }
    printf("---------------------------------------------------\n");
}