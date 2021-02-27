%{
#include "lex.yy.h"
#include <string.h>

const char* category_names[] = {
    "IDENT",
    "KEYWORD",
    "OPERATOR",
    "INTCONST",
    "REALCONST",
    "PUNCTUATION",
    "STRING",
    "COMMENT"
};

int line_number;
char name[256];
int token_count = 0;
enum category category;
%}

keyword             if|else|while|for|function|return|break|continue|and|not|or|local|true|false|nil
operator            (\=)|(\+)|(\-)|(\*)|(\/)|(\%)|(\=\=)|(\!\=)|(\+\+)|(\-\-)|(\>)|(\<)|(\>\=)|(\<\=)
intconst            [0-9]+
punctuation         (\{)|(\})|(\()|(\))|(\[)|(\])|(\:)|(\,)|(\:\:)|(\.)|(\.\.)|(\;)
ident               [a-zA-Z][a-zA-Z0-9_]*
string              \"[^"\n"]+\"
comment             "//".+|"/*".+"*/"

%option yylineno

%%
{keyword} {
    processToken(KEYWORD);
    return 100;
}
{ident} {
    processToken(IDENT);
    return 101;
}
{comment} {
    processToken(COMMENT);
    return 102;
}
{operator} {
    processToken(OPERATOR);
    return 103;
}
{intconst} {
    processToken(INTCONST);
    return 104;
}
{intconst}\.{intconst} {
    processToken(REALCONST);
    return 105;
}
{punctuation} {
    processToken(PUNCTUATION);
    return 106;
}
{string} {
    processToken(STRING);
    return 107;
}
. { }
\n { }
%%

void processToken(enum category cat){
    category = cat;
    token_count++;
    line_number = yylineno;
    strcpy(name, yytext);
}

int yywrap(){
    return 1;
}

int alpha_yylex(void *ylval){
    int r = yylex();
    ((struct alpha_token_t*)ylval)->line_number = line_number;
    ((struct alpha_token_t*)ylval)->token_no = token_count;
    ((struct alpha_token_t*)ylval)->category = category;
    strcpy(((struct alpha_token_t*)ylval)->text, name);
    return r;
}