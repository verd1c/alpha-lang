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
    #include "stack.h"
    #include "consts.h"
    #include "binary.h"

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
        "TABLEGETELEM",   "TABLESETELEM", "JUMP"
    };

    char *_func_name;
    int _func_count = 0;
    int _anon_func_counter = 0;
    int _further_checks = 0;
    int _func_lvalue_check = 0;
    int _in_control = 0;
    long _temp_counter = 0;
    int _func_locals = 0;

    int _valid_comp = 1;

    int scope;
    extern int yylineno;
    extern char *yytext;
    extern FILE *yyin;
    SymTable *symTable;
    Stack *_call_stack;
    Stack *_funcstart_stack;
    struct ExprList *idexprlist;
    OffsetStack *scopeoffsetstack;


%}

%start program

%union{
    char*   strValue;
    double  numValue;

    struct SymTableEntry    *symValue;
    struct Expression       *exprValue;
    struct Stmt             *stmtValue;

    struct Call             *functionCall;
}

%token<numValue>    NUM
%token<strValue>    ID STRING
%token<strValue>    IF ELSE WHILE FOR FUNCTION RETURN BREAK CONTINUE AND NOT OR LOCAL TRUE FALSE NIL
%token<strValue>    LEFT_BRACE RIGHT_BRACE LEFT_BRACKET RIGHT_BRACKET LEFT_PARENTHESIS RIGHT_PARENTHESIS SEMICOLON COMMA COLON DOUBLE_COLON DOT DOUBLE_DOT
%token<strValue>    EQUALS PLUS MINUS MULT DIV MOD EQUALS_EQUALS NOT_EQUALS PLUS_PLUS MINUS_MINUS GREATER_THAN LESS_THAN GREATER_OR_EQUAL LESS_OR_EQUAL UMINUS

%type<numValue>     M N ifprefix whilestart whilecond elseprefix funcbody
%type<stmtValue>    statement statements forstmt returnstmt whilestmt ifstmt block blockstmt
%type<strValue>     idlist 
%type<exprValue>    expression lvalue funcstart const primary term assignexpr elist call funcdef forprefix objectdef indexed indexedelem arg member
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

statements  :   statements {} statement
                | statement 
                ;

statement   :   expression SEMICOLON
                    {
                        $$ = stmt();
                        mk_bool_vmasm($1);
                        //reset_temp_counter();
                    }
                | ifstmt
                    {
                        $$ = $1;
                        //reset_temp_counter();
                    }
                | whilestmt
                    {
                        $$ = stmt();
                        //reset_temp_counter();
                    }
                | forstmt
                    {
                        $$ = stmt();
                        //reset_temp_counter();
                    }
                | returnstmt            {
                                            SymTableEntry *curfunc = (SymTableEntry*)top(_call_stack);    

                                            if(!curfunc){
                                                printf("input:%d: error:  Return outside of scope \n", yylineno);
                                                _valid_comp = 0;
                                                YYABORT;
                                            }     

                                            if(scope==0 && _in_control==0){
                                                printf("input:%d: error:  Return outside of scope \n", yylineno);

                                                _valid_comp = 0;
                                                YYABORT;
                                            }else{
                                                 //printf("Returns  %d %d\n", e->value.varValue->line, yylineno);
                                                if(_func_count>0 && _in_control >0) _in_control--;
                                                else _in_control=0;
                                            }
                                            $$ = $1; // might need new
                                            //reset_temp_counter();
                                        }

                | BREAK SEMICOLON       {
                                            if(scope==0 && _in_control==0){
                                                printf("input:%d: error:  Break outside of scope \n", yylineno);
                                                _valid_comp = 0;
                                                YYABORT;
                                            }else{
                                                if(_in_control>0)  _in_control--;
                                            }
                                            $$ = stmt();
                                            $$->breaklist = next_quad();
                                            emit(JUMP_I, NULL, NULL, NULL, 0, yylineno);
                                            //reset_temp_counter();
                                        }      
                | CONTINUE SEMICOLON    { 
                                            if(scope==0 && _in_control==0){

                                                printf("input:%d: error:  Continue outside of scope \n", yylineno);
                                                _valid_comp = 0;
                                                YYABORT;
                                            }else{
                                                if(_in_control>0)  _in_control--;
                                            }
                                            $$ = stmt();
                                            $$->contlist = next_quad();
                                            emit(JUMP_I, NULL, NULL, NULL, 0, yylineno);
                                            //reset_temp_counter();
                                        }
                | block
                    {
                        $$ = $1;
                        //reset_temp_counter();
                    }
                | funcdef
                    {
                        $$ = stmt();
                        //reset_temp_counter();
                    }
                | SEMICOLON
                    {
                        $$ = stmt();
                        //reset_temp_counter();
                    }
                ;

