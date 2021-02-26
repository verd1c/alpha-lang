%{
#include "lex.yy.h"
#include <string.h>
int line_number;
char name[256];
%}

%option yylineno

%%
if {
    line_number = yylineno;
    strcpy(name, yytext);
    return 100;
}
else {
    line_number = yylineno;
    strcpy(name, yytext);
    return 101;
}
while {
    line_number = yylineno;
    strcpy(name, yytext);
    return 102;
}
. { }
\n { }
%%

int yywrap(){
    return 1;
} 
int alpha_yylex(void *ylval){
    int r = yylex();
    ((struct alpha_token_t*)ylval)->line_number = line_number;
    strcpy(((struct alpha_token_t*)ylval)->name, name);
    return r;
}