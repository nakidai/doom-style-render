#include "../cl_def.h"

typedef struct {
    cmd_fn_t fn;
    char     name[64];
} cmd_t;

static cmd_t      cmds[1024]     = { NULL };
static cmd_var_t* cmd_vars[1024] = { NULL };
static usize      cmd_len     = 0;
static usize      cmd_var_len = 0;

void CMD_Init(void) {

}

void CMD_Free(void) {

}

void CMD_AddCommand(const char* name, cmd_fn_t func) {
    cmd_t cmd = {};
    memcpy(cmd.name, name, strlen(name));
    cmd.fn   = func;

    cmds[cmd_len++] = cmd;
}

void CMD_AddVariable(cmd_var_t* var) {
    cmd_vars[cmd_var_len++] = var;
}

int CMD_ExecuteText(const char* in) {
    if (*in == '#' || in == NULL || *in == '\0') return COMMENT;

    int ret = VARIABLE;

    char* text = M_TempAlloc(strlen(in) + 1);
    memcpy(text, in, strlen(in));
    text[strlen(in)] = '\0';

    const char* name = strtok(text, " ");

    for (usize i = 0; i < cmd_len; i++) {
        if (strcmp(cmds[i].name, name) == 0) {
            ret = cmds[i].fn(strtok(NULL, ""));
            goto done;
        }
    }

    for (usize i = 0; i < cmd_var_len; i++) {
        if (strcmp(cmd_vars[i]->name, name) == 0) {
            const char* value = strtok(NULL, "");

            if (value == NULL) {
                char buf[32];
                snprintf(buf, sizeof(buf), "variable %s is %s\n", name, cmd_vars[i]->value);
                CON_Printf(buf);
            }
            else memcpy(cmd_vars[i]->value, value, strlen(value));

            goto done;
        }
    }

    ret = COMMAND_NOT_FOUND;

    done:
        M_TempFree(text);
        return ret;
}