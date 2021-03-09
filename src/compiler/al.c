#include <stdio.h>
#include <stdlib.h>
#include "../include/lex.h"

/*
* Prints a given token
*/
void print_token(struct alpha_token_t* token);

int main(int argc, char** argv){
    FILE* fp;
    struct alpha_token_t token;

    // Set input stream
    if (argc == 1) {
        printf("Usage: ./a.out text_file_name (-o output_file_name)\n");
        exit(0);
    }

    fp = fopen(argv[1], "r"); 
    yyin = fp;

    // Set output stream
    if (argc >= 3) {

        // Check for output parameter
        if (strcmp(argv[2], "-o") != 0) {
            printf("Unknown parameter %s\n", argv[2]);
            exit(0);
        }

        // Check for output file parameter
        if (argc != 4) {
            printf("No output file given\n");
            exit(0);
        }

        // Redirect stdout
        if (!freopen(argv[3], "w", stdout)) {
            printf("Could not open file for writing\n");
            exit(0);
        }
    }

    // Perform lexical analysis
    struct alpha_token_t *token_list = (struct alpha_token_t*)malloc(sizeof(struct alpha_token_t));
    alpha_yylex(token_list);

    // Print token list
    while(token_list){
        print_token(token_list);
        token_list = token_list->next;
    }

    return 0;
}

/*
* Prints a given token
*/
void print_token(struct alpha_token_t* token) {
    // Print token data
    if (token->category == KEYWORD || token->category == OPERATOR || token->category == PUNCTUATION)
        printf("%d: #%d \"%s\" %s %s", token->line_number, token->token_no, token->text, category_names[token->category], token_category_name[token->token_category]);
    else if (token->category == INTCONST || token->category == REALCONST)
        printf("%d: #%d \"%s\" %s %s", token->line_number, token->token_no, token->text, category_names[token->category], token->text);
    else if (token->category == COMMENT)
        if (token->token_category == BLOCK_COMMENT || token->token_category == NESTED_COMMENT)
            printf("%d: #%d \"%d - %d\" %s %s", token->line_number, token->token_no, token->line_number, token->comment_ending_line_no, category_names[token->category], token_category_name[token->token_category]);
        else
            printf("%d: #%d \"%s\" %s \"%s\"", token->line_number, token->token_no, token->text, category_names[token->category], token_category_name[token->token_category]);
    else
        printf("%d: #%d \"%s\" %s \"%s\"", token->line_number, token->token_no, token->text, category_names[token->category], token->text);

    // Print type
    if (token->category == STRING || token->category == IDENT)
        printf(" <-char*");
    else if (token->category == INTCONST)
        printf(" <-integer");
    else if (token->category == REALCONST)
        printf(" <-real");
    else
        printf(" <-enumerated");

    printf("\n");
    return;
}