%{
#include <string.h>
#include "lex.h"
#include "parser.h"

struct alpha_token_t *token_head = NULL;
int token_count = 1;
%}

intconst            [0-9]+
ident               [a-zA-Z][a-zA-Z0-9_]*
string              \"
comment             "//"[^\n\r]*
multiline_comment	"/""*"

%option yylineno

%%
"if"            {return IF;}
"else"          {return ELSE;}
"while"         {return WHILE;}
"for"           {return FOR;}
"function"      {return FUNCTION;}
"return"        {return RETURN;}
"break"         {return BREAK;}
"continue"      {return CONTINUE;}
"and"           {return AND;}
"not"           {return NOT;}
"or"            {return OR;}
"local"         {return LOCAL;}
"true"          {return TRUE;}
"false"         {return FALSE;}
"nil"           {return NIL;}
"["             {return LEFT_BRACE;}
"]"             {return RIGHT_BRACE;}
"{"             {return LEFT_BRACKET;}
"}"             {return RIGHT_BRACKET;}
"("             {return LEFT_PARENTHESIS;}
")"             {return RIGHT_PARENTHESIS;}
";"             {return SEMICOLON;}
","             {return COMMA;}
":"             {return COLON;}
"::"            {return DOUBLE_COLON;}
"."             {return DOT;}
".."            {return DOUBLE_DOT;}
"="             {return EQUALS;}
"+"             {return PLUS;}
"-"             {return MINUS;}
"*"             {return MULT;}
"/"             {return DIV;}
"%"             {return MOD;}
"=="            {return EQUALS_EQUALS;}
"!="            {return NOT_EQUALS;}
"++"            {return PLUS_PLUS;}
"--"            {return MINUS_MINUS;}
">"             {return GREATER_THAN;}
"<"             {return LESS_THAN;}
">="            {return GREATER_OR_EQUAL;}
"<="            {return LESS_OR_EQUAL;}

{intconst} {
   // yylval.numValue = atoi(yytext);
   	int d;
	sscanf(yytext, "%d", &d);
	yylval.numValue = d;
    return NUM;
}

{intconst}\.{intconst} {
	double f;
	sscanf(yytext, "%lf", &f);
	yylval.numValue = f;
    return NUM;
}

{ident} {
    yylval.strValue = strdup(yytext);
    return ID;
}

{string} {
    yylval.strValue = strdup(process_string());
    return STRING;
}

{comment} {
	
}

{multiline_comment} {
	process_multiline();
}

\n { }
[ \t\r] ;   // Ignore whitespaces and carriage return
. {
    if(strcmp(yytext, " ") != 0)
        printf("Invalid characted %s encountered, skipping...\n", yytext);
}
%%





/*
* Processes and adds token with given category, text, etc. into the token list
* 
* params:
* 
* lvalp             list head
* cat               token general category
* text              token string representation
* starting_line     comment starting line, only used with multiline or nested comments
* token_category    specific category, only used with comments
*/
// void process_token(struct alpha_token_t* lvalp, enum category cat, char* text, unsigned long starting_line, enum token_category token_category){
//     struct alpha_token_t *token_iter = token_head, *new_token;

//     // Check for given empty head
//     if(!lvalp){
//         printf("Null list head given, exiting\n");
//         exit(0);
//     }

//     // Check if its the first time processing
//     if(!token_head){
//         token_head = lvalp;

//         // For block comments, yylineno at the end of the processing will be the finishing line
//         if(token_category == BLOCK_COMMENT || token_category == NESTED_COMMENT){
//             token_head->line_number = starting_line;
//             token_head->comment_ending_line_no = yylineno;
//         }else
//            token_head->line_number = yylineno;

//         token_head->token_no = token_count++;
//         token_head->category = cat;
//         strcpy(token_head->text, text);
//         if(cat == KEYWORD || cat == OPERATOR || cat == PUNCTUATION) token_head->token_category = find_token_type(cat, text);
//         if(cat == COMMENT)
//             token_head->token_category = token_category;
//         token_head->next = NULL;

//         return;
//     }

//     // Append new token at end of list
//     while(token_iter->next != NULL)
//         token_iter = token_iter->next;

//     new_token = (struct alpha_token_t*)malloc(sizeof(struct alpha_token_t));
    
//     if(token_category == BLOCK_COMMENT || token_category == NESTED_COMMENT){
//         new_token->line_number = starting_line;
//         new_token->comment_ending_line_no = yylineno;
//     }else
//         new_token->line_number = yylineno;

//     new_token->token_no = token_count++;
//     new_token->category = cat;
//     strcpy(new_token->text, text);
//     if(cat == KEYWORD || cat == OPERATOR || cat == PUNCTUATION) new_token->token_category = find_token_type(cat, text);
//     if(cat == COMMENT)
//             new_token->token_category = token_category;
//     new_token->next = NULL;

//     token_iter->next = new_token;
//     return;
// }

/*
* Processes a multiline comment, recursively processing more nested comments inside it until the closing comment is met
* 
* params:
* 
* lvalp             list head
* tokencat          the token specific category, used for recursion to tell apart nested comments
*/
void process_multiline(){
    int c;

    // Loop input
    while(1){
        c = input();

        // Check for EOF before closing comment, throw error
        if(c == EOF || c == 0){
            printf("Comment unclosed at end of file\n");
            break;
        }

        // Check for nested comment and recursively call for that
        if(c == '/'){
            if((c = input()) == '*'){
                process_multiline();
            }else
                unput(c);
        }

        // Closing comment
        if(c == '*'){
            if((c = input()) == '/')
                break;
            else
                unput(c);
        }
    }
    return;
}

/*
* Processes current input into a string until second " is met and replaces all escape characters with their hex equivalent
* If a string is unclosed at end of file, an error is printed
* 
* params:
* 
* lvalp             list head
* 
* returns char*     the processed string
*/
char* process_string() {
	char* str = (char*)malloc(512 * sizeof(char));
	char c;
	int i = 0, j = 0;
	
    // Loop input
	while (1) {
		c = input();

        // Check for EOF before end of string
		if (c == EOF || c == 0) {
			printf("String unclosed at end of file\n");
			exit(0);
		}else if (c == '"') {
			break; // End of string
		}

        // Escape character starting
		if (c == '\\') {
			
            // Switch next character
			c = input();
			switch (c) {
			case 'a':
				str[j] = 0x07;
				break;
			case 'e':
				str[j] = 0x1B;
				break;
			case 'f':
				str[j] = 0x0C;
				break;
			case 'n':
				str[j] = 0x0A;
				break;
			case 'r':
				str[j] = 0x0D;
				break;
			case 't':
				str[j] = 0x09;
				break;
			case 'v':
				str[j] = 0x0B;
				break;
			case '\\':
				str[j] = 0x5C;
				break;
			case '\'':
				str[j] = 0x27;
				break;
			case '\"':
				str[j] = 0x22;
				break;
			case '?':
				str[j] = 0x3F;
				break;
			default:
				printf("Warning: invalid escape character \\%c\n", c);
				str[j] = '\\';
				str[++j] = c;
				break;
			}
			i++;
			j++;
		}else {
			if (c != '\"') {
				str[j] = c;
				j++;
			}
		}

		i++;
	}
	str[j] = '\0';

	return str;
}

int yywrap(){
    return 1;
}