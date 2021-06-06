// AVM.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <fstream>
#include <iostream>
#include "vm.h"
#include "vmarg.h"

int main()
{
    VM vm = VM();
    vm.parse("target.abc");
    vm.execute_cycle();
    vm.execute_cycle();
    vm.execute_cycle();
    vm.printInstructions();
}