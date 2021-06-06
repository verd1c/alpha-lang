#include <iostream>
#include <fstream>
#include "vmarg.h"
#include "vm.h"

VM::VM() {
	return;
}

char *breadstr(std::ifstream *f) {
	std::vector<char> strv;
	char *str;
	char c;
	int len = 0;

	f->read(reinterpret_cast<char *>(&c), sizeof(char));
	while (c != '\0') {
		len++;
		strv.push_back(c);

		f->read(reinterpret_cast<char *>(&c), sizeof(char));
	}

	str = (char *)malloc((len + 1) * sizeof(char));
	int i;
	for (i = 0; i < len; i++) {
		str[i] = strv.at(i);
	}
	str[i] = '\0';
	return str;
}

int VM::parse(const char *fname) {
	std::ifstream binary(fname, std::ios::in | std::ios::binary);

	if (!binary.is_open())
		return 0;

	binary.read(reinterpret_cast<char *>(&this->magic_number), sizeof(unsigned));

	unsigned len;
	binary.read(reinterpret_cast<char *>(&len), sizeof(unsigned));
	for (int i = 0; i < len; i++) {
		char *str = breadstr(&binary);
		this->strings.push_back(string(str));
		cout << "String: " << this->strings.front() << "\n";
	}

	binary.read(reinterpret_cast<char *>(&len), sizeof(unsigned));
	for (int i = 0; i < len; i++) {
		double d;
		char *str = breadstr(&binary);
		d = stod(str);
		//binary.read(reinterpret_cast<char *>(&d), sizeof(double));
		this->nums.push_back(d);
		cout << "Num : " << d << "\n";
	}

	binary.read(reinterpret_cast<char *>(&len), sizeof(unsigned));
	for (int i = 0; i < len; i++) {
		userfunc uf;
		binary.read(reinterpret_cast<char *>(&uf.address), sizeof(unsigned));
		binary.read(reinterpret_cast<char *>(&uf.local_size), sizeof(unsigned));
		uf.id = breadstr(&binary);
		userfuncs.push_back(uf);
		cout << "Read Userfunc\n";
	}

	binary.read(reinterpret_cast<char *>(&len), sizeof(unsigned));
	for (int i = 0; i < len; i++) {
		char *str = breadstr(&binary);
		this->libfuncs.push_back(string(str));
		cout << "Libfunc: " << this->libfuncs.front() << "\n";
	}


	// read instrs
	binary.read(reinterpret_cast<char *>(&this->instruction_len), sizeof(unsigned));
	for (int i = 0; i < this->instruction_len; i++) {
		Instruction instr;
		memset(&instr, 0, sizeof(Instruction));
		binary.read(reinterpret_cast<char *>(&instr), 32);

		this->instructions.push_back(instr);
	}

	binary.close();
	return 1;
}

unsigned int VM::getMagicNumber() {
	return this->magic_number;
}

void printArg(VMArg arg) {
	std::cout << "[" << arg.type << " : " << arg.val << "]";
	return;
}

const char *vmOpCodeToStr[] = {
	"assign_v", "add_v", "sub_v",
	"mul_v", "div_v", "mod_v",
	"uminus_v", "and_v", "or_v",
	"not_v", "jeq_v", "jne_v",
	"jle_v", "jge_v", "jlt_v",
	"jgt_v", "call_v", "pusharg_v",
	"funcenter_v", "funcexit_v", "newtable_v",
	"tablegetelem_v", "tablesetelem_v", "jump_v", "nop_v"
};

const char *vmArgOpToStr[] = {
	"label",
	"global",
	"formal",
	"local",
	"number",
	"string",
	"bool",
	"nil",
	"userfunc",
	"libfunc",
	"retval"
};

void encode_vmarg(FILE *out, VMArg arg) {

	fprintf(out, " [%s : %d] ", vmArgOpToStr[arg.type], arg.val);
}

void VM::printInstructions() {
	int i = 0;
	for (Instruction instr : this->instructions) {

		printf("%d : %s ", i++, vmOpCodeToStr[instr.op]);

		if (instr.op == assign_v) {
			encode_vmarg(stdout, instr.result);
			encode_vmarg(stdout, instr.arg1);
		}
		else if (instr.op == jump_v || instr.op == funcenter_v || instr.op == funcexit_v) {
			encode_vmarg(stdout, instr.result);
		}
		else if (instr.op == pusharg_v || instr.op == call_v) {
			encode_vmarg(stdout, instr.arg1);
		}
		else {
			encode_vmarg(stdout, instr.result);
			encode_vmarg(stdout, instr.arg1);
			encode_vmarg(stdout, instr.arg2);
		}
		std::cout << "\n";
	}
}

std::vector<Instruction> VM::getInstructions() {
	return this->instructions;
}

AVM_memcell *VM::translate_operand(VMArg *arg, AVM_memcell *reg) {
	switch (arg->type) {

	case VMArg_t::global:
		return &this->stack.stack[AVM_STACKSIZE - 1 - arg->val];
	case VMArg_t::local:
	case VMArg_t::formal:
	case VMArg_t::retval:
		break;
	}
}

void VM::execute_cycle(void) {
	for (Instruction instr : this->instructions) {
		(exec_funcs[instr.op])(&instr);
	}
}