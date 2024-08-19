// Copyright (C) by gimura 2024
// --- PREFIX ---
//
// C_* - working with configs
//
// --- DESCRIPTION ---
//
// config reading
//
// --- SYNTAX ---
//
// structure: PREFIX CONTENT;
// 
// PREFIX:
//     $ - once variable
//     * - array
//     # - comment
//
// CONTENT: any string
//
// --------------

#include "../../cl_def.h"

typedef struct {
    char value[64]; // variable content
    char name[32];  // variable name

    enum {
        VAR,   // once variable  ($)
        ARRAY, // array variable (*)
        NONE   // comment or empty string
    } type;
} conf_var_t; // variable type

// get variable in line
static conf_var_t C_GetLineVariable(const char* line) {
    conf_var_t var = {};

    // if line empty or comment, return variable with type NONE
    if (line == NULL)                  { var.type = NONE; goto done; }
    if (*line == '\0' || *line == '#') { var.type = NONE; goto done; }

    switch (*line) {
        case '$': // if once variable
            var.type = VAR;
            break;

        case '*': // if array
            var.type = ARRAY;
            break;

        default: ERROR("C_GetLineVariable: unexpected prefix %c", *line); // syntax error
    }

    // create buffer for strtok_r
    char* line_res = malloc(strlen(line)); // wtf, why only malloc working? it's a memory manager bug?
    memcpy(line_res, line, strlen(line));

    const char* name = strtok_r(line_res, " ", &line_res);  // get name 
    const char* value = strtok_r(line_res, ";", &line_res); // get content

    // copy to structure buffer
    memcpy(&var.name, name + 1, strlen(name)); 
    memcpy(&var.value, value, strlen(value));

    ASSERT(var.name != NULL, "C_GetLineVariable: name of variable not found");
    ASSERT(var.value != NULL, "C_GetLineVariable: value of variable not found");

    done: return var;
}

// get variable by name
char* C_GetVariable(char* source, const char* name) {
    char* line = strtok(source, "\n\r"); // get first line

    while (line != NULL) {
        const conf_var_t var = C_GetLineVariable(line); // get line variable
        line = strtok(NULL, "\n\r");                    // get next line

        if (var.type != VAR) continue;             // if var type not once variable ($), continue
        if (strcmp(var.name, name) != 0) continue; // if var name and finding var name not equals, continue

        // allocate return buffer
        char* value_ptr = M_TempAlloc(sizeof(var.value));
        memcpy(value_ptr, var.value, sizeof(var.value));

        return value_ptr;
    }

    return NULL; // if variable not found, return NULL
}

// get array from config
conf_array_t C_GetArray(char* source, const char* name) {
    conf_array_t array = {}; // create array variable

    // enumerate lines
    for(char* line = strtok(source, "\n\r"); line != NULL; line = strtok(NULL, "\n\r")) {
        const conf_var_t var = C_GetLineVariable(line);  // get variable in line

        if (var.type != ARRAY) continue;           // if var type not array, continue
        if (strcmp(var.name, name) != 0) continue; // if var name not equals, continue

        // allocate buffer for variable
        char* value_ptr = M_TempAlloc(sizeof(var.value));
        memcpy(value_ptr, var.value, sizeof(var.value));

        array.d[array.s++] = value_ptr; // push variable at last
    }

    return array; // return array
}