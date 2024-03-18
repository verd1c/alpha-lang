#include <fstream>
#include <iostream>
#include "vm.h"
#include "vmarg.h"

// watch
VM *debug;

int main(int argc, char **argv)
{
    // pain
    VM vm = VM();
    debug = &vm;
    const char *fname = argc > 1 ? argv[1] : "target.abc";
    if (!vm.parse(fname)) {
        std::cerr << "alpha-vm: cannot open " << fname << "\n";
        return 1;
    }
    while (!vm.exec_finished) {
        vm.execute_cycle();
    }
    return 0;
}