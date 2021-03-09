#include <stdio.h>
#include <stdlib.h>
#include "../include/lex.h"

/*
* General categories in string format used for printing
*/
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

/*
* Specific categories in string format used for printing
*/
const char* token_category_name[] = {
    "IF",
    "ELSE",
    "WHILE",
    "FOR",
    "FUNCTION",
    "RETURN",
    "BREAK",
    "CONTINUE",
    "AND",
    "NOT",
    "OR",
    "LOCAL",
    "TRUE",
    "FALSE",
    "NIL",
    "EQUALS",
    "PLUS",
    "MINUS",
    "MULT",
    "DIV",
    "MOD",
    "EQUALS_EQUALS",
    "NOT_EQUALS",
    "PLUS_PLUS",
    "MINUS_MINUS",
    "GREATER_THAN",
    "LESS_THAN",
    "GREATER_OR_EQUAL",
    "LESS_OR_EQUAL",
    "LEFT_BRACE",
    "RIGHT_BRACE",
    "LEFT_BRACKET",
    "RIGHT_BRACKET",
    "LEFT_PARENTHESIS",
    "RIGHT_PARENTHESIS",
    "SEMICOLON",
    "COMMA",
    "COLON",
    "DOUBLE_COLON",
    "DOT",
    "DOUBLE_DOT",
    "LINE_COMMENT",
    "BLOCK_COMMENT",
    "NESTED_COMMENT"
};

/*
* Returns token specific category enumeration given its string representation
*
* params:
*
* cat               token general category
* text              token string representation
*
* returns enum      the token's specific category
*/
enum token_category find_token_type(enum category cat, char* text) {
    if (cat == KEYWORD) {
        if (strcmp(text, "if") == 0) return IF;
        else if (strcmp(text, "else") == 0) return ELSE;
        else if (strcmp(text, "while") == 0) return WHILE;
        else if (strcmp(text, "for") == 0) return FOR;
        else if (strcmp(text, "function") == 0) return FUNCTION;
        else if (strcmp(text, "return") == 0) return RETURN;
        else if (strcmp(text, "break") == 0) return BREAK;
        else if (strcmp(text, "continue") == 0) return CONTINUE;
        else if (strcmp(text, "and") == 0) return AND;
        else if (strcmp(text, "not") == 0) return NOT;
        else if (strcmp(text, "or") == 0) return OR;
        else if (strcmp(text, "local") == 0) return LOCAL;
        else if (strcmp(text, "true") == 0) return TRUE;
        else if (strcmp(text, "false") == 0) return FALSE;
        else if (strcmp(text, "nil") == 0) return NIL;
    }else if (cat == OPERATOR) {
        if (strcmp(text, "=") == 0) return EQUALS;
        else if (strcmp(text, "+" ) == 0) return PLUS;
        else if (strcmp(text, "-" ) == 0) return MINUS;
        else if (strcmp(text, "*" ) == 0) return MULT;
        else if (strcmp(text, "/" ) == 0) return DIV;
        else if (strcmp(text, "%" ) == 0) return MOD;
        else if (strcmp(text, "==") == 0) return EQUALS_EQUALS;
        else if (strcmp(text, "!=") == 0) return NOT_EQUALS;
        else if (strcmp(text, "++") == 0) return PLUS_PLUS;
        else if (strcmp(text, "--") == 0) return MINUS_MINUS;
        else if (strcmp(text, ">" ) == 0) return GREATER_THAN;
        else if (strcmp(text, "<" ) == 0) return LESS_THAN;
        else if (strcmp(text, ">=") == 0) return GREATER_OR_EQUAL;
        else if (strcmp(text, "<=") == 0) return LESS_OR_EQUAL;
    }else if (cat == PUNCTUATION) {
        if (strcmp(text, "{") == 0) return LEFT_BRACE;
        else if (strcmp(text, "}") == 0) return RIGHT_BRACE;
        else if (strcmp(text, "[") == 0) return LEFT_BRACKET;
        else if (strcmp(text, "]") == 0) return RIGHT_BRACKET;
        else if (strcmp(text, "(") == 0) return LEFT_PARENTHESIS;
        else if (strcmp(text, ")") == 0) return RIGHT_PARENTHESIS;
        else if (strcmp(text, ";") == 0) return SEMICOLON;
        else if (strcmp(text, ",") == 0) return COMMA;
        else if (strcmp(text, ":") == 0) return COLON;
        else if (strcmp(text, "::") == 0) return DOUBLE_COLON;
        else if (strcmp(text, ".") == 0) return DOT;
        else if (strcmp(text, "..") == 0) return DOUBLE_DOT;
    }
}