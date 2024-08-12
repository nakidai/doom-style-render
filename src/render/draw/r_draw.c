#include "../../cl_def.h"

extern vidstate_t g_cVidstate;
extern render_state_t g_cRenderstate;

void D_VertLine(int y0, int y1, int x, u32 color) {
    ASMD_VertLine(y0, y1, x, color);
}

// u32* g_pTex = NULL;

// void D_TextLine(int y0, int y1, int x, int tex_id, int u, int v, int add_v) {
//     const texture_t* tex = &g_cRenderstate.textures[tex_id];

//     for (int y = y0; y <= y1; y++) {
//         g_cVidstate.pixels[y * SCREEN_WIDTH + x] = tex->data[0];
//         v += add_v;
//     }
// }