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

#include "cl_def.h"

// command type
typedef struct {
    cmd_fn_t fn;       // pointer to command function
    char     name[64]; // name of command
} cmd_t;

static cmd_t      cmds[128]     = { NULL }; // command list
static cmd_var_t* cmd_vars[256] = { NULL }; // variables list
static usize      cmd_len     = 0; // count of commands
static usize      cmd_var_len = 0; // count of variables

// builtin console commands
int CMD_Echo(char* args);
int CMD_ExecCommand(char* args);

// init builtin console commands
void CMD_Init(void) {
    CMD_AddCommand("echo", &CMD_Echo);
    CMD_AddCommand("exec", &CMD_ExecCommand);
}

// add new command
void CMD_AddCommand(const char* name, cmd_fn_t func) {
    cmd_t cmd = {};                       // create command structure
    memcpy(cmd.name, name, strlen(name)); // copy name to structure
    cmd.fn = func;                        // set function pointer to target function

    cmds[cmd_len++] = cmd; // push command structure to last free index
}

// add console variable
void CMD_AddVariable(cmd_var_t* var) {
    cmd_vars[cmd_var_len++] = var; // push variable to last free index
}

// execute text
// TODO: optimize linear search (add hash maps)
int CMD_ExecuteText(const char* in) {
    if (*in == '#' || in == NULL || *in == '\0') return COMMENT; // if comment or null string, return

    int ret = VARIABLE; // set return status to variable

    char text[64] = {'\0'};       // create buffer for strtok
    memcpy(text, in, strlen(in)); // copy in variable to buffer
    text[strlen(in)] = '\0';      // set last char to '\0'

    const char* name = strtok(text, " "); // get name 

    for (usize i = 0; i < cmd_len; i++) {
        // if name and command name in index i equals, execute command fuction
        if (strcmp(cmds[i].name, name) == 0) {
            ret = cmds[i].fn(strtok(NULL, "")); // run command fuction
            goto done;
        }
    }

    for (usize i = 0; i < cmd_var_len; i++) {
        cmd_var_t* var = cmd_vars[i];

        // if name and variable name in index i equals
        if (strcmp(var->name, name) == 0) {
            const char* value = strtok(NULL, ""); // get all after name

            // if value is NULL, print variable value
            if (value == NULL) {
                char buf[32];
                snprintf(buf, sizeof(buf), "variable %s is %s\n", name, var->string);
                CON_Printf(buf);
            } else { // else parse value to variable fields
                memcpy(var->string, value, strlen(value)); // parse string variable
                sscanf(value, "%i", &var->integer);        // parse integer variable
                sscanf(value, "%f", &var->floating);       // parse floating variable
            }

            goto done;
        }
    }

    ret = COMMAND_NOT_FOUND; // if command or variable with name not found, set ret to command not found value 

    done:
        return ret; // return result
}

// echo command
int CMD_Echo(char* args) {
    // add \n to end
    char buf[64];
    snprintf(buf, sizeof(buf), "%s\n", args);

    CON_Printf(buf); // print variable
    return SUCCESS;
}

// exec command
int CMD_ExecCommand(char* args) {
    char  path[64];
    char* game_dir = SYS_GetGameDir();
    sprintf(path, "%s%s", game_dir, args);
    M_TempFree(game_dir);

    FILE* file = fopen(path, "r"); // open file in arguments
    if (file == NULL) return 2;    // if file not found, return

    // execute all lines from file
    char line[64];
    while (fgets(line, 64, file)) {
        if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0'; // remove \n in end
        CMD_ExecuteText(line);                                             // execute
    }

    fclose(file); // close file
    
    return SUCCESS;
}