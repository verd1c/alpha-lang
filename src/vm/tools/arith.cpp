#include "../tools.h"

arithmetic_func_t impl::arithmeticFuncs[] = {
		impl::add,
		impl::sub,
		impl::mul,
		impl::div,
		impl::mod,
		impl::minus
};

double impl::add(double x, double y) {
	return x + y;
}

double impl::sub(double x, double y) {
	return x - y;
}

double impl::mul(double x, double y) {
	return x * y;
}

double impl::div(double x, double y) {
	if (y == 0)
	{
		printf("Error:Division with zero attempted.\n");
	}
	return x / y;
}

double impl::mod(double x, double y) {
	if (y == 0)
	{
		printf("Error:Division with zero attempted.\n");
	}
	return ((unsigned)x) % ((unsigned)y);
}

double impl::minus(double x, double y) {
	if (x == 0)
	{
		printf("Error:Cannot uminus zero.\n");
	}
	return (x * y);
}
