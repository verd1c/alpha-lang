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


    while((r = alpha_yylex(&token)) != 0){
        printf("%d: #%d \"%s\" %s\n", token.line_number, token.token_no, token.text, category_names[token.category]);
    }

    return 0;
}