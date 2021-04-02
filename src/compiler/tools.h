#ifndef __TOOLS_H__
#define __TOOLS_H__
#include <stdarg.h>

extern int yylineno;

inline void bp(void);

void alpha_error(char *error, ...);

#endif