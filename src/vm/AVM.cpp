#include <fstream>
#include <iostream>
#include "vm.h"
#include "vmarg.h"

// watch
VM *debug;

int main()
{
    // pain
    VM vm = VM();
    debug = &vm;
    vm.parse("target.abc");
    while (!vm.exec_finished) {
        vm.execute_cycle();
    }
    return 1;
}