expression  :   assignexpr                      
                    {
                        $$ = $1;
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
                | expression DIV expression     
                    {
                        $$ = sym_expr(new_temp(symTable, scope));
                        emit(DIV_I, $$, $1, $3, 0, yylineno);
                    }
                | expression MOD expression
                    {
                        $$ = sym_expr(new_temp(symTable, scope));
                        emit(MOD_I, $$, $1, $3, 0, yylineno);
                    }
                | expression GREATER_THAN expression
                    {
                        $$ = expr(BOOLEXPR_E);
                        $$->sym = new_temp(symTable, scope);
                        $$->truelist = llist(next_quad());
                        $$->falselist = llist(next_quad() + 1);
                        emit(IF_GREATER_I, NULL, $1, $3, 0, yylineno);
                        emit(JUMP_I, NULL, NULL, NULL, 0, yylineno);
                    }
                | expression GREATER_OR_EQUAL expression
                    {
                        $$ = expr(BOOLEXPR_E);
                        $$->sym = new_temp(symTable, scope);
                        $$->truelist = llist(next_quad());
                        $$->falselist = llist(next_quad() + 1);
                        emit(IF_GREATEREQ_I, NULL, $1, $3, 0, yylineno);
                        emit(JUMP_I, NULL, NULL, NULL, 0, yylineno);
                    }
                | expression LESS_THAN expression
                    {
                        $$ = expr(BOOLEXPR_E);
                        $$->sym = new_temp(symTable, scope);
                        $$->truelist = llist(next_quad());
                        $$->falselist = llist(next_quad() + 1);
                        emit(IF_LESS_I, NULL, $1, $3, 0, yylineno);
                        emit(JUMP_I, NULL, NULL, NULL, 0, yylineno);
                    }
                | expression LESS_OR_EQUAL expression
                    {
                        $$ = expr(BOOLEXPR_E);
                        $$->sym = new_temp(symTable, scope);
                        $$->truelist = llist(next_quad());
                        $$->falselist = llist(next_quad() + 1);
                        emit(IF_LESSEQ_I, NULL, $1, $3, 0, yylineno);
                        emit(JUMP_I, NULL, NULL, NULL, 0, yylineno);
                    }
                | expression EQUALS_EQUALS {mk_bool_vmasm($1);} expression
                    {
                        $$ = expr(BOOLEXPR_E);
                        $$->sym = new_temp(symTable, scope);

                        mk_bool_vmasm($4);

                        $$->truelist = llist(next_quad());
                        $$->falselist = llist(next_quad() + 1);
                        emit(IF_EQ_I, NULL, $1, $4, 0, yylineno);
                        emit(JUMP_I, NULL, NULL, NULL, 0, yylineno);
                    }
                | expression NOT_EQUALS {mk_bool_vmasm($1);} expression    
                    {
                        $$ = expr(BOOLEXPR_E);
                        $$->sym = new_temp(symTable, scope);

                        mk_bool_vmasm($4);

                        $$->truelist = llist(next_quad());
                        $$->falselist = llist(next_quad() + 1);
                        emit(IF_NOTEQ_I, NULL, $1, $4, 0, yylineno);
                        emit(JUMP_I, NULL, NULL, NULL, 0, yylineno);
                    }
                | expression AND {mk_nbe_vmasm(AND_I, $1);} M expression
                    {
                        $$ = expr(BOOLEXPR_E);
                        $$->sym = new_temp(symTable, scope);

                        // emit if_eq if second expression is not a boolexpr
                        if($5->type != BOOLEXPR_E){
                            $5->truelist = llist(next_quad());
                            $5->falselist = llist(next_quad() + 1);
                            emit(IF_EQ_I, NULL, $5, bool_expr(1), 0, yylineno);
                            emit(JUMP_I, NULL, NULL, NULL, 0, yylineno);
                        }

                        if($1->type == BOOLEXPR_E)
                            llist_patch($1->truelist, $4);

                        $$->truelist = $5->truelist;
                        $$->falselist = llist_merge($1->falselist, $5->falselist);
                    }
                | expression OR {mk_nbe_vmasm(OR_I, $1);} M expression
                    {
                        $$ = expr(BOOLEXPR_E);
                        $$->sym = new_temp(symTable, scope);

                         // emit if_eq if second expression is not a boolexpr
                        if($5->type != BOOLEXPR_E){
                            $5->truelist = llist(next_quad());
                            $5->falselist = llist(next_quad() + 1);
                            emit(IF_EQ_I, NULL, $5, bool_expr(1), 0, yylineno);
                            emit(JUMP_I, NULL, NULL, NULL, 0, yylineno);
                        }

                        if($1->type == BOOLEXPR_E)
                            llist_patch($1->falselist, $4);

                        $$->truelist = llist_merge($1->truelist, $5->truelist);
                        $$->falselist = $5->falselist;
                    }
                | term  
                    {
                        $$ = $1;
                    }
                ;

M           : {$$ = next_quad();}

term        :   LEFT_PARENTHESIS expression RIGHT_PARENTHESIS 
                    {
                        $$ = $2;
                    }
                | MINUS expression %prec UMINUS
                    {
                        $$ = expr(ARITHEXPR_E);
                        $$->sym = new_temp(symTable, scope);
                        emit(UMINUS_I, $$, $2, NULL, next_quad(), yylineno);
                    }
                | NOT expression
                    {
                        llist_t temp;
                        $$ = sym_expr(new_temp(symTable, scope));
                        $$->type = BOOLEXPR_E;

                        if($2->type != BOOLEXPR_E){

                            // reverse
                            $$->falselist = llist(next_quad());
                            $$->truelist = llist(next_quad() + 1);
                            emit(IF_EQ_I, NULL, $2, bool_expr(1), 0, yylineno);
                            emit(JUMP_I, NULL, NULL, NULL, 0, yylineno);
                        }else{
                            temp = $2->truelist;
                            $$->truelist = $2->falselist;
                            $$->falselist = temp;
                        }

                    }
                | PLUS_PLUS lvalue
                    {            
                        Expr *ex = (Expr*)0;   
                        SymTableEntry *e;

                        // Check wether lvalue is a function

                        if($2 && $2->sym && _further_checks){
                            e = $2->sym;
                            ex = $2;

                            if(e->type == LOCAL_VAR || e->type == GLOBAL_VAR || e->type == ARGUMENT_VAR){

                                // If it's a variable, check wether we have to create a new one or its referencing an existing one
                                if(!e->isActive){
                                    //printf("Adding, I found it inactive on line %d %d\n", e->value.varValue->line, yylineno);
                                    if(scope == 0){
                                        ex = sym_expr(insert(symTable, e->value.varValue->name, scope, yylineno, GLOBAL_VAR));
                                    }else
                                        ex = sym_expr(insert(symTable, e->value.varValue->name, scope, yylineno, LOCAL_VAR));
                                }else if(!is_valid(_call_stack, e, scope)){
                                    if(e->value.varValue->scope != scope){
                                        printf("input:%d: error: could not access variable %s\n", yylineno, e->value.varValue->name);
                                        _valid_comp = 0;
                                        YYABORT;
                                    }
                                }

                            }else{
                                
                                // Functions can not be l-values
                                if(_func_lvalue_check) {
                                    printf("input:%d: error: on function %s, functions cannot be l-values\n", yylineno, e->value.funcValue->name);
                                    _valid_comp = 0;
                                    YYABORT;
                                }
                            }

                            if(ex->type == TABLEITEM_E){
                                $$ = emit_if_table_item(symTable, scope, ex);
                                emit(ADD_I, $$, $$, num_expr(1), 0, yylineno);
                                emit(TABLESETELEM_I, ex, ex->index, $$, 0, yylineno);
                            }else{
                                emit(ADD_I, ex, ex, num_expr(1), 0, yylineno);
                                $$ = expr(ARITHEXPR_E);
                                $$->sym = new_temp(symTable, scope);
                                emit(ASSIGN_I, $$, ex, NULL, 0, yylineno);
                            }
                        }
                    }
                | lvalue PLUS_PLUS 
                    {
                        Expr *ex = (Expr*)0, *val = (Expr*)0;
                        SymTableEntry *e;

                        // Check wether lvalue is a function

                        if($1 && $1->sym && _further_checks){
                            e = $1->sym;
                            ex = $1;

                            if(e->type == LOCAL_VAR || e->type == GLOBAL_VAR || e->type == ARGUMENT_VAR){

                                // If it's a variable, check wether we have to create a new one or its referencing an existing one
                                if(!e->isActive){
                                    //printf("Adding, I found it inactive on line %d %d\n", e->value.varValue->line, yylineno);
                                    if(scope == 0){
                                        ex = sym_expr(insert(symTable, e->value.varValue->name, scope, yylineno, GLOBAL_VAR));
                                    }else
                                        ex = sym_expr(insert(symTable, e->value.varValue->name, scope, yylineno, LOCAL_VAR));
                                }else if(!is_valid(_call_stack, e, scope)){
                                    if(e->value.varValue->scope != scope){
                                        printf("input:%d: error: could not access variable %s\n", yylineno, e->value.varValue->name);
                                        _valid_comp = 0;
                                        YYABORT;
                                    }
                                }

                            }else{
                                
                                // Functions can not be l-values
                                if(_func_lvalue_check) {
                                    printf("input:%d: error: on function %s, functions cannot be l-values\n", yylineno, e->value.funcValue->name);
                                    _valid_comp = 0;
                                    YYABORT;
                                }
                            }

                            $$ = sym_expr(new_temp(symTable, scope));
                            if(ex->type == TABLEITEM_E){
                                val = emit_if_table_item(symTable, scope, ex);
                                emit(ASSIGN_I, $$, val, NULL, 0, yylineno);
                                emit(ADD_I, val, val, num_expr(1), 0, yylineno);
                                emit(TABLESETELEM_I, ex, ex->index, val, 0, yylineno);
                            }else{
                                emit(ASSIGN_I, $$, ex, NULL, 0, yylineno);
                                emit(ADD_I, ex, ex, num_expr(1), 0, yylineno);
                            }
                        }
                    }
                | MINUS_MINUS lvalue 
                    {               
                        Expr *ex = (Expr*)0;
                        SymTableEntry *e;

                        // Check wether lvalue is a function

                        if($2 && $2->sym && _further_checks){
                            e = $2->sym;
                            ex = $2;

                            if(e->type == LOCAL_VAR || e->type == GLOBAL_VAR || e->type == ARGUMENT_VAR){

                                // If it's a variable, check wether we have to create a new one or its referencing an existing one
                                if(!e->isActive){
                                    //printf("Adding, I found it inactive on line %d %d\n", e->value.varValue->line, yylineno);
                                    if(scope == 0){
                                        ex = sym_expr(insert(symTable, e->value.varValue->name, scope, yylineno, GLOBAL_VAR));
                                    }else
                                        ex = sym_expr(insert(symTable, e->value.varValue->name, scope, yylineno, LOCAL_VAR));
                                }else if(!is_valid(_call_stack, e, scope)){
                                    if(e->value.varValue->scope != scope){
                                        printf("input:%d: error: could not access variable %s\n", yylineno, e->value.varValue->name);
                                        _valid_comp = 0;
                                        YYABORT;
                                    }
                                }

                            }else{
                                
                                // Functions can not be l-values
                                if(_func_lvalue_check) {
                                    printf("input:%d: error: on function %s, functions cannot be l-values\n", yylineno, e->value.funcValue->name);
                                    _valid_comp = 0;
                                    YYABORT;
                                }
                            }

                            if(ex->type == TABLEITEM_E){
                                $$ = emit_if_table_item(symTable, scope, ex);
                                emit(SUB_I, $$, $$, num_expr(1), 0, yylineno);
                                emit(TABLESETELEM_I, ex, ex->index, $$, 0, yylineno);
                            }else{
                                emit(SUB_I, ex, ex, num_expr(1), 0, yylineno);
                                $$ = expr(ARITHEXPR_E);
                                $$->sym = new_temp(symTable, scope);
                                emit(ASSIGN_I, $$, ex, NULL, 0, yylineno);
                            }
                        }
                    }
                | lvalue MINUS_MINUS 
                    {               
                        Expr *ex = (Expr*)0, *val = (Expr*)0;
                        SymTableEntry *e;

                        // Check wether lvalue is a function

                        if($1 && $1->sym && _further_checks){
                            e = $1->sym;
                            ex = $1;

                            if(e->type == LOCAL_VAR || e->type == GLOBAL_VAR || e->type == ARGUMENT_VAR){

                                // If it's a variable, check wether we have to create a new one or its referencing an existing one
                                if(!e->isActive){
                                    //printf("Adding, I found it inactive on line %d %d\n", e->value.varValue->line, yylineno);
                                    if(scope == 0){
                                        ex = sym_expr(insert(symTable, e->value.varValue->name, scope, yylineno, GLOBAL_VAR));
                                    }else
                                        ex = sym_expr(insert(symTable, e->value.varValue->name, scope, yylineno, LOCAL_VAR));
                                }else if(!is_valid(_call_stack, e, scope)){
                                    if(e->value.varValue->scope != scope){
                                        printf("input:%d: error: could not access variable %s\n", yylineno, e->value.varValue->name);
                                        _valid_comp = 0;
                                        YYABORT;
                                    }
                                }

                            }else{
                                
                                // Functions can not be l-values
                                if(_func_lvalue_check) {
                                    printf("input:%d: error: on function %s, functions cannot be l-values\n", yylineno, e->value.funcValue->name);
                                    _valid_comp = 0;
                                    YYABORT;
                                }
                            }

                            $$ = sym_expr(new_temp(symTable, scope));
                            if(ex->type == TABLEITEM_E){
                                val = emit_if_table_item(symTable, scope, ex);
                                emit(ASSIGN_I, $$, val, NULL, 0, yylineno);
                                emit(SUB_I, val, val, num_expr(1), 0, yylineno);
                                emit(TABLESETELEM_I, ex, ex->index, val, 0, yylineno);
                            }else{
                                emit(ASSIGN_I, $$, ex, NULL, 0, yylineno);
                                emit(SUB_I, ex, ex, num_expr(1), 0, yylineno);
                            }
                        }



                    }
                | primary   
                    {   
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
                                    }else if(!is_valid(_call_stack, e, scope)){
                                        if(e->value.varValue->scope != scope && e->type != GLOBAL_VAR){
                                            printf("input:%d: error: could not access variable %s\n", yylineno, e->value.varValue->name);
                                            _valid_comp = 0;
                                            YYABORT;
                                            $1 = NULL;
                                        }
                                    }

                                }else{
                                    $1 = NULL;
                                    // Functions can not be l-values
                                    if(_func_lvalue_check) {
                                        printf("input:%d: error: on function %s, functions cannot be l-values\n", yylineno, e->value.funcValue->name);
                                        _valid_comp = 0;
                                        YYABORT;
                                    }
                                }
                            }
                        }
