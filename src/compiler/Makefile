default:
	bison --yacc --defines --output=parser.c parser.y
	gcc -o calc parser.c scanner.c struct.c -g 