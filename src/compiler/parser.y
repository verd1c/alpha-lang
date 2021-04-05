%{
    #include "parser.h"
    #include "symbol_table.h"
    int yylex (void);
    int yyerror (char* yaccProvidedMessage);

    char *typeToString[] = {
        "LOCAL_VAR",
        "GLOBAL_VAR",
        "ARGUMENT",

        "USER_FUNC",
        "LIB_FUNC"
    };

    char *_func_name;
    int _func_count = 0;
    int _anon_func_counter = 0;

    int scope;
    extern int yylineno;
    extern char *yytext;
    extern FILE *yyin;
    SymTable *symTable;
%}

%start program

%union{
    char*   strValue;
    int     intValue;
    double  numValue;

    struct SymTableEntry* exprValue;
}

%token<numValue>    NUM
%token<strValue>    ID STRING
%token<strValue>    IF ELSE WHILE FOR FUNCTION RETURN BREAK CONTINUE AND NOT OR LOCAL TRUE FALSE NIL
%token<strValue>    LEFT_BRACE RIGHT_BRACE LEFT_BRACKET RIGHT_BRACKET LEFT_PARENTHESIS RIGHT_PARENTHESIS SEMICOLON COMMA COLON DOUBLE_COLON DOT DOUBLE_DOT
%token<strValue>    EQUALS PLUS MINUS MULT DIV MOD EQUALS_EQUALS NOT_EQUALS PLUS_PLUS MINUS_MINUS GREATER_THAN LESS_THAN GREATER_OR_EQUAL LESS_OR_EQUAL UMINUS

%type<strValue>     member call arg funcstart idlist
%type<exprValue>    lvalue

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
                | returnstmt
                | BREAK SEMICOLON
                | CONTINUE SEMICOLON
                | block
                | funcdef
                | SEMICOLON
                ;

expression  :   assignexpr
                | expression PLUS expression
                | expression MINUS expression
                | expression MULT expression
                | expression DIV expression
                | expression MOD expression
                | expression GREATER_THAN expression
                | expression GREATER_OR_EQUAL expression
                | expression LESS_THAN expression
                | expression LESS_OR_EQUAL expression
                | expression EQUALS_EQUALS expression
                | expression NOT_EQUALS expression
                | expression AND expression
                | expression OR expression
                | term
                ;

term        :   LEFT_PARENTHESIS expression RIGHT_PARENTHESIS
                | UMINUS expression
                | NOT expression
                | PLUS_PLUS lvalue
                | lvalue PLUS_PLUS
                | MINUS_MINUS lvalue
                | lvalue MINUS_MINUS
                | primary
                ;

assignexpr  :   lvalue EQUALS expression    {
                                                SymTableEntry *e;

                                                // Check wether lvalue is a function
                                                e = $1;
                                                if(e->type == LOCAL_VAR || e->type == GLOBAL_VAR || e->type == ARGUMENT_VAR){

                                                    // If it's a variable, check wether we have to create a new one or its referencing an existing one
                                                    if(_func_count != 0 || !e->isActive){
                                                        // Variable was either out of function or not active

                                                        // If it's global or in the same scope, we can reference it
                                                        if(e->type != GLOBAL_VAR && e->value.varValue->scope != scope ){

                                                            // Else, we have to insert a new one
                                                            if(scope == 0){
                                                                insert(symTable, e->value.varValue->name, scope, yylineno, GLOBAL_VAR);
                                                            }else
                                                                insert(symTable, e->value.varValue->name, scope, yylineno, LOCAL_VAR);
                                                        }
                                                    }
                                                }else{
                                                    
                                                    // Functions can not be l-values
                                                    printf("input:%d: error: on function %s, functions cannot be l-values\n", yylineno, e->value.funcValue->name);
                                                }
                                            }

primary     :   lvalue
                | call
                | objectdef
                | LEFT_PARENTHESIS funcdef RIGHT_PARENTHESIS
                | const
                ;

