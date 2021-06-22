#include "tools.h"
#include "vm.h"
#include <iostream>


void VM::vmerr(std::string err) {
	unsigned err_line = this->instructions.at(this->pc).src_line;
	std::string error("misw th zwh mou:");
	error.append(std::to_string(err_line));
	error.append(": ");
	error.append(err);
	error.append("\n");
	std::cout << error;
	return;
}