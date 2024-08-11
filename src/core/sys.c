#include "../cl_def.h"

static int    argc;
static char** argv;

void SYS_Init(int _argc, char** _argv) {
    argc = _argc;
    argv = _argv;
}

char* SYS_GetArg(const char* name) {
    char* out = NULL;

    for (char* tok = strtok(argv, " "); tok != NULL; tok = strtok(NULL, " ")) {
        if (strcmp(tok, name) == 0) {
            out = strtok(NULL, " ");
            goto done;
        }
    }

    done: return out;
}