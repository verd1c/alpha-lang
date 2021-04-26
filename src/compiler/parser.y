%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <assert.h>
    #include <stddef.h>
    #include <string.h>
    #include "parser.h"
    #include "symbol_table.h"
    #include "intermediate.h"
    #include "tools.h"

    int yylex (void);
    int yyerror (char* yaccProvidedMessage);

    Quad*           quads;
    unsigned        total;
    unsigned int    currQuad;

    char *typeToString[] = {
        "LOCAL_VAR",
        "GLOBAL_VAR",
        "ARGUMENT",

        "USER_FUNC",
        "LIB_FUNC"
    };

    char *opcodeToString[] =  {
        "ASSIGN",  "ADD",    "SUB",
        "MUL",    "DIV",    "MOD",
        "UMINUS", "AND",    "OR",
        "NOT",    "IF_EQ",  "IF_NOTEQ",
        "IF_LESSEQ",  "IF_GREATEREQ",   "IF_LESS",
        "IF_GREATER",    "CALL", "PARAM",
        "RET",   "GETRETVAL",  "FUNCSTART",
        "FUNCEND",    "TABLECREATE", 
        "TABLEGETELEM",   "TABLESETELEM"
    };

    char *_func_name;
    int _func_count = 0;
    int _anon_func_counter = 0;
    int _further_checks = 0;
    int _func_lvalue_check = 0;
    int _in_control = 0;
    long _temp_counter = 0;

    int scope;
    extern int yylineno;
    extern char *yytext;
    extern FILE *yyin;
    SymTable *symTable;
    CallStack *stack;
%}

%start program

%union{
    char*   strValue;
    double  numValue;

    struct SymTableEntry   *symValue;
    struct Expression      *exprValue;

    struct Call            *functionCall;
}

%token<numValue>    NUM
%token<strValue>    ID STRING
%token<strValue>    IF ELSE WHILE FOR FUNCTION RETURN BREAK CONTINUE AND NOT OR LOCAL TRUE FALSE NIL
%token<strValue>    LEFT_BRACE RIGHT_BRACE LEFT_BRACKET RIGHT_BRACKET LEFT_PARENTHESIS RIGHT_PARENTHESIS SEMICOLON COMMA COLON DOUBLE_COLON DOT DOUBLE_DOT
%token<strValue>    EQUALS PLUS MINUS MULT DIV MOD EQUALS_EQUALS NOT_EQUALS PLUS_PLUS MINUS_MINUS GREATER_THAN LESS_THAN GREATER_OR_EQUAL LESS_OR_EQUAL UMINUS

%type<strValue>     member arg idlist 
%type<exprValue>    expression lvalue funcstart const primary term assignexpr elist call funcdef
%type<functionCall> normcall methodcall callsuffix

%right EQUALS
%left OR
%left AND
%nonassoc EQUALS_EQUALS NOT_EQUALS
%nonassoc GREATER_THAN GREATER_OR_EQUAL LESS_THAN LESS_OR_EQUAL
%left PLUS MINUS
%left MULT DIV MOD
%right NOT PLUS_PLUS MINUS_MINUS UMINUS
%left DOT DOUBLE_DOT
%left LEFT_BRACE RIGHT_BRACE
%left LEFT_PARENTHESIS RIGHT_PARENTHESIS


%%

program     :   statements

statements  :   statements statement
                |
                ;

statement   :   expression SEMICOLON
                | ifstmt    
                | whilestmt
                | forstmt  
                | returnstmt            {
                                            if(scope==0 && _in_control==0){
                                                printf("input:%d: error:  Return outside of scope \n", yylineno);
                                            }else{
                                                 //printf("Returns  %d %d\n", e->value.varValue->line, yylineno);
                                                if(_func_count>0 && _in_control >0) _in_control--;
                                                else _in_control=0;
                                            }
                                        }

                | BREAK SEMICOLON       {
                                            if(scope==0 && _in_control==0){
                                                printf("input:%d: error:  Break outside of scope \n", yylineno);
                                            }else{
                                                if(_in_control>0)  _in_control--;
                                            }
                                        }      
                | CONTINUE SEMICOLON    { 
                                            if(scope==0 && _in_control==0){

                                                printf("input:%d: error:  Continue outside of scope \n", yylineno);
                                            }else{
                                                if(_in_control>0)  _in_control--;
                                            }
                                        }
                | block
                | funcdef
                | SEMICOLON
                ;

