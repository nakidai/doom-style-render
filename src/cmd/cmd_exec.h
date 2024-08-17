// Copyright (C) by gimura 2024
// --- PREFIX ---
//
// CMD_* - working with commands
//
// --- DESCRIPTION ---
//
// command executor
//
// -------------------

#ifndef _cmd_exec_h
#define _cmd_exec_h

#include "utils/u_def.h"

#define SUCCESS           0   // success command exit code
#define COMMAND_NOT_FOUND 1   // command not found
#define VARIABLE          255 // variable exit code
#define COMMENT           256 // comment exit code

#define READ_FLOAT_VAR(x) ({ if (sscanf(args, "%f", &x) != 1) return 2; return SUCCESS; }) // macro from read float variable
#define READ_INT_VAR(x)   ({ if (sscanf(args, "%i", &x) != 1) return 2; return SUCCESS; }) // macro from read int variable

typedef int (*cmd_fn_t)(char*); // command fuction
typedef struct {
    char name[32];  // variable name
    char value[64]; // variable value
} cmd_var_t; // variable

void CMD_Init(void); // init console

void CMD_AddCommand(const char* name, cmd_fn_t func); // add command
void CMD_AddVariable(cmd_var_t* var);                 // add variable
int CMD_ExecuteText(const char* text);                // execute text

#endif