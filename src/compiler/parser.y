%{
    #include "parser.h"
    #include "struct.h"
    int yylex (void);
    int yyerror (char* yaccProvidedMessage);

    extern int yylineno;
    extern char *yytext;
    extern FILE *yyin;

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
%token<strValue>    EQUALS PLUS MINUS MULT DIV MOD EQUALS_EQUALS NOT_EQUALS PLUS_PLUS MINUS_MINUS GREATER_THAN LESS_THAN GREATER_OR_EQUAL LESS_OR_EQUAL

%type<strValue>       lvalue

%right EQUALS
%left OR
%left AND
%nonassoc EQUALS_EQUALS NOT_EQUALS
%nonassoc GREATER_THAN GREATER_OR_EQUAL LESS_THAN LESS_OR_EQUAL
%left PLUS MINUS
%left MULT DIV MOD
%right NOT PLUS_PLUS MINUS_MINUS
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
                | MINUS expression
                | NOT expression
                | PLUS_PLUS lvalue
                | lvalue PLUS_PLUS
                | MINUS_MINUS lvalue
                | lvalue MINUS_MINUS
                | primary
                ;

assignexpr  :   lvalue EQUALS expression    {insert(symTable, $1, scope, yylineno, LOCAL_VAR);}
                ;

primary     :   lvalue
                | call
                | objectdef
                | LEFT_PARENTHESIS funcdef RIGHT_PARENTHESIS
                | const
                ;

lvalue      :   ID                  {$$ = $1;}
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

elist       :   expression {printf("Found end of elist\n");}
                | expression COMMA elist {printf("FOUND ELIST\n");}
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

block       :   blockstart blockend {printf("Found block at %d\n", yylineno);}
                | blockstart blockstmt blockend
                ;

blockstart  :   LEFT_BRACKET    {
                                    scope++; // Up scope
                                    printf("Current scope at %d : %d\n", yylineno + 1, scope);
                                }

blockend    :   RIGHT_BRACKET   {
                                    scope--;
                                }

blockstmt   :   statement
                | statement blockstmt
                |
                ;

funcdef     :   FUNCTION ID argstart idlist argend block
                | FUNCTION argstart idlist argend block
                ;

argstart    :   LEFT_PARENTHESIS {scope++;}

argend      :   RIGHT_PARENTHESIS {scope--;}

const       :   NUM | STRING | NIL | TRUE | FALSE

idlist      :   arg
                | arg COMMA idlist
                |
                ;

arg         :   ID      {printf("Arg %s is at scope %d\n", $1, scope);}

ifstmt      :   IF LEFT_PARENTHESIS expression RIGHT_PARENTHESIS statement {printf("I FOUND IF\n");}
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
    SymTableEntry *e;
    e = lookup(symTable, "a", 1, LOCAL_VAR);
    printf("%d\n", e->value.varValue->line);
    e = lookup(symTable, "b", 4, LOCAL_VAR);
    printf("%d\n", e->value.varValue->line);
    e = lookup(symTable, "c", 2, LOCAL_VAR);
    printf("%d\n", e->value.varValue->line);
    return 0;
}