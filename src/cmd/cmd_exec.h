#ifndef _cmd_exec_h
#define _cmd_exec_h

#include "../utils/u_def.h"

typedef void (*cmd_fn_t)(int, char**);

void CMD_Init(void);
void CMD_Free(void);

void CMD_AddCommand(const char* name, cmd_fn_t func);
void CMD_ExecuteText(const char* text);

#endif