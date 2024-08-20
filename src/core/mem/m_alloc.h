// Copyright (C) by gimura 2024
// --- PREFIX ---
//
// M_* - memory functions
//
// --- DESCRIPTION ---
//
// working with memory
//
// -------------------

#ifndef _m_alloc_h
#define _m_alloc_h

#include "cl_def.h"

#define TEMP_HEAP KB(256) // default temp heap size
#define GLOB_HEAP MB(14)  // default glob heap size

#define TEMP_COLLECT_RATE 16 // collect rate for temp buffer
#define GLOB_COLLECT_RATE 32 // collect rate for glob buffer

#define MEM_BLOCK_SIG 0x4CE0CA70 // normal mem block signature
#define OLD_BLOCK_SIG 0xDEADB104 // cleaned mem block signature

void M_Init(void);  // init memory manager
void M_Free(void);  // free memory manager
void M_Debug(void); // print memory blocks

void* M_TryGlobAlloc(usize size); // try alloc in glob buffer (if error return NULL)
void* M_TryTempAlloc(usize size); // try alloc in temp buffer (if error return NULL)

void* M_GlobAlloc(usize size); // glob alloc
void* M_TempAlloc(usize size); // temp alloc

void M_GlobFree(void* ptr); // free memory in glob buffer
void M_TempFree(void* ptr); // free memory in temp buffer

void M_GlobCollect(void); // collect free blocks in glob buffer
void M_TempCollect(void); // collect free blocks in temp buffer

#endif