EQUALS expression   {

                        mk_bool_vmasm($4);

                        if($1->type == TABLEITEM_E){
                            emit(TABLESETELEM_I, $1, $1->index, $4, 0, yylineno);
                            //emit(TABLESETELEM_I, $4, $1, $1->index, 0, yylineno);

                            $$ = emit_if_table_item(symTable, scope, $1);
                            $$->type = ASSIGNEXPR_E;
                        }else{
                            emit(ASSIGN_I, $1, $4, NULL, 0, yylineno);
                            $$ = sym_expr(new_temp(symTable, scope));

                            // second emit
                            emit(ASSIGN_I, $$, $1, NULL, 0, yylineno);
                        }
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
                                    }else if(!is_valid(_call_stack, e, scope)){
                                        if(e->value.varValue->scope != scope && e->type != GLOBAL_VAR){
                                            printf("input:%d: error: could not access variable %s\n", yylineno, e->value.varValue->name);
                                            _valid_comp = 0;
                                            YYABORT;
                                        }
                                    }

                                }
                            }

                            $$ = emit_if_table_item(symTable, scope, $1);
                        }
                | call 
                    {
                        $$ = $1;
                    }
                | objectdef
                    {
                        $$ = $1;
                    }
                | LEFT_PARENTHESIS funcdef RIGHT_PARENTHESIS 
                    {
                        $$ = $2;
                    }
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
                                                    printf("IN HERE for %s\n", e->value.funcValue->name);
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
                                                        _valid_comp = 0;
                                                        YYABORT;
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
                                                    _valid_comp = 0;
                                                    YYABORT;
                                                    $$ = NULL;
                                                }else{
                                                    // If exists, set reference
                                                    $$ = sym_expr(e);
                                                }
                                                _further_checks = 0;
                                                _func_lvalue_check = 1;
                                            }
                | member                    {
                                                _func_lvalue_check = 0;
                                                $$ = $1;
                                            }
                ;

