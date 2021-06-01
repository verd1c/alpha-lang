#ifndef __TOOLS_H__
#define __TOOLS_H__
#include <stdarg.h>
#include <stdio.h>

enum message_t {
	INFO,
	MEMORY_ERROR
};

extern int yylineno;

inline void bp(void);

void alpha_message(FILE* fp, enum message_t type, char *error, ...);

#endif