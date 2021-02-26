struct alpha_token_t{
    int line_number;
    char name[256];
};

extern FILE* yyin;
int alpha_yylex(void *ylval);