member      :   lvalue DOT ID   
                    {   
                        $$ = member_expr(symTable, scope, $1, $3);
                    }
                | lvalue LEFT_BRACE expression RIGHT_BRACE  
                    {
                        Expr *res;
                        printf("Hallo in here for %s\n", $1->sym->value.varValue->name);
                        $1 = emit_if_table_item(symTable, scope, $1);
                        res = expr(TABLEITEM_E);
                        res->sym = $1->sym;
                        res->index = $3;
                        $$ = res;
                    }
                | call DOT ID   
                    {
                        $$ = member_expr(symTable, scope, $1, $3);
                    }
                | call LEFT_BRACE expression RIGHT_BRACE
                    {
                        Expr *res;

                        $1 = emit_if_table_item(symTable, scope, $1);
                        res = expr(TABLEITEM_E);
                        res->sym = $1->sym;
                        res->index = $3;
                        $$ = res;
                    }
                ;

call        :   call LEFT_PARENTHESIS elist RIGHT_PARENTHESIS
                    {
                        $$ = make_call(symTable, scope, $1, reverse_elist(&$3));
                    }
                | lvalue callsuffix {
                                        SymTableEntry* e;

                                        Call *c;
                                        SymTableEntry *e1;
                                        e1 = $1->sym;
                                        c = $2;
                                        char *orig_name = $2->name;
                                        c->name = strdup(e1->value.funcValue->name);

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
                                                }else if(!is_valid(_call_stack, e, scope)){
                                                    if(e->value.varValue->scope != scope && e->value.varValue->scope != 0){
                                                        printf("input:%d: error: could not access variable %s\n", yylineno, e->value.varValue->name);
                                                        _valid_comp = 0;
                                                        YYABORT;
                                                    }
                                                }

                                                
                                            }
                                        }

                                        if($2->isMethod){
                                            Expr *h = $1;

                                            if($2->elist->type == NIL_E){
                                                $1 = member_expr(symTable, scope, $1, orig_name); 
                                                emit_if_table_item(symTable, scope, $1);

                                                $1->next = NULL;
                                                $$ = make_call(symTable, scope, $1, $1);
                                            }else{
                                               
    
                                                $1 = member_expr(symTable, scope, $1, orig_name);
                                                emit_if_table_item(symTable, scope, $1);

                                                $1->next = $2->elist;
                                                $2->elist = $1;
                    
                                                $$ = make_call(symTable, scope, $1, reverse_elist(&$2->elist));
                                            }

                                        }else{
                                            $$ = make_call(symTable, scope, $1, reverse_elist(&$2->elist));
                                        }
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
                                                                $$ = function_call(0, NULL, scope, $2);
                                                            }

