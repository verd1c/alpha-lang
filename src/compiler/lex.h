enum category {
    IDENT,
    KEYWORD,
    OPERATOR,
    INTCONST,
    REALCONST,
    PUNCTUATION,
    STRING,
    COMMENT
};

enum token_category {
    // Keywords
    IF,
    ELSE,
    WHILE,
    FOR,
    FUNCTION,
    RETURN,
    BREAK,
    CONTINUE,
    AND,
    NOT,
    OR,
    LOCAL,
    TRUE,
    FALSE,
    NIL,

    // Operators
    EQUALS,
    PLUS,
    MINUS,
    MULT,
    DIV,
    MOD,
    EQUALS_EQUALS,
    NOT_EQUALS,
    PLUS_PLUS,
    MINUS_MINUS,
    GREATER_THAN,
    LESS_THAN,
    GREATER_OR_EQUAL,
    LESS_OR_EQUAL,

    // Punctuation
    LEFT_BRACE,
    RIGHT_BRACE,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    LEFT_PARENTHESIS,
    RIGHT_PARENTHESIS,
    SEMICOLON,
    COMMA,
    COLON,
    DOUBLE_COLON,
    DOT,
    DOUBLE_DOT,

    // Comments
    LINE_COMMENT,
    BLOCK_COMMENT,
    NESTED_COMMENT
};

extern const char* token_category_name[];
extern const char* category_names[];

struct alpha_token_t{
    unsigned long line_number;
    unsigned long token_no;
    char text[256];
    enum category category;
    enum token_category token_category;
    unsigned long ending_line;

    struct alpha_token_t* next;
};

extern FILE* yyin;
int yylex (struct alpha_token_t* lvalp);
void process_token(struct alpha_token_t* lvalp, enum category cat, char* text, unsigned long starting_line, enum token_category token_category);
char* process_string(struct alpha_token_t* lvalp, char* yytext);
enum token_category find_token_type(enum category cat, char* text);