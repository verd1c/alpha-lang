#include <assert.h>
#include "tools.h"

inline void bp(void){
    //asm __volatile__ (".byte 0xc3");
    return;
}

void alpha_message(FILE* fp, enum message_t type, char* error, ...) {
    char *iter, *s;
    unsigned int i;

    switch (type) {
    case INFO:

        break;
    case MEMORY_ERROR:
        fprintf(fp, "alpha:%d: memory error\n", yylineno);
        break;
    default:
        assert(0);
    }

    va_list arg;
    va_start(arg, error);

    va_end(arg);
    return;
}