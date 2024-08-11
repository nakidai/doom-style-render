#include "../../cl_def.h"

typedef struct {
    char value[64];
    char name[32];

    enum {
        VAR,
        ARRAY,
        NONE
    } type;
} conf_var_t;

static conf_var_t C_GetLineVariable(const char* line) {
    conf_var_t var = {};

    if (line == NULL)                  { var.type = NONE; goto done; }
    if (*line == '\0' || *line == '#') { var.type = NONE; goto done; }

    switch (*line) {
        case '$':
            var.type = VAR;
            break;

        case '*':
            var.type = ARRAY;
            break;

        default: ERROR("C_GetLineVariable: unexpected prefix %c", *line);
    }

    char* line_res = malloc(strlen(line));
    memcpy(line_res, line, strlen(line));

    const char* name = strtok_r(line_res, " ", &line_res);
    const char* value = strtok_r(line_res, ";", &line_res);

    memcpy(&var.name, name + 1, strlen(name));
    memcpy(&var.value, value, strlen(value));

    ASSERT(var.name != NULL, "C_GetLineVariable: name of variable not found");
    ASSERT(var.value != NULL, "C_GetLineVariable: value of variable not found");

    done: return var;
}

char* C_GetVariable(char* source, const char* name) {
    char* line = strtok(source, "\n\r");

    while (line != NULL) {
        const conf_var_t var = C_GetLineVariable(line);
        line = strtok(NULL, "\n\r");

        if (var.type != VAR) continue;
        if (strcmp(var.name, name) != 0) continue;

        char* value_ptr = M_TempAlloc(sizeof(var.value));
        memcpy(value_ptr, var.value, sizeof(var.value));

        return value_ptr;
    }

    return NULL;
}

conf_array_t C_GetArray(char* source, const char* name) {
    conf_array_t array = {};

    for(char* line = strtok(source, "\n\r"); line != NULL; line = strtok(NULL, "\n\r")) {
        const conf_var_t var = C_GetLineVariable(line);

        if (var.type != ARRAY) continue;
        if (strcmp(var.name, name) != 0) continue;

        char* value_ptr = M_TempAlloc(sizeof(var.value));
        memcpy(value_ptr, var.value, sizeof(var.value));

        array.d[array.s++] = value_ptr;
    }

    return array;
}