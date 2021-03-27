%{
    #include "parser.h"
    #include <stdio.h>
    #include <stdlib.h>
    int yylex (void);
    int yyerror (char* yaccProvidedMessage);

    extern int yylineno;
    extern char *yytext;
    extern FILE *yyin;
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
%token<strValue>    LEFT_BRACE RIGHT_BRACE LEFT_BRACKET RIGHT_BRACKET LEFT_PARENTHESIS RIGHT_PARENTHESIS SEMICOLON COMMAA COLON DOUBLE_COLON DOT DOUBLE_DOT
%token<strValue>    EQUALS PLUS MINUS MULT DIV MOD EQUALS_EQUALS NOT_EQUALS PLUS_PLUS MINUS_MINUS GREATER_THAN LESS_THAN GREATER_OR_EQUAL LESS_OR_EQUAL

%right EQUALS
%left COMMA
%left PLUS
%left MULT DIV
%nonassoc   MINUS
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

assignexpr  :   lvalue EQUALS expression
                ;

primary     :   lvalue
                | call
                | objectdef
                | LEFT_PARENTHESIS funcdef RIGHT_PARENTHESIS
                | const
                ;

lvalue      :   ID
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

block       :   LEFT_BRACKET RIGHT_BRACKET
                | LEFT_BRACKET blockstmt RIGHT_BRACKET
                ;

blockstmt   :   statement
                | statement blockstmt
                |
                ;

funcdef     :   FUNCTION ID LEFT_PARENTHESIS idlist RIGHT_PARENTHESIS block
                | FUNCTION LEFT_PARENTHESIS idlist RIGHT_PARENTHESIS block
                ;

const       :   NUM | STRING | NIL | TRUE | FALSE

idlist      :   ID
                | ID COMMA idlist
                |
                ;

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

    yyparse();
    return 0;
}