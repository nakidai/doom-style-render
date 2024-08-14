#include "../../cl_def.h"

void T_AllocTexture(texture_t* tex) {
    tex->data = M_GlobAlloc(tex->size.x * tex->size.y);
}

void T_FreeTexture(texture_t* tex) {
    M_GlobFree(tex->data);
}

void T_ReadTexture(texture_t* tex, const char* path) {
    BMP* bmp = bopen((char*) path);
    tex->size = (v2i) { get_width(bmp), get_height(bmp) };
    T_AllocTexture(tex);

    for (int i = 0; i < tex->size.x; i++) {
        for (int j = 0; j < tex->size.y; j++) {
            int r, g, b;
            get_pixel_rgb(bmp, i, j, &r, &g, &b);

            tex->data[j * tex->size.x + i] = r;
        }
    }
}

void T_GenDebugTexture(texture_t* tex) {
    const u32 black = 0xFF0000FF;
    const u32 red   = 0xFFFF00FF;

    for (int i = 0; i < tex->size.x * tex->size.y; i++) {
        u32 color = rand() & 1 ? black : red;
        tex->data[i] = color;
    }
}