methodcall  :   DOUBLE_DOT ID LEFT_PARENTHESIS elist RIGHT_PARENTHESIS  {
                                                                            $$ = function_call(1, $2, scope, $4);
                                                                        }

elist       :   expression                  {
                                                mk_bool_vmasm($1);
                                                $$ = $1;
                                            }
                | expression COMMA elist    {
                                                Expr *head = $1;

                                                head->next = $3;

                                                $$ = head;
                                            }
                |
                    {
                        $$ = nil_expr();
                    }
                ;
        
objectdef   :    LEFT_BRACE elist RIGHT_BRACE
                    {
                        int i = 0;
                        Expr* t = expr(NEWTABLE_E);
                        t->sym  = new_temp(symTable, scope);
                        emit(TABLECREATE_I,t,NULL,NULL,next_quad(),yylineno);
                        Expr* tmp = $2;
                        if(tmp->type != NIL_E){
                            while(tmp != NULL){
                                emit(TABLESETELEM_I,t,num_expr(i++),tmp,next_quad(),yylineno);
                                tmp = tmp->next;
                            }
                        }
                        $$ = t;
                    }
                | LEFT_BRACE indexed RIGHT_BRACE
                    {
                        Expr* t = expr(NEWTABLE_E);
                        t->sym  = new_temp(symTable, scope);
                        emit(TABLECREATE_I, t, NULL, NULL, next_quad(), yylineno);
                        Expr* tmp = $2;
                        while(tmp != NULL){
                            emit(TABLESETELEM_I, t, tmp->index, tmp->next_index, next_quad(), yylineno);
                            tmp = tmp->next;
                        }
                        $$ = t;
                    }
                ;

