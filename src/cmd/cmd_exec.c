#include "../cl_def.h"

typedef struct {
    cmd_fn_t fn;
    char     name[64];
} cmd_t;

static cmd_t cmds[1024] = { NULL };
static usize cmd_len = 0;

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

int CMD_ExecuteText(char* text) {
    int ret = WTF_WHY;

    const char* name = strtok(text, " ");

    for (usize i = 0; i < cmd_len; i++) {
        if (strcmp(cmds[i].name, name) == 0) {
            ret = cmds[i].fn(strtok(NULL, ""));
            goto done;
        }
    }

    ret = COMMAND_NOT_FOUND;

    done: return ret;
}