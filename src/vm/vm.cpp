#include <iostream>
#include <fstream>
#include "vmarg.h"
#include "vm.h"

VM::VM() {
	return;
}

int VM::parse(const char *fname) {
	std::ifstream binary(fname, std::ios::in | std::ios::binary);

	if (!binary.is_open())
		return 0;

	binary.read(reinterpret_cast<char *>(&this->magic_number), sizeof(unsigned));
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

void VM::printInstructions() {
	for (Instruction instr : this->instructions) {

		std::cout << "[" << "]  " << instr.op << " ";
		printArg(instr.result);
		printArg(instr.arg1);
		printArg(instr.arg2);
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
	}
}