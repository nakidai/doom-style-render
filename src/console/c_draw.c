#include "cl_def.h"

static texture_t con_chars;

void CON_DrawInit(void) {
    T_ReadTexture(&con_chars, "chars");
}

void CON_DrawFree(void) {
    T_FreeTexture(&con_chars);
}

extern vidstate_t g_cVidstate;
extern char con_buf[1024];

void CON_DrawChar(v2i pos, const char c) {
    for (int i = 0; i < CHAR_SIZE; i++) {
        for (int j = 0; j < CHAR_SIZE; j++) {
            if (con_chars.data[j * con_chars.size.x + (i + (int) c * CHAR_SIZE)] == 0xFF000000) continue;
            g_cVidstate.pixels[(j + pos.y) * SCREEN_WIDTH + (i + pos.x)] = con_chars.data[j * con_chars.size.x + (i + (int) c * CHAR_SIZE)];
        }
    }
}

void CON_DrawString(v2i pos, const char* text) {
    usize line = 0;
    v2i   loop_pos = pos;

    for (usize i = 0; i < strlen(text); i++) {
        loop_pos.x += CHAR_SIZE;

        if (text[i] == '\n') { line++; loop_pos.x = pos.x; loop_pos.y -= CHAR_SIZE; }
        if (loop_pos.x >= SCREEN_WIDTH) { line++; loop_pos.x = pos.x; loop_pos.y -= CHAR_SIZE; }
        if (loop_pos.y <= 0) { loop_pos = pos; memset(con_buf, '\0', sizeof(con_buf)); }

        CON_DrawChar(loop_pos, text[i]);
    }
}