indexed     :   indexedelem
                    {
                        $$ = $1;
                    }
                | indexedelem COMMA indexed
                    {
                        Expr* tmp = $1;
                        while(tmp->next != NULL){
                            tmp = tmp->next;
                        }
                        tmp->next   = $3;
                        $$ = $1;
                    }
                ;
                
indexedelem :   LEFT_BRACKET expression COLON expression RIGHT_BRACKET
                    {
                        mk_bool_vmasm($4);
                        $$=expr(NEWTABLE_E);
                        $$->index= $2;
                        $$->next_index  = $4;
                    }
                ;

block       :   blockstart blockend 
                    {
                        $$ = stmt();
                    }
                | blockstart blockstmt blockend 
                    {
                        $$ = $2;
                    }
                ;

blockstart  :   LEFT_BRACKET    
                    {
                        scope++; // Up scope
                    }

blockend    :   RIGHT_BRACKET   
                    {
                        // Scope down and hide old symbols
                        scope_down(symTable);
                    }

blockstmt   :   statement    
                    {
                        $$ = $1;
                    }
                | statement blockstmt 
                    {
                        $$ = stmt();
                        $$->breaklist = llist_merge($1->breaklist, $2->breaklist);
                        $$->contlist = llist_merge($1->contlist, $2->contlist);
                    }
                ;

