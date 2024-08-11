#ifndef _r_draw_h
#define _r_draw_h

#include "../../utils/u_def.h"

extern void ASMD_VertLine(int y0, int y1, int x, u32 color);
extern void ASMD_TextLine(int y0, int y1, int x, int u, int v);

void D_VertLine(int y0, int y1, int x, u32 color);
void D_TextLine(int y0, int y1, int x, int tex_id, int u, int v, int add_v);

#endif