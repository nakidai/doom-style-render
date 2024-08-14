#ifndef _cmd_exec_h
#define _cmd_exec_h

#include "../utils/u_def.h"

#define SUCCESS           0
#define COMMAND_NOT_FOUND 1
#define WTF_WHY           255

typedef int (*cmd_fn_t)(char*);

void CMD_Init(void);
void CMD_Free(void);

void CMD_AddCommand(const char* name, cmd_fn_t func);
int CMD_ExecuteText(char* text);

#endif