#include <stdio.h>
#include <stdlib.h>
#include "lex.yy.h"


int main(int argc, char** argv){
    struct alpha_token_t token;
    int r;

    FILE *fp; 
    char filename[50]; 
    printf("Enter the filename: \n"); 
    scanf("%s",filename); 
    fp = fopen(filename,"r"); 
    yyin = fp;

    struct alpha_token_t *token_list = (struct alpha_token_t*)malloc(sizeof(struct alpha_token_t));
    token_list->line_number = 25;
    yylex(token_list);

    while(token_list){
        printf("%d: #%d \"%s\" %s\n", token_list->line_number, token_list->token_no, token_list->text, category_names[token_list->category]);
        token_list = token_list->next;
    }

    return 0;
}