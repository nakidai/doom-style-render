#ifndef _f_conf_h
#define _f_conf_h

#include "../../utils/u_def.h"

#define MAX_ARRAY_SIZE 256

typedef struct {
    usize s;
    char* d[MAX_ARRAY_SIZE];
} conf_array_t;

char* C_GetVariable(char* source, const char* name);
conf_array_t C_GetArray(char* source, const char* name);

#endif