expression  :   assignexpr                      {

                                                }
                | expression PLUS expression    {
                                                    $$ = sym_expr(new_temp(symTable, scope));
                                                    emit(ADD_I, $$, $1, $3, 0, yylineno);
                                                }
                | expression MINUS expression   {
                                                    $$ = sym_expr(new_temp(symTable, scope));
                                                    emit(SUB_I, $$, $1, $3, 0, yylineno);
                                                }
                | expression MULT expression    {
                                                    $$ = sym_expr(new_temp(symTable, scope));
                                                    emit(MUL_I, $$, $1, $3, 0, yylineno);
                                                }
                | expression DIV expression     {
                                                    $$ = sym_expr(new_temp(symTable, scope));
                                                    emit(DIV_I, $$, $1, $3, 0, yylineno);
                                                }
                | expression MOD expression     {
                                                    $$ = sym_expr(new_temp(symTable, scope));
                                                    emit(MOD_I, $$, $1, $3, 0, yylineno);
                                                }
                | expression GREATER_THAN expression    {

                                                }
                | expression GREATER_OR_EQUAL expression    {

                                                }
                | expression LESS_THAN expression    {

                                                }
                | expression LESS_OR_EQUAL expression    {

                                                }
                | expression EQUALS_EQUALS expression    {

                                                }
                | expression NOT_EQUALS expression    {

                                                }
                | expression AND expression    {

                                                }
                | expression OR expression    {

                                                }
                | term  {
                            $$ = $1;
                        }
                ;

