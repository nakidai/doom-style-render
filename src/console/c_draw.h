#ifndef _c_draw_h
#define _c_draw_h

#include "utils/u_def.h"

#define CHAR_SIZE 16

void CON_DrawInit(void);
void CON_DrawFree(void);

void CON_DrawChar(v2i pos, const char c);
void CON_DrawString(v2i pos, const char* text);

#endif