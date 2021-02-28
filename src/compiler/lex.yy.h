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
    unsigned long line_number;
    unsigned long token_no;
    char text[256];
    enum category category;

    struct alpha_token_t* next;
};

extern FILE* yyin;
int yylex (struct alpha_token_t* lvalp);
void process_token(struct alpha_token_t* lvalp, enum category cat);