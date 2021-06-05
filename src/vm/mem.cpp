#include "mem.h"

AVMStack::AVMStack() {
	for (unsigned i = 0; i < AVM_STACKSIZE; i++) {
		AVM_WIPEOUT(this->stack[i]);
		this->stack[i].type = AVM_memcell_t::undef_m;
	}
}