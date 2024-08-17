#ifndef _sys_h
#define _sys_h

#include "../utils/u_def.h"

typedef struct {

} pack_t;

void SYS_Init(int argc, char* argv[]);

char* SYS_GetArg(const char* name);
char* SYS_GetGameDir(void);

u8*   SYS_ReadFile(const char* name);
usize SYS_ReadFileSize(const char* name);

#endif