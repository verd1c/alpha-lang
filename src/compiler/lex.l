%{
#include "lex.yy.h"
#include <string.h>

#define YY_DECL int yylex (struct alpha_token_t* lvalp)

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

struct alpha_token_t *token_head = NULL;

int line_number;
char name[256];
int token_count = 0;
enum category category;
%}

keyword             if|else|while|for|function|return|break|continue|and|not|or|local|true|false|nil
operator            (\=)|(\+)|(\-)|(\*)|(\/)|(\%)|(\=\=)|(\!\=)|(\+\+)|(\-\-)|(\>)|(\<)|(\>\=)|(\<\=)
intconst            [0-9]+
punctuation         (\{)|(\})|(\[)|(\])|(\()|(\))|(\;)|(\,)|(\:)|(\:\:)|(\.)|(\.\.)
ident               [a-zA-Z][a-zA-Z0-9_]*
string              \"[^"\n"]+\"
comment             "//".+|"/*".+"*/"

%option yylineno

%%
{keyword} {
    process_token(lvalp, KEYWORD);
}
{ident} {
    process_token(lvalp, IDENT);
}
{comment} {
    process_token(lvalp, COMMENT);
}
{operator} {
    process_token(lvalp, OPERATOR);
}
{intconst} {
    process_token(lvalp, INTCONST);
}
{intconst}\.{intconst} {
    process_token(lvalp, REALCONST);
}
{punctuation} {
    process_token(lvalp, PUNCTUATION);
}
{string} {
    process_token(lvalp, STRING);
}
. { }
\n { }
%%

void process_token(struct alpha_token_t* lvalp, enum category cat){
    struct alpha_token_t *token_iter = token_head, *new_token;

    if(!lvalp){
        printf("Null list head given, exiting\n");
        exit(0);
    }

    if(!token_head){
        token_head = lvalp;

        token_head->line_number = yylineno;
        token_head->token_no = token_count++;
        token_head->category = cat;
        strcpy(token_head->text, yytext);
        token_head->next = NULL;

        return;
    }

    while(token_iter->next != NULL)
        token_iter = token_iter->next;

    new_token = (struct alpha_token_t*)malloc(sizeof(struct alpha_token_t));
    new_token->line_number = yylineno;
    new_token->token_no = token_count++;
    new_token->category = cat;
    strcpy(new_token->text, yytext);
    new_token->next = NULL;

    token_iter->next = new_token;
    return;
}

int yywrap(){
    return 1;
}