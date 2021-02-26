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
        printf("Found token %s in line %d\n", token.name, token.line_number);
    }

    return 0;
}