funcdef     :   funcstart LEFT_PARENTHESIS {scope++;} idlist { enterscopespace(); resetfunctionlocalsoffset();} RIGHT_PARENTHESIS  
                    {    
                        scope--; 
                        _func_count++;

                        // Push function to stack
                        if($1 && $1->sym)
                            push(_call_stack, (void*)$1->sym);
                        //printCallStack(stack, yylineno);
                    } 
                                                                                    
                funcbody   {
                            _func_count--;
                            exitscopespace();
                            if($1 && $1->sym)
                                $1->sym->total_locals = $8;

                            //printf("TOTAL LOCALS FOR SMTH: %d\n", $8);
                            
                            restorecurrscopeoffset(pop_and_top_offset(scopeoffsetstack));

                            SymTableEntry *last = (SymTableEntry*)top(_call_stack);

                            // Removing function from stack
                            if($1 && $1->sym)
                                pop(_call_stack);

                            emit(FUNCEND_I, sym_expr(last), 0, 0, 0, yylineno);
                            int toPatch = *((int*)top(_funcstart_stack));
                            pop(_funcstart_stack);
                            patch_label(toPatch, next_quad());
                            $$ = $1;
                        }
                ;

funcbody    :   block   
                    {
                        $$ = currscopeoffset();
                        //printf("CURSCOPEOFFSET : %d\n", $$);
                        exitscopespace();
                    }

