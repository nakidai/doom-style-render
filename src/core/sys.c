#include "../cl_def.h"

static int    argc;
static char** argv;

void SYS_Init(int _argc, char** _argv) {
    argc = _argc;
    argv = _argv;
}

char* SYS_GetArg(const char* name) {
    char* out = NULL;

    for (int i = 0; i < argc; i++) {
        if (strcmp(name, argv[i]) == 0) {
            out = argv[i + 1];
            goto done;
        }
    }

    done: return out;
}