term        :   LEFT_PARENTHESIS expression RIGHT_PARENTHESIS
                | UMINUS expression
                | NOT expression
                | PLUS_PLUS lvalue {               
                            SymTableEntry *e;

                            // Check wether lvalue is a function

                            if($2 && $2->sym && _further_checks){
                                $$ = $2;
                                e = $2->sym;

                                if(e->type == LOCAL_VAR || e->type == GLOBAL_VAR || e->type == ARGUMENT_VAR){

                                    // If it's a variable, check wether we have to create a new one or its referencing an existing one
                                    if(!e->isActive){
                                        //printf("Adding, I found it inactive on line %d %d\n", e->value.varValue->line, yylineno);
                                        if(scope == 0){
                                            $$ = sym_expr(insert(symTable, e->value.varValue->name, scope, yylineno, GLOBAL_VAR));
                                        }else
                                            $$ = sym_expr(insert(symTable, e->value.varValue->name, scope, yylineno, LOCAL_VAR));
                                    }else if(!is_valid(stack, e, scope)){
                                        if(e->value.varValue->scope != scope){
                                            printf("input:%d: error: could not access variable %s\n", yylineno, e->value.varValue->name);
                                        }
                                    }

                                }else{
                                    
                                    // Functions can not be l-values
                                    if(_func_lvalue_check) printf("input:%d: error: on function %s, functions cannot be l-values\n", yylineno, e->value.funcValue->name);
                                }
                            }
                        }
                | lvalue PLUS_PLUS {               
                            SymTableEntry *e;

                            // Check wether lvalue is a function

                            if($1 && $1->sym && _further_checks){
                                $$ = $1;
                                e = $1->sym;

                                if(e->type == LOCAL_VAR || e->type == GLOBAL_VAR || e->type == ARGUMENT_VAR){

                                    // If it's a variable, check wether we have to create a new one or its referencing an existing one
                                    if(!e->isActive){
                                        //printf("Adding, I found it inactive on line %d %d\n", e->value.varValue->line, yylineno);
                                        if(scope == 0){
                                            $$ = sym_expr(insert(symTable, e->value.varValue->name, scope, yylineno, GLOBAL_VAR));
                                        }else
                                            $$ = sym_expr(insert(symTable, e->value.varValue->name, scope, yylineno, LOCAL_VAR));
                                    }else if(!is_valid(stack, e, scope)){
                                        if(e->value.varValue->scope != scope){
                                            printf("input:%d: error: could not access variable %s\n", yylineno, e->value.varValue->name);
                                        }
                                    }

                                }else{
                                    
                                    // Functions can not be l-values
                                    if(_func_lvalue_check) printf("input:%d: error: on function %s, functions cannot be l-values\n", yylineno, e->value.funcValue->name);
                                }
                            }
                        }
                | MINUS_MINUS lvalue {               
                            SymTableEntry *e;

                            // Check wether lvalue is a function

                            if($2 && $2->sym && _further_checks){
                                $$ = $2;
                                e = $2->sym;

                                if(e->type == LOCAL_VAR || e->type == GLOBAL_VAR || e->type == ARGUMENT_VAR){

                                    // If it's a variable, check wether we have to create a new one or its referencing an existing one
                                    if(!e->isActive){
                                        //printf("Adding, I found it inactive on line %d %d\n", e->value.varValue->line, yylineno);
                                        if(scope == 0){
                                            $$ = sym_expr(insert(symTable, e->value.varValue->name, scope, yylineno, GLOBAL_VAR));
                                        }else
                                            $$ = sym_expr(insert(symTable, e->value.varValue->name, scope, yylineno, LOCAL_VAR));
                                    }else if(!is_valid(stack, e, scope)){
                                        if(e->value.varValue->scope != scope){
                                            printf("input:%d: error: could not access variable %s\n", yylineno, e->value.varValue->name);
                                        }
                                    }

                                }else{
                                    
                                    // Functions can not be l-values
                                    if(_func_lvalue_check) printf("input:%d: error: on function %s, functions cannot be l-values\n", yylineno, e->value.funcValue->name);
                                }
                            }
                        }
                | lvalue MINUS_MINUS {               
                            SymTableEntry *e;

                            // Check wether lvalue is a function

                            if($1 && $1->sym && _further_checks){
                                $$ = $1;
                                e = $1->sym;

                                if(e->type == LOCAL_VAR || e->type == GLOBAL_VAR || e->type == ARGUMENT_VAR){

                                    // If it's a variable, check wether we have to create a new one or its referencing an existing one
                                    if(!e->isActive){
                                        //printf("Adding, I found it inactive on line %d %d\n", e->value.varValue->line, yylineno);
                                        if(scope == 0){
                                            $$ = sym_expr(insert(symTable, e->value.varValue->name, scope, yylineno, GLOBAL_VAR));
                                        }else
                                            $$ = sym_expr(insert(symTable, e->value.varValue->name, scope, yylineno, LOCAL_VAR));
                                    }else if(!is_valid(stack, e, scope)){
                                        if(e->value.varValue->scope != scope){
                                            printf("input:%d: error: could not access variable %s\n", yylineno, e->value.varValue->name);
                                        }
                                    }

                                }else{
                                    
                                    // Functions can not be l-values
                                    if(_func_lvalue_check) printf("input:%d: error: on function %s, functions cannot be l-values\n", yylineno, e->value.funcValue->name);
                                }
                            }
                        }
                | primary   {
                                $$ = $1;
                            }
                ;

assignexpr  :   lvalue 
                        {               
                            SymTableEntry *e;

                            // Check wether lvalue is a function

                            if($1 && $1->sym && _further_checks){
                                e = $1->sym;

                                if(e->type == LOCAL_VAR || e->type == GLOBAL_VAR || e->type == ARGUMENT_VAR){

                                    // If it's a variable, check wether we have to create a new one or its referencing an existing one
                                    if(!e->isActive){
                                        //printf("Adding, I found it inactive on line %d %d\n", e->value.varValue->line, yylineno);
                                        if(scope == 0){
                                            $1 = sym_expr(insert(symTable, e->value.varValue->name, scope, yylineno, GLOBAL_VAR));
                                        }else
                                            $1 = sym_expr(insert(symTable, e->value.varValue->name, scope, yylineno, LOCAL_VAR));
                                    }else if(!is_valid(stack, e, scope)){
                                        if(e->value.varValue->scope != scope && e->type != GLOBAL_VAR){
                                            printf("input:%d: error: could not access variable %s\n", yylineno, e->value.varValue->name);
                                            $1 = NULL;
                                        }
                                    }

                                }else{
                                    $1 = NULL;
                                    // Functions can not be l-values
                                    if(_func_lvalue_check) printf("input:%d: error: on function %s, functions cannot be l-values\n", yylineno, e->value.funcValue->name);
                                }
                            }
                        }
