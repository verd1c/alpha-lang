#include <stdio.h>
#include <stdlib.h>
#include "../include/lex.h"

void print_token(struct alpha_token_t *token) {
    if (token->category == KEYWORD || token->category == OPERATOR || token->category == PUNCTUATION)
        printf("%d: #%d \"%s\" %s %s\n", token->line_number, token->token_no, token->text, category_names[token->category], token_category_name[token->token_category]);
    else if (token->category == INTCONST || token->category == REALCONST)
        printf("%d: #%d \"%s\" %s %s\n", token->line_number, token->token_no, token->text, category_names[token->category], token->text);
    else if (token->category == COMMENT && token->token_category == BLOCK_COMMENT || token->token_category == NESTED_COMMENT)
        printf("%d: #%d \"%d - %d\" %s %s\n", token->line_number, token->token_no, token->line_number, token->comment_ending_line_no, category_names[token->category], token_category_name[token->token_category]);
    else
        printf("%d: #%d \"%s\" %s \"%s\"\n", token->line_number, token->token_no, token->text, category_names[token->category], token->text);
}

int main(int argc, char** argv){
    FILE* fp;
    struct alpha_token_t token;

    if (argc == 1) {
        printf("Usage: ./a.out text_file_name\n");
        exit(0);
    }

    fp = fopen(argv[1], "r"); 
    yyin = fp;

    struct alpha_token_t *token_list = (struct alpha_token_t*)malloc(sizeof(struct alpha_token_t));
    yylex(token_list);

    while(token_list){
        print_token(token_list);
        token_list = token_list->next;
    }

    return 0;
}