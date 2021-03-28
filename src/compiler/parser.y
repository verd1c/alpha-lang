%{
    #include "parser.h"
    #include "struct.h"
    int yylex (void);
    int yyerror (char* yaccProvidedMessage);

    extern int yylineno;
    extern char *yytext;
    extern FILE *yyin;

    char *typeToString[] = {
        "LOCAL_VAR",
        "GLOBAL_VAR",
        "ARGUMENT",

        "USER_FUNC",
        "LIB_FUNC"
    };

    int scope;
    SymTable *symTable;
%}

%start program

%union{
    char*   strValue;
    int     intValue;
    double  numValue;
}

%token<numValue>    NUM
%token<strValue>    ID STRING
%token<strValue>    IF ELSE WHILE FOR FUNCTION RETURN BREAK CONTINUE AND NOT OR LOCAL TRUE FALSE NIL
%token<strValue>    LEFT_BRACE RIGHT_BRACE LEFT_BRACKET RIGHT_BRACKET LEFT_PARENTHESIS RIGHT_PARENTHESIS SEMICOLON COMMA COLON DOUBLE_COLON DOT DOUBLE_DOT
%token<strValue>    EQUALS PLUS MINUS MULT DIV MOD EQUALS_EQUALS NOT_EQUALS PLUS_PLUS MINUS_MINUS GREATER_THAN LESS_THAN GREATER_OR_EQUAL LESS_OR_EQUAL UMINUS

%type<strValue>       lvalue

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
                |
                ;

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
                | expression op expression
                | term
                ;

op          :   PLUS | MINUS | MULT | DIV | MOD | GREATER_THAN | GREATER_OR_EQUAL | LESS_THAN | LESS_OR_EQUAL | EQUALS_EQUALS | NOT_EQUALS | AND | OR

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
                                                if(scope == 0)
                                                    insert(symTable, $1, scope, yylineno, GLOBAL_VAR);
                                                else
                                                    insert(symTable, $1, scope, yylineno, LOCAL_VAR);
                                            }
                ;

primary     :   lvalue
                | call
                | objectdef
                | LEFT_PARENTHESIS funcdef RIGHT_PARENTHESIS
                | const
                ;

lvalue      :   ID                          {$$ = $1;}
                | LOCAL ID
                | DOUBLE_COLON ID
                | member
                ;

member      :   lvalue DOT ID
                | lvalue LEFT_BRACE expression RIGHT_BRACE
                | call DOT ID
                | call LEFT_BRACE expression RIGHT_BRACE
                ;

call        :   call LEFT_PARENTHESIS elist RIGHT_PARENTHESIS
                | lvalue callsuffix
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
        
objectdef   :   LEFT_BRACE RIGHT_BRACE
                | LEFT_BRACE elist RIGHT_BRACE
                | LEFT_BRACE indexed RIGHT_BRACE
                ;

indexed     :   indexedelem
                | indexedelem COMMA indexed
                |
                ;

indexedelem :   LEFT_BRACKET expression COLON expression RIGHT_BRACKET

block       :   blockstart blockend 
                | blockstart blockstmt blockend
                ;

blockstart  :   LEFT_BRACKET    {
                                    scope++; // Up scope
                                }

blockend    :   RIGHT_BRACKET   {
                                    scope--;
                                }

blockstmt   :   statement
                | statement blockstmt
                |
                ;

funcdef     :   funcstart argstart idlist argend block 
                | funcstart argstart idlist argend block
                ;

funcstart   :   FUNCTION ID {
                                SymTableEntry *e;

                                // Search for libfunc
                                if((e = lookup(symTable, $2, 0, FUN)) == NULL){
                                    insert(symTable, $2, scope, yylineno, USER_FUNC);
                                }else{
                                    if(e->type == USER_FUNC)
                                        printf("Error:%d: Duplicate user function %s\n", yylineno, $2);
                                    else
                                        printf("Error:%d: Duplicate lib function %s\n", yylineno, $2);
                                }
                            }
                | FUNCTION

argstart    :   LEFT_PARENTHESIS {scope++;}

argend      :   RIGHT_PARENTHESIS {scope--;}

const       :   NUM | STRING | NIL | TRUE | FALSE

idlist      :   arg
                | arg COMMA idlist
                |
                ;

arg         :   ID

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
    printf("ERORR: %s\n", yaccProvidedMessage);
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
    return 0;
}