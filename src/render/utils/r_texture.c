#include "../../cl_def.h"

void T_AllocTexture(texture_t* tex) {
    tex->data = M_GlobAlloc(tex->size.x * tex->size.y);
}

void T_FreeTexture(texture_t* tex) {
    M_GlobFree(tex->data);
}

void T_ReadTexture(texture_t* tex, const char* path) {
    BMP* bmp = bopen(path);
    tex->size = (v2i) { get_width(bmp), get_height(bmp) };
    T_AllocTexture(tex);
}

void T_GenDebugTexture(texture_t* tex) {
    const u32 black = 0xFF0000FF;
    const u32 red   = 0xFFFF00FF;

    const i32 half_x = tex->size.x / 2;
    const i32 half_y = tex->size.y / 2;

    for (int i = 0; i < tex->size.x * tex->size.y; i++) {
        u32 color = rand() & 1 ? black : red;
        tex->data[i] = color;
    }
}