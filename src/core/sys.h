#ifndef _sys_h
#define _sys_h

#include "../utils/u_def.h"

void SYS_Init(int argc, char* argv[]);
char* SYS_GetArg(const char* name);

#endif