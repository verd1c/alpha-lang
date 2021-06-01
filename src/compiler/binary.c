#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "binary.h"

int mk_bin(char *fname) {
	FILE *f;

	f = fopen(fname, "wb");

	if (!f) {
		printf("AAAAAAAAAA\n");
		return 0;
	}

	bin = f;

	int magicnum = MAGIC_NUMBER;
	fwrite(&magicnum, sizeof(unsigned), 1, f);


	return 1;
}