#include "../../cl_def.h"

extern vidstate_t g_cVidstate;
extern render_state_t g_cRenderstate;

void D_VertLine(int y0, int y1, int x, u32 color) {
    for (int y = y0; y <= y1; y++) {
        g_cVidstate.pixels[y * SCREEN_WIDTH + x] = color;
    }
}