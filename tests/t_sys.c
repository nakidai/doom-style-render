#include "cl_def.h"

static void T_SysGetArg(void) {
    char* args[] = {"-first_arg", "first_value", "-second_arg"};

    SYS_Init(3, args);

    if (strcmp(SYS_GetArg("-first_arg"), "first_value") != 0) ERROR("T_SysGetArg: get argument error");
    if (SYS_GetArg("-null_arg") != NULL)           ERROR("T_SysGetArg: get argument error");
    if (SYS_GetArg("-second_arg") != NULL)         ERROR("T_SysGetArg: get argument error");
}

int main() {
    T_SysGetArg();

    return 0;
}