EQUALS expression   {
                        emit(ASSIGN_I, $1, $4, NULL, 0, yylineno);
                    }

primary     :   lvalue  {               
                            SymTableEntry *e;

                            // Check wether lvalue is a function

                            if($1 && $1->sym && _further_checks){
                                $$ = $1;
                                e = $1->sym;

                                if(e->type == LOCAL_VAR || e->type == GLOBAL_VAR || e->type == ARGUMENT_VAR){

                                    // If it's a variable, check wether we have to create a new one or its referencing an existing one
                                    if(!e->isActive){
                                        //printf("Adding, I found it inactive on line %d %d\n", e->value.varValue->line, yylineno);
                                        if(scope == 0){
                                            $$ = sym_expr(insert(symTable, e->value.varValue->name, scope, yylineno, GLOBAL_VAR));
                                        }else
                                            $$ = sym_expr(insert(symTable, e->value.varValue->name, scope, yylineno, LOCAL_VAR));
                                    }else if(!is_valid(stack, e, scope)){
                                        if(e->value.varValue->scope != scope && e->type != GLOBAL_VAR){
                                            printf("input:%d: error: could not access variable %s\n", yylineno, e->value.varValue->name);
                                        }
                                    }

                                }
                            }
                        }
                | call {$$ = $1;}
                | objectdef
                | LEFT_PARENTHESIS funcdef RIGHT_PARENTHESIS {$$ = $2;}
                | const {
                            $$ = $1;
                        }
                ;

lvalue      :   ID                          {
                                                SymTableEntry *e;
                                                
                                                // Check if active variables exist in any scope from current and down
                                                if((e = lookup_variable(symTable, $1, scope)) == NULL){
                                                    
                                                    // If not, insert the variable
                                                    if(scope == 0)
                                                        $$ = sym_expr(insert(symTable, $1, scope, yylineno, GLOBAL_VAR));
                                                    else
                                                        $$ = sym_expr(insert(symTable, $1, scope, yylineno, LOCAL_VAR));
                                                }else{

                                                    // If exists, set value for further checks
                                                    //printf("Setting valuue for further checks on-> %s %s\n", e->value.varValue->name, typeToString[e->type]);
                                                    $$ = sym_expr(e);
                                                }
                                                _further_checks = 1;
                                                _func_lvalue_check = 1;
                                            }
                | LOCAL ID                  {
                                                SymTableEntry *e;
                                                
                                                // Check current scope
                                                if((e = lookup_no_type(symTable, $2, scope)) == NULL){
                                                    if((e = lookup(symTable, $2, 0, LIB_FUNC)) == NULL){
                                                        if(scope == 0){
                                                            $$ = sym_expr(insert(symTable, $2, scope, yylineno, GLOBAL_VAR));
                                                        }else{
                                                            $$ = sym_expr(insert(symTable, $2, scope, yylineno, LOCAL_VAR));
                                                        }
                                                    }else{
                                                        printf("input:%d: error: local symbol %s is attempting to shadow a library function\n", yylineno, $2);
                                                        $$ = NULL;
                                                    }
                                                }else{
                                                    // If exists, set reference
                                                    $$ = sym_expr(e);
                                                }
                                                _further_checks = 1;
                                                _func_lvalue_check = 1;
                                            }
                | DOUBLE_COLON ID           {
                                                SymTableEntry *e;
                                                
                                                // Check globals
                                                if((e = lookup_no_type(symTable, $2, 0)) == NULL){
                                                    printf("input:%d: error: global reference %s in line %d not found\n", yylineno, $2, yylineno);
                                                    $$ = NULL;
                                                }else{
                                                    // If exists, set reference
                                                    $$ = sym_expr(e);
                                                }
                                                _further_checks = 0;
                                                _func_lvalue_check = 1;
                                            }
                | member                    {
                                            _func_lvalue_check = 0;}
                ;

member      :   lvalue DOT ID   {}
                | lvalue LEFT_BRACE expression RIGHT_BRACE  {}
                | call DOT ID 
                | call LEFT_BRACE expression RIGHT_BRACE
                ;

