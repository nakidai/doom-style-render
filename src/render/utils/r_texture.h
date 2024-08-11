#ifndef _r_texture_h
#define _r_texture_h

#include "../../utils/u_def.h"

typedef struct {
    u32* data;
    v2i size, offset;
} texture_t;

void T_AllocTexture(texture_t* tex);
void T_ReadTexture(texture_t* tex, const char* path);
void T_GenDebugTexture(texture_t* tex);
void T_FreeTexture(texture_t* tex);

#endif