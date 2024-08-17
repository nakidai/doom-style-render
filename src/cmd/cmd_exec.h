#ifndef _cmd_exec_h
#define _cmd_exec_h

#include "../utils/u_def.h"

#define SUCCESS           0
#define COMMAND_NOT_FOUND 1
#define VARIABLE          255
#define COMMENT           256

#define READ_FLOAT_VAR(x) ({ if (sscanf(args, "%f", &x) != 1) return 2; return SUCCESS; })
#define READ_INT_VAR(x)   ({ if (sscanf(args, "%i", &x) != 1) return 2; return SUCCESS; })

typedef int (*cmd_fn_t)(char*);
typedef struct {
    char name[32];
    char value[64];
} cmd_var_t;

void CMD_Init(void);
void CMD_Free(void);

void CMD_AddCommand(const char* name, cmd_fn_t func);
void CMD_AddVariable(cmd_var_t* var);
int CMD_ExecuteText(const char* text);

#endif