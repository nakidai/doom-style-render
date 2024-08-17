#include "cl_def.h"

typedef struct {
    char        command[64];
    int         code;
    key_event_t type;
} bind_t;

static key_handler_t handlers[128];
static bind_t        binds[128];
static usize handlers_len = 0;
static usize binds_len    = 0;

void KEY_HandleBinds(int code, key_event_t type) {
    for (usize i = 0; i < binds_len; i++) {
        if (binds[i].code == code && (binds[i].type == type || binds[i].type == KEY_ANY)) {            
            CMD_ExecuteText(binds[i].command);
        }
    }
}

int CMD_BindCommand(char* args) {
    const char* key_code_str   = strtok(args, " ");
    const char* key_press_type = strtok(NULL, " ");
    const char* command        = strtok(NULL, "");

    int key_code = (int) NAN;
    if (sscanf(
        key_code_str,
        "%i", &key_code
    ) == 1) goto done;
    if (sscanf(
        key_code_str,
        "%c", (char*) &key_code
    ) == 1) goto done;
    
    if (key_code == NAN) return 2;

    done:;
    key_event_t type;

    if      (strcmp(key_press_type, "DOWN") == 0) type = KEY_DOWN;
    else if (strcmp(key_press_type, "UP") == 0)   type = KEY_UP;
    else if (strcmp(key_press_type, "ANY") == 0)  type = KEY_ANY;
    else                                          return 2;

    bind_t bind = {};
    bind.code = key_code;
    bind.type = type;
    memcpy(bind.command, command, strlen(command));
    
    binds[binds_len++] = bind;

    return SUCCESS;
}

void KEY_Init(void) {
    KEY_AddKeyHandler(&KEY_HandleBinds);
    CMD_AddCommand("bind", &CMD_BindCommand);
}

extern state_t g_cState;

void KEY_Update(void) {
    for (usize i = 0; i < g_cState.event_count; i++) {
        SDL_Event ev = g_cState.events[i];

        switch (ev.type) {
            case SDL_KEYDOWN: {
                for (usize j = 0; j < handlers_len; j++) {
                    handlers[j](ev.key.keysym.sym, KEY_DOWN);
                }

                break;
            }

            case SDL_KEYUP: {
                for (usize j = 0; j < handlers_len; j++) {
                    handlers[j](ev.key.keysym.sym, KEY_UP);
                }

                break;
            }

            default:
                break;
        }
    }
}

void KEY_AddKeyHandler(key_handler_t fn) {
    handlers[handlers_len++] = fn;
}