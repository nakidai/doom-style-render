#ifndef _m_alloc_h
#define _m_alloc_h

#include "../../utils/u_def.h"
#include "m_utils.h"

#define TEMP_HEAP KB(256)
#define GLOB_HEAP MB(14)

#define TEMP_COLLECT_RATE 25
#define GLOB_COLLECT_RATE 50

#define MEM_BLOCK_SIG 0x4CE0CA70
#define OLD_BLOCK_SIG 0xDEADB104

void M_Init(void);
void M_Free(void);
void M_Debug(void);

void* M_TryGlobAlloc(usize size);
void* M_TryTempAlloc(usize size);

void* M_GlobAlloc(usize size);
void* M_TempAlloc(usize size);

void M_GlobFree(void* ptr);
void M_TempFree(void* ptr);

void M_GlobCollect(void);
void M_TempCollect(void);

#endif