call        :   call LEFT_PARENTHESIS elist RIGHT_PARENTHESIS   {
                                                                    $$ = make_call(symTable, scope, $1, reverse_elist(&$3));
                                                                }
                | lvalue callsuffix {
                                        SymTableEntry* e;

                                        Call *c;
                                        SymTableEntry *e1;
                                        e1 = $1->sym;
                                        c = $2;
                                        c->name = strdup(e1->value.funcValue->name);
                                        print_call(c);

                                        //printf("Usage of %s as call in %d\n", e->value.funcValue->name, yylineno);

                                        // Check wether lvalue is a function

                                        if($1 && $1->sym){
                                            e = $1->sym;

                                            if(e->type == LOCAL_VAR || e->type == GLOBAL_VAR || e->type == ARGUMENT_VAR){

                                                // If it's a variable, check wether we have to create a new one or its referencing an existing one
                                                if(!e->isActive){
                                                    //printf("Adding, I found it inactive on line %d %d\n", e->value.varValue->line, yylineno);
                                                    if(scope == 0){
                                                        insert(symTable, e->value.varValue->name, scope, yylineno, GLOBAL_VAR);
                                                    }else
                                                        insert(symTable, e->value.varValue->name, scope, yylineno, LOCAL_VAR);
                                                }else if(!is_valid(stack, e, scope)){
                                                    if(e->value.varValue->scope != scope){
                                                        printf("input:%d: error: could not access variable %s\n", yylineno, e->value.varValue->name);
                                                    }
                                                }

                                                
                                            }
                                        }

                                        $$ = make_call(symTable, scope, $1, reverse_elist(&$2->elist));
                                    }
                | LEFT_PARENTHESIS funcdef RIGHT_PARENTHESIS LEFT_PARENTHESIS elist RIGHT_PARENTHESIS {
                                                        Expr *g = $2;
                                                        $$ = make_call(symTable, scope, g, reverse_elist(&$5));
                                                                                                        }
                ;
            
callsuffix  :   normcall        {$$ = $1;}
                | methodcall    {$$ = $1;}
                ;

normcall    :   LEFT_PARENTHESIS elist RIGHT_PARENTHESIS    {
                                                                $$ = function_call(FALSE, NULL, scope, $2);
                                                            }

methodcall  :   DOUBLE_DOT ID LEFT_PARENTHESIS elist RIGHT_PARENTHESIS  {
                                                                            $$ = function_call(TRUE, $2, scope, $4);
                                                                        }

elist       :   expression                  {
                                                $$ = $1;
                                            }
                | expression COMMA elist    {
                                                Expr *head = $1;

                                                head->next = $3;

                                                $$ = head;
                                            }
                |
                ;
        
objectdef   :    LEFT_BRACE elist RIGHT_BRACE
                | LEFT_BRACE indexed RIGHT_BRACE
                ;

indexed     :   indexedelem
                | indexedelem COMMA indexed
                ;
                
indexedelem :   LEFT_BRACKET expression COLON expression RIGHT_BRACKET
                ;

block       :   blockstart blockend 
                | blockstart blockstmt blockend
                ;

blockstart  :   LEFT_BRACKET    {
                                    scope++; // Up scope
                                }

blockend    :   RIGHT_BRACKET   {
                                    // Scope down and hide old symbols
                                    scope_down(symTable);
                                }

blockstmt   :   statement
                | statement blockstmt
                ;

funcdef     :   funcstart LEFT_PARENTHESIS {scope++;} idlist RIGHT_PARENTHESIS  {    scope--; 
                                                                                    _func_count++;

                                                                                    // Push function to stack
                                                                                    if($1 && $1->sym)
                                                                                        push(stack, $1->sym);
                                                                                    //printCallStack(stack, yylineno);
                                                                                } 
                                                                                    
                block   {
                            _func_count--;

                            // Removing function from stack
                            if($1 && $1->sym)
                                pop(stack);

                            emit(FUNCEND_I, 0, 0, 0, 0, yylineno);
                            $$ = $1;
                        }
                ;

