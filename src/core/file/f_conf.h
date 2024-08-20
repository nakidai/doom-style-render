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

#ifndef _f_conf_h
#define _f_conf_h

#include "cl_def.h"

#define MAX_ARRAY_SIZE 256 // max array size

typedef struct {
    usize s;                 // size of array
    char* d[MAX_ARRAY_SIZE]; // data in array
} conf_array_t; // config array structure

char*     C_GetVariable(char* source, const char* name); // get once variable
conf_array_t C_GetArray(char* source, const char* name); // get array

#endif