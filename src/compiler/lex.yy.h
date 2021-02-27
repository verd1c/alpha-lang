enum category{
    IDENT,
    KEYWORD,
    OPERATOR,
    INTCONST,
    REALCONST,
    PUNCTUATION,
    STRING,
    COMMENT
};

extern const char* category_names[];

struct alpha_token_t{
    int line_number;
    int token_no;
    char text[256];
    enum category category;
};

extern FILE* yyin;
int alpha_yylex(void *ylval);
void processToken(enum category cat);