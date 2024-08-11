#ifndef _r_render_h
#define _r_render_h

#include "../utils/u_def.h"
#include "utils/r_texture.h"

#define EYE_Z 1.0f
#define HFOV DEG2RAD(120.0f)
#define VFOV 0.5f

#define ZNEAR 0.0001f
#define ZFAR  128.0f

typedef struct {
    texture_t textures[256];
} render_state_t;

void R_Init(void);
void R_Render(void);
void R_Free(void);

#endif