funcstart   :   FUNCTION ID {
                                SymTableEntry *e;

                                _func_name = $2;

                                // Search for libfunc
                                if((e = function_lookup(symTable, $2, scope)) == NULL){

                                    // Check libfuncs
                                    if((e = lookup(symTable, $2, 0, LIB_FUNC)) != NULL){
                                        printf("input:%d: error: shadowing of library function %s is not permitted\n", yylineno, $2);
                                    }else{
                                        $$ = sym_expr(insert(symTable, $2, scope, yylineno, USER_FUNC));
                                    }
                                }else{
                                    if(e->type == USER_FUNC)
                                        printf("input:%d: error: duplicate user function %s, first defined in line %d\n", yylineno, $2, e->value.funcValue->line);
                                    else if(e->type == LIB_FUNC)
                                        printf("input:%d: error: shadowing of library function %s is not permitted\n", yylineno, $2);
                                    else
                                        printf("input:%d: error: function %s has conflicting type with variable %s first defined in line %d\n", yylineno, $2, e->value.varValue->name, e->value.varValue->line);
                                    $$ = NULL;
                                }

                                emit(FUNCSTART_I, 0, 0, 0, 0, yylineno);
                            }
                | FUNCTION  {  
                                char *s;
                                s = (char *)malloc(8 * sizeof(char));
                                _func_name = s;
     
                                sprintf(s, "$%d", _anon_func_counter);

                                $$ = sym_expr(insert(symTable, s, scope, yylineno, USER_FUNC));

                                _anon_func_counter++;

                                emit(FUNCSTART_I, 0, 0, 0, 0, yylineno);
                            }

const       :   NUM         {
                                $<exprValue>$ = num_expr($1);
                            }
                | STRING    {
                                $$ = string_expr($1);
                            }
                | NIL       {
                                $$ = nil_expr();
                            }
                | TRUE      {
                                $$ = bool_expr(1);
                            }
                | FALSE     {
                                $$ = bool_expr(0);
                            }

idlist      :   arg
                | arg COMMA idlist
                | {}
                ;

arg         :   ID  {
                        SymTableEntry *e, *a;

                        if((e = lookup(symTable, $1, 0, LIB_FUNC)) != NULL){
                            printf("input:%d: error: argument %s in function %s attempting to shadow a library function\n", yylineno, $1, _func_name);
                        }else{
                            if((e = lookup(symTable, _func_name, scope - 1, USER_FUNC)) != NULL){
                                if((a = lookup_active(symTable, $1, scope)) == NULL){
                                    insert(symTable, $1, scope, yylineno, ARGUMENT_VAR);
                                }else{
                                    printf("input:%d: error: duplicate argument %s in function %s\n", yylineno, $1, e->value.funcValue->name);
                                }
                            } 
                        }                         
                    }

ifstmt      :   ifstart expression RIGHT_PARENTHESIS statement
                | ifstart expression RIGHT_PARENTHESIS statement ELSE statement
                ;

ifstart     :   IF LEFT_PARENTHESIS { _in_control++; printf("line: %d: if statement\n", yylineno); }

whilestmt   :   whilestart expression RIGHT_PARENTHESIS statement

whilestart  :   WHILE LEFT_PARENTHESIS { _in_control++; printf("line: %d: while statement\n", yylineno);}

forstmt     :   forstart elist SEMICOLON expression SEMICOLON elist RIGHT_PARENTHESIS statement

forstart    :   FOR LEFT_PARENTHESIS { _in_control++; printf("line: %d: for statement\n", yylineno);} 

returnstmt  :   RETURN SEMICOLON
                | RETURN expression SEMICOLON
                ;



%%

int yyerror(char *yaccProvidedMessage){
    printf("alpha: %s\n", yaccProvidedMessage);
    return 1;
}

void printSymTable(SymTable *symTable){
    SymTableEntry *e;
    int i;

    printf("----------------------------------------------------------------\n");
    printf("Symbol Table  [Name                ] [Line] [Scope] [Type      ]\n");
    printf("----------------------------------------------------------------\n");
    for(i = 0; i < SIZE; i++){
        e = symTable->table[i];

        if(e == NULL) continue;

        while(e != NULL){
            if(e->type == LOCAL_VAR || e->type == GLOBAL_VAR || e->type == ARGUMENT_VAR){
                printf("SymTableEntry [%-20s] [%-4d] [%-5d] [%-10s]\n", e->value.varValue->name, e->value.varValue->line, e->value.varValue->scope, typeToString[e->type]);
            }else{
                printf("SymTableEntry [%-20s] [%-4d] [%-5d] [%-10s]\n", e->value.funcValue->name, e->value.funcValue->line, e->value.funcValue->scope, typeToString[e->type]);
            }

            e = e->nextEntry;
        }
    }
    printf("----------------------------------------------------------------\n");
}