funcstart   :   FUNCTION ID {
                                SymTableEntry *e;

                                _func_name = $2;

                                // Search for libfunc
                                if((e = function_lookup(symTable, $2, scope)) == NULL){

                                    // Check libfuncs
                                    if((e = lookup(symTable, $2, 0, LIB_FUNC)) != NULL){
                                        printf("input:%d: error: shadowing of library function %s is not permitted\n", yylineno, $2);
                                        _valid_comp = 0;
                                        YYABORT;
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
                                    _valid_comp = 0;
                                    YYABORT;
                                    $$ = NULL;
                                }


                                if($$ && $$->sym)
                                    $$->sym->iaddress = next_quad();

                                if($$){
                                    int *ti = (int*)malloc(sizeof(int));
                                    *ti = next_quad();
                                    push(_funcstart_stack, (void*)ti);
                                    emit(JUMP_I, 0, 0, 0, next_quad(), yylineno);
                                    emit(FUNCSTART_I, $$, 0, 0, 0, yylineno);
                                }



                                push_offset(scopeoffsetstack, currscopeoffset());
                                
                                enterscopespace(); 
                                resetformalargsoffset();
                            }
                | FUNCTION  {  
                                char *s;
                                s = (char *)malloc(8 * sizeof(char));
                                _func_name = s;
     
                                sprintf(s, "$%d", _anon_func_counter);

                                $$ = sym_expr(insert(symTable, s, scope, yylineno, USER_FUNC));

                                _anon_func_counter++;

                                if($$ && $$->sym)
                                    $$->sym->iaddress = next_quad();

                                int *ti = (int*)malloc(sizeof(int));
                                *ti = next_quad();
                                push(_funcstart_stack, (void*)ti);
                                emit(JUMP_I, 0, 0, 0, next_quad(), yylineno);
                                emit(FUNCSTART_I, $$, 0, 0, 0, yylineno);
                                push_offset(scopeoffsetstack, currscopeoffset());

                                enterscopespace(); 
                                resetformalargsoffset();
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
                    {
                        _func_locals++;
                    }
                | arg COMMA idlist 
                    {
                        _func_locals++;
                    }
                | {}
                ;

arg         :   ID  
                    {
                        SymTableEntry *e, *a;
                        $$ = NULL;

                        if((e = lookup(symTable, $1, 0, LIB_FUNC)) != NULL){
                            printf("input:%d: error: argument %s in function %s attempting to shadow a library function\n", yylineno, $1, _func_name);
                            _valid_comp = 0;
                            YYABORT;
                        }else{
                            if((e = lookup(symTable, _func_name, scope - 1, USER_FUNC)) != NULL){
                                if((a = lookup_active(symTable, $1, scope)) == NULL){
                                    $$ = sym_expr(insert(symTable, $1, scope, yylineno, ARGUMENT_VAR));
                                }else{
                                    printf("input:%d: error: duplicate argument %s in function %s\n", yylineno, $1, e->value.funcValue->name);
                                    _valid_comp = 0;
                                    YYABORT;
                                }
                            } 
                        }                         
                    }

ifstmt      :   ifprefix statement  
                    {
                        $$ = stmt();
                        $$->contlist = $2->contlist;
                        $$->breaklist = $2->breaklist;
                        patch_label($1, next_quad());
                    }
                | ifprefix statement elseprefix statement
                    {
                        $$ = stmt();
                        $$->contlist = llist_merge($2->contlist, $4->contlist);
                        $$->breaklist = llist_merge($2->breaklist, $4->breaklist);
                        patch_label($1, $3 + 1);
                        patch_label($3, next_quad());
                    }
                ;

elseprefix  :   ELSE
                    {
                        $$ = next_quad();
                        emit(JUMP_I, NULL, NULL, NULL, 0, yylineno);
                    }

ifprefix    :   IF LEFT_PARENTHESIS { _in_control++; } expression RIGHT_PARENTHESIS
                    {
                        mk_bool_vmasm($4);

                        emit(IF_EQ_I, NULL, $4, bool_expr(1), next_quad() + 2, yylineno);
                        $$ = next_quad();
                        emit(JUMP_I, NULL, NULL, NULL, 0, yylineno);
                    }

whilestart  :   WHILE
                    {
                        $$ = next_quad();
                    }

whilecond   :   LEFT_PARENTHESIS { _in_control++;} expression RIGHT_PARENTHESIS
                    {
                        mk_bool_vmasm($3);
                        emit(IF_EQ_I, NULL, $3, bool_expr(1), next_quad() + 2, yylineno);
                        $$ = next_quad();
                        emit(JUMP_I, NULL, NULL, NULL, 0, yylineno);
                    }

whilestmt   :   whilestart whilecond statement
                    {
                        $$ = stmt();
                        $$->contlist = $3->contlist;
                        $$->breaklist = $3->breaklist;

                        emit(JUMP_I, NULL, NULL, NULL, $1, yylineno);
                        patch_label($2, next_quad());
                        llist_patch($3->breaklist, next_quad());
                        llist_patch($3->contlist, $1);
                    }

forprefix   :   forstart elist SEMICOLON M expression SEMICOLON
                    {
                        $$ = bool_expr(1);
                        mk_bool_vmasm($5);
                        $$->test = $4;
                        $$->enter = next_quad();
                        emit(IF_EQ_I, NULL, $5, bool_expr(1), next_quad(), yylineno);
                    }

forstart    :   FOR LEFT_PARENTHESIS { ++_in_control;}

forstmt     :   forprefix N elist RIGHT_PARENTHESIS N statement N
                    {
                        $$ = stmt();
                        $$->contlist = $6->contlist;
                        $$->breaklist = $6->breaklist;

                        patch_label($1->enter, $5 + 1);
                        patch_label($2, next_quad());
                        patch_label($5, $1->test);
                        patch_label($7, $2 + 1);

                        llist_patch($6->breaklist,next_quad());
                        llist_patch($6->contlist,$2+1);
                    }

N           :       {
                        $$ = next_quad();
                        emit(JUMP_I, NULL, NULL, NULL, 0, yylineno);
                    }

returnstmt  :   RETURN SEMICOLON
                    {
                        emit(RET_I, NULL, NULL, NULL, 0, yylineno);
                    }
                | RETURN expression SEMICOLON
                    {
                        mk_bool_vmasm($2);
                        emit(RET_I, $2, NULL, NULL, 0, yylineno);
                        $$ = $2;
                    }
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

int main(int argc, char **argv){

    // redir stdout
    //freopen("quads.txt", "w", stdout);

    if(argc > 1){
        if(!(yyin = fopen(argv[1], "r"))){
            printf("File not found\n");
            exit(0);
        }
    }else{
        yyin = stdin;

    }

    init_quads();
    symTable = init_sym_table();
    _call_stack = stack();
    _funcstart_stack = stack();
    scopeoffsetstack = init_offset_stack();
    yyparse();

    if(!_valid_comp){
        printf("alpha: compilation failed\n");
        return 0;
    }

    //printSymTable(symTable);
    printByScope(symTable);
    printQuads();

    parse_target_code();

    mk_bin("../AVM/target.abc");

    return 0;
}
