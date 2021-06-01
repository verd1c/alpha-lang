default:
	flex --outfile=scanner.c lex.l
	bison --yacc --defines --output=parser.c parser.y -v
	gcc -o calc parser.c scanner.c symbol_table.c intermediate.c tools.c stack.c -lm -g -Wall