#ifndef __VM__
#define __VM__

#include "mem.h"

class VM {
	AVMStack stack;

public:
	void execution_cycle();
};

#endif