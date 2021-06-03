// AVM.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <fstream>
#include <iostream>
#include "vmarg.h"

int main()
{
    Compiler compiler = Compiler();
    compiler.parse("target.abc");

    compiler.printInstructions();
}