lvalue      :   ID                          {
                                                SymTableEntry *e;
                                                
                                                // Check if active variables exist in any scope from current and down
                                                if((e = lookup_variable(symTable, $1, scope)) == NULL){
                                                    
                                                    // If not, insert the variable
                                                    if(scope == 0)
                                                        $$ = insert(symTable, $1, scope, yylineno, GLOBAL_VAR);
                                                    else
                                                        $$ = insert(symTable, $1, scope, yylineno, LOCAL_VAR);
                                                }else{

                                                    // If exists, set value for further checks
                                                    $$ = e;
                                                }
                                            }
                | LOCAL ID                  {
                                                SymTableEntry *e;
                                                
                                                // Check current scope
                                                if((e = lookup_no_type(symTable, $2, scope)) == NULL){
                                                    if((e = lookup(symTable, $2, 0, LIB_FUNC)) == NULL){
                                                        if(scope == 0){
                                                            $$ = insert(symTable, $2, scope, yylineno, GLOBAL_VAR);
                                                        }else{
                                                            $$ = insert(symTable, $2, scope, yylineno, LOCAL_VAR);
                                                        }
                                                    }else{
                                                        printf("input:%d: error: local symbol %s is attempting to shadow a library function\n", yylineno, $2);
                                                    }
                                                }else{
                                                    // If exists, set reference
                                                    $$ = e;
                                                }
                                            }
                | DOUBLE_COLON ID           {
                                                SymTableEntry *e;
                                                
                                                // Check globals
                                                if((e = lookup_no_type(symTable, $2, 0)) == NULL){
                                                    printf("input:%d: error: global reference %s in line %d not found\n", yylineno, $2, yylineno);
                                                }else{
                                                    // If exists, set reference
                                                    $$ = e;
                                                }
                                            }
                | member                    {}
                ;

member      :   lvalue DOT ID   {}
                | lvalue LEFT_BRACE expression RIGHT_BRACE  {}
                | call DOT ID
                | call LEFT_BRACE expression RIGHT_BRACE
                ;

call        :   call LEFT_PARENTHESIS elist RIGHT_PARENTHESIS
                | lvalue callsuffix {
                                        SymTableEntry* e;

                                        e = $1;
                                        printf("Usage of %s as call in %d\n", e->value.funcValue->name, yylineno);
                                    }
                | LEFT_PARENTHESIS funcdef RIGHT_PARENTHESIS LEFT_PARENTHESIS elist RIGHT_PARENTHESIS
                ;
            
callsuffix  :   normcall
                | methodcall
                ;

normcall    :   LEFT_PARENTHESIS elist RIGHT_PARENTHESIS

methodcall  :   DOUBLE_DOT ID LEFT_PARENTHESIS elist RIGHT_PARENTHESIS

elist       :   expression 
                | expression COMMA elist 
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
                                    scope_down(symTable);
                                }

blockstmt   :   statement
                | statement blockstmt
                ;

funcdef     :   funcstart LEFT_PARENTHESIS {scope++;} idlist RIGHT_PARENTHESIS {scope--; _func_count++;} block {_func_count--;}
                ;

funcstart   :   FUNCTION ID {
                                SymTableEntry *e;
                                int entryScope;

                                $$ = $2;
                                _func_name = $2;

                                // Search for libfunc
                                if((e = function_lookup(symTable, $2, scope)) == NULL){
                                    insert(symTable, $2, scope, yylineno, USER_FUNC);
                                }else{
                                    if(e->type == USER_FUNC)
                                        printf("input:%d: error: duplicate user function %s, first defined in line %d\n", yylineno, $2, e->value.funcValue->line);
                                    else if(e->type == LIB_FUNC)
                                        printf("input:%d: error: shadowing of library function %s is not permitted\n", yylineno, $2);
                                    else
                                        printf("input:%d: error: function %s has conflicting type with variable %s first defined in line %d\n", yylineno, $2, e->value.varValue->name, e->value.varValue->line);
                                }
                            }
                | FUNCTION  {
                                SymTableEntry *e;
                                int entryScope;
                                
                                static char *s;
                                s = (char *)malloc(5*sizeof(char));

                                sprintf(s, "@%d", _anon_func_counter);

                                insert(symTable, s, scope, yylineno, USER_FUNC);

                                _anon_func_counter++;
                            }

const       :   NUM | STRING | NIL | TRUE | FALSE

idlist      :   arg
                | arg COMMA idlist
                | {}
                ;

arg         :   ID   {
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

ifstmt      :   IF LEFT_PARENTHESIS expression RIGHT_PARENTHESIS statement
                | IF LEFT_PARENTHESIS expression RIGHT_PARENTHESIS statement ELSE statement
                ;

whilestmt   :   WHILE LEFT_PARENTHESIS expression RIGHT_PARENTHESIS statement

forstmt     :   FOR LEFT_PARENTHESIS elist SEMICOLON expression SEMICOLON elist RIGHT_PARENTHESIS statement

returnstmt  :   RETURN SEMICOLON
                | RETURN expression SEMICOLON
                ;



%%

int yyerror(char *yaccProvidedMessage){
    printf("alpha: %s\n", yaccProvidedMessage);
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
    int i, scope = 0;

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
    if(argc > 1){
        if(!(yyin = fopen(argv[1], "r"))){
            printf("AAA\n");
            exit(0);
        }
    }else{
        yyin = stdin;

    }

    symTable = init_sym_table();
    yyparse();

    //printSymTable(symTable);
    printByScope(symTable);

    return 0;
}
