#include "tools.h"

inline void bp(void){
    asm __volatile__ (".byte 0xc3");
    return;
}

void alpha_error(char *error, ...){
    char *iter, *s;
    unsigned int i;

    va_list arg;
    va_start(arg, error);

    va_end(arg);
}