%{
#include <string.h>
#include "../include/lex.h"

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
int token_count = 0;
%}

keyword             if|else|while|for|function|return|break|continue|and|not|or|local|true|false|nil
operator            (\=)|(\+)|(\-)|(\*)|(\/)|(\%)|(\=\=)|(\!\=)|(\+\+)|(\-\-)|(\>)|(\<)|(\>\=)|(\<\=)
intconst            [0-9]+
punctuation         (\{)|(\})|(\[)|(\])|(\()|(\))|(\;)|(\,)|(\:)|(\:\:)|(\.)|(\.\.)
ident               [a-zA-Z][a-zA-Z0-9_]*
string              \"
comment             "//"[^\r]*
multiline_comment   "/*"

%option yylineno

%%
{keyword} {
    process_token(lvalp, KEYWORD, yytext, 0, 0);
}
{multiline_comment} {
    process_multiline(lvalp, BLOCK_COMMENT);
}
{ident} {
    process_token(lvalp, IDENT, yytext, 0, 0);
}
{comment} {
    process_token(lvalp, COMMENT, yytext, 0, LINE_COMMENT);
}
{operator} {
    process_token(lvalp, OPERATOR, yytext, 0, 0);
}
{intconst} {
    process_token(lvalp, INTCONST, yytext, 0, 0);
}
{intconst}\.{intconst} {
    process_token(lvalp, REALCONST, yytext, 0, 0);
}
{punctuation} {
    process_token(lvalp, PUNCTUATION, yytext, 0, 0);
}
{string} {
    process_token(lvalp, STRING, process_string(lvalp), 0, 0);
}
. { }
\n { }
%%

void process_token(struct alpha_token_t* lvalp, enum category cat, char* text, unsigned long starting_line, enum token_category token_category){
    struct alpha_token_t *token_iter = token_head, *new_token;

    if(!lvalp){
        printf("Null list head given, exiting\n");
        exit(0);
    }

    if(!token_head){
        token_head = lvalp;

        if(token_category == BLOCK_COMMENT || token_category == NESTED_COMMENT){
            token_head->line_number = starting_line;
            token_head->comment_ending_line_no = yylineno;
        }else
           token_head->line_number = yylineno;

        token_head->token_no = token_count++;
        token_head->category = cat;
        strcpy(token_head->text, text);
        if(cat == KEYWORD || cat == OPERATOR || cat == PUNCTUATION) token_head->token_category = find_token_type(cat, text);
        if(cat == COMMENT)
            token_head->token_category = token_category;
        token_head->next = NULL;

        return;
    }

    while(token_iter->next != NULL)
        token_iter = token_iter->next;

    new_token = (struct alpha_token_t*)malloc(sizeof(struct alpha_token_t));
    
    if(token_category == BLOCK_COMMENT || token_category == NESTED_COMMENT){
        new_token->line_number = starting_line;
        new_token->comment_ending_line_no = yylineno;
    }else
        new_token->line_number = yylineno;

    new_token->token_no = token_count++;
    new_token->category = cat;
    strcpy(new_token->text, text);
    if(cat == KEYWORD || cat == OPERATOR || cat == PUNCTUATION) new_token->token_category = find_token_type(cat, text);
    if(cat == COMMENT)
            new_token->token_category = token_category;
    new_token->next = NULL;

    token_iter->next = new_token;
    return;
}

void process_multiline(struct alpha_token_t* lvalp, enum token_category tokencat){
    unsigned long starting_line = yylineno;
    int c, lci;

    while(1){
        c = input();
        if(c == EOF || c == 0){
            printf("Comment unclosed at end of file\n");
            break;
        }

        if(c == '/'){
            if((c = input()) == '*'){
                process_multiline(lvalp, NESTED_COMMENT);
            }else
                unput(c);
        }

        if(c == '*'){
            if((c = input()) == '/')
                break;
            else
                unput(c);
        }
    }

    process_token(lvalp, COMMENT, " ", starting_line, tokencat);
    return;
}

char* process_string(struct alpha_token_t* lval) {
	char* str = (char*)malloc(256 * sizeof(char));
	char c;
	int i = 0, j = 0;
	
	while (1) {
		c = input();

		if (c == EOF || c == 0) {
			printf("String unclosed at end of file\n");
			exit(0);
		}else if (c == '"') {
			break;
		}

		if (c == '\\') {
			c = input();
			switch (c) {
			case 'a':
				str[j] = 0x07;
				break;
			case 'e':
				str[j] = 0x1B;
				break;
			case 'f':
				str[j] = 0x0C;
				break;
			case 'n':
				str[j] = 0x0A;
				break;
			case 'r':
				str[j] = 0x0D;
				break;
			case 't':
				str[j] = 0x09;
				break;
			case 'v':
				str[j] = 0x0B;
				break;
			case '\\':
				str[j] = 0x5C;
				break;
			case '\'':
				str[j] = 0x27;
				break;
			case '\"':
				str[j] = 0x22;
				break;
			case '?':
				str[j] = 0x3F;
				break;
			default:
				printf("Warning: invalid escape character \\%c\n", c);
				str[j] = '\\';
				str[++j] = c;
				break;
			}
			i++;
			j++;
		}else {
			if (c != '\"') {
				str[j] = c;
				j++;
			}
		}

		i++;
	}
	str[j] = '\0';

	return str;
}

int yywrap(){
    return 1;
}