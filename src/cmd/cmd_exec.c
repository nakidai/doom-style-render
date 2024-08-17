#include "../cl_def.h"

typedef struct {
    cmd_fn_t fn;
    char     name[64];
} cmd_t;

static cmd_t      cmds[128]     = { NULL };
static cmd_var_t* cmd_vars[256] = { NULL };
static usize      cmd_len     = 0;
static usize      cmd_var_len = 0;

int CMD_Echo(char* args);
int CMD_ExecCommand(char* args);

void CMD_Init(void) {
    CMD_AddCommand("echo", &CMD_Echo);
    CMD_AddCommand("exec", &CMD_ExecCommand);
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

    char text[64] = {'\0'};
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
        return ret;
}

int CMD_Echo(char* args) {
    char buf[64];
    snprintf(buf, sizeof(buf), "%s\n", args);

    CON_Printf(buf);
    return SUCCESS;
}

int CMD_ExecCommand(char* args) {
    FILE* file = fopen(args, "r");

    if (file == NULL) return 2;

    char line[64];
    while (fgets(line, 64, file)) {
        if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';
        CMD_ExecuteText(line);
    }

    fclose(file);
    
    return SUCCESS;
}