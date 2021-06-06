#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "binary.h"

int mk_bin(char *fname) {
	FILE *f;
	Instruction instr;
	int i;
	userfunc uf;
	double d;
	char *str;
	char buf[30];

	f = fopen(fname, "wb");

	if (!f) {
		printf("AAAAAAAAAA\n");
		return 0;
	}

	bin = f;

	int magicnum = MAGIC_NUMBER;
	fwrite(&magicnum, sizeof(unsigned), 1, f);

	// strigns
	fwrite(&currString, sizeof(unsigned), 1, f);
	for (i = 0; i < currString; i++) {
		str = stringConsts[i];
		fwrite(str, strlen(str) + 1, 1, f);
	}

	// nums
	fwrite(&currNum, sizeof(unsigned), 1, f);
	for (i = 0; i < currNum; i++) {
		sprintf(buf, "%lf", numConsts[i]);
		//printf("Writing Num : %lf\n", d);
		fwrite(buf, strlen(buf) + 1, 1, f);
	}

	// userfuncs
	fwrite(&currUser, sizeof(unsigned), 1, f);
	for (i = 0; i < currUser; i++) {
		uf = userFuncs[i];
		fwrite(&uf.address, sizeof(unsigned), 1, f);
		fwrite(&uf.local_size, sizeof(unsigned), 1, f);
		str = uf.id;
		fwrite(str, strlen(str) + 1, 1, f);
	}

	// libfuncs
	fwrite(&currLib, sizeof(unsigned), 1, f);
	for (i = 0; i < currLib; i++) {
		str = namedLibfuncs[i];
		fwrite(str, strlen(str) + 1, 1, f);
	}

	fwrite(&currInstr, sizeof(unsigned), 1, f);

	printf("SIZE: %d\n", sizeof(Instruction));
	for (i = 0; i < currInstr; i++)
	{
		instr = instructions[i];

		fwrite(&instr, sizeof(Instruction), 1, f);

		// if (instr.opcode == assign_v)
		// {

		// 	fwrite(&instr.opcode, 1, 1, f);
		// 	fwrite(&instr.result, sizeof(VMArg), 1, f);
		// 	fwrite(&instr.arg1, sizeof(VMArg), 1, f);
		// 	//fwrite(&instr.result.type, 1, 1, f);
		// 	//fwrite(&instr.result.val, 4, 1, f);
		// 	//fwrite(&instr.arg1.type, 1, 1, f);
		// 	//fwrite(&instr.arg1.val, 4, 1, f);
		// }
		// else if (instr.opcode == jump_v || instr.opcode == funcenter_v || instr.opcode == funcexit_v)
		// {
		// 	fwrite(&instr.opcode, 1, 1, f);
		// 	fwrite(&instr.result.type, 1, 1, f);
		// 	fwrite(&instr.result.val, 4, 1, f);
		// }
		// else if (instr.opcode == pusharg_v || instr.opcode == call_v)
		// {
		// 	fwrite(&instr.opcode, 1, 1, f);
		// 	fwrite(&instr.arg1.type, 1, 1, f);
		// 	fwrite(&instr.arg1.val, 4, 1, f);
		// }
		// else
		// {
		// 	fwrite(&instr.opcode, 1, 1, f);
		// 	fwrite(&instr.result.type, 1, 1, f);
		// 	fwrite(&instr.result.val, 4, 1, f);
		// 	fwrite(&instr.arg1.type, 1, 1, f);
		// 	fwrite(&instr.arg1.val, 4, 1, f);
		// 	fwrite(&instr.arg2.type, 1, 1, f);
		// 	fwrite(&instr.arg2.val, 4, 1, f);
		// }
	}
	return 1;
}