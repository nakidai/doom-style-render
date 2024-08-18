#include "cl_def.h"

extern vidstate_t video_state;

void GFX_Blackout(const u8 s, v2i left, v2i right) {
    for (int i = left.x; i < right.x; i++) {
        for (int j = left.y; j < right.y; j++) {
            u32* pixel = &video_state.pixels[j * SCREEN_WIDTH + i];
            *pixel = MATH_AbgrMul(*pixel, s);
        }
    }
}

void GFX_Diseling(const u8 s, v2i left, v2i right) {
    for (int i = left.x; i < right.x; i++) {
        for (int j = left.y; j < right.y; j++) {
            u32* pixel = &video_state.pixels[j * SCREEN_WIDTH + i];
            *pixel = (i % 2) == 0 && (j % 2) == 0 ? MATH_AbgrMul(*pixel, s) : *pixel;
        }
    }
}