void printByScope(SymTable *symTable){
    SymTableEntry *scopeIter, *e;
    int scope = 0;

    printf("----------------------------------------------------------------\n");
    printf("Symbol Table  [Name                ] [Line] [Scope] [Type      ]\n");
    printf("----------------------------------------------------------------\n");
    scopeIter = symTable->scopeChain;

    while(scopeIter != NULL){
        e = scopeIter;
        printf("|                          SCOPE %d                             |\n", scope);
        printf("----------------------------------------------------------------\n");

        while(e != NULL){

            if(e->type == LOCAL_VAR || e->type == GLOBAL_VAR || e->type == ARGUMENT_VAR){
                printf("SymTableEntry [%-20s] [%-4d] [%-5d] [%-10s]\n", e->value.varValue->name, e->value.varValue->line, e->value.varValue->scope, typeToString[e->type]);
            }else{
                printf("SymTableEntry [%-20s] [%-4d] [%-5d] [%-10s]\n", e->value.funcValue->name, e->value.funcValue->line, e->value.funcValue->scope, typeToString[e->type]);
            }
            
            e = e->nextInScope;
        }

        scopeIter = scopeIter->nextScope;
        if(scopeIter != NULL){
            if(scopeIter->type == LOCAL_VAR || scopeIter->type == GLOBAL_VAR || scopeIter->type == ARGUMENT_VAR){
                scope = scopeIter->value.varValue->scope;
            }else{
                scope = scopeIter->value.funcValue->scope;
            }
        }


        printf("----------------------------------------------------------------\n");
    }

    return;
}

void printQuads(void){
    Quad q;
    Expr *e;
    int i;

    printf("-------------------------------\n");
    printf("Printing Quads\n");
    printf("-------------------------------\n");
    for(i = 0; i < currQuad; i++){
        q = quads[i];
        printf("%-10s ", opcodeToString[q.op]);

        // print result
        e = q.result;
        if(e){
            if(e->type == CONSTNUM_E){
                printf(" %-3f ", e->numConst);
            }else if(e->type == CONSTBOOL_E){
                if(e->boolConst == 0)
                    printf(" %-5s ", "false");
                else
                    printf(" %-5s ", "true");
            }else{
                if(e->sym->type == LOCAL_VAR || e->sym->type == GLOBAL_VAR || e->sym->type == ARGUMENT_VAR)
                    printf(" %-10s ", e->sym->value.varValue->name);
                else
                    printf(" %-10s ", e->sym->value.funcValue->name);
            }
        }

        // print arg1
        e = q.arg1;
        if(e){
            if(e->type == CONSTNUM_E){
                printf(" %-3f ", e->numConst);
            }else if(e->type == CONSTBOOL_E){
                if(e->boolConst == 0)
                    printf(" %-5s ", "false");
                else
                    printf(" %-5s ", "true");
            }else{
                if(e->sym->type == LOCAL_VAR || e->sym->type == GLOBAL_VAR || e->sym->type == ARGUMENT_VAR)
                    printf(" %-10s ", e->sym->value.varValue->name);
                else
                    printf(" %-10s ", e->sym->value.funcValue->name);
            }
        }

        // print arg2
        e = q.arg2;
        if(e){
            if(e->type == CONSTNUM_E){
                printf(" %-3f ", e->numConst);
            }else if(e->type == CONSTBOOL_E){
                if(e->boolConst == 0)
                    printf(" %-5s ", "false");
                else
                    printf(" %-5s ", "true");
            }else{
                if(e->sym->type == LOCAL_VAR || e->sym->type == GLOBAL_VAR || e->sym->type == ARGUMENT_VAR)
                    printf(" %-10s ", e->sym->value.varValue->name);
                else
                    printf(" %-10s ", e->sym->value.funcValue->name);
            }
        }

        printf("\n");
    }



}

int main(int argc, char **argv){
    if(argc > 1){
        if(!(yyin = fopen(argv[1], "r"))){
            printf("AAA\n");
            exit(0);
        }
    }else{
        yyin = stdin;

    }

    init_quads();
    symTable = init_sym_table();
    stack = init_call_stack();
    yyparse();

    //printSymTable(symTable);
    printByScope(symTable);
    printQuads();

    return 0;
}
