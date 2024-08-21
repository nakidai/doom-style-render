// Copyright (C) by gimura 2024
// --- PREFIX ---
//
// CON_* - console functions
//
// --- DESCRIPTION ---
//
// char drawing
//
// -------------------

#include "cl_def.h"

static texture_t con_chars; // char set

void CON_DrawInit(void) {
    T_ReadTexture(&con_chars, "chars"); // load char set form game directory
}

void CON_DrawFree(void) {
    T_FreeTexture(&con_chars); // free char set texture
}

extern vidstate_t video_state; // link video state

// char drawing
void CON_DrawChar(v2i pos, const char c) {
    // register keyword in iterators for optimisation
    for (register int i = 0; i < CHAR_SIZE; i++) {
        for (register int j = 0; j < CHAR_SIZE; j++) {
            // skip pixel if it transparent
            if (con_chars.data[j * con_chars.size.x + (i + (int) c * CHAR_SIZE)] == 0xFF000000) continue;

            // set pixel in pixel buffer to current char pixel
            // I think it's too hard to read
            video_state.pixels[(j + pos.y) * SCREEN_WIDTH + (i + pos.x)]
                = con_chars.data[j * con_chars.size.x + (i + (int) c * CHAR_SIZE)];
        }
    }
}

// string drawing
void CON_DrawString(v2i pos, const char* text) {
    // I don't know how it works
    usize line = 0;
    v2i   loop_pos = pos;

    for (usize i = 0; i < strlen(text); i++) {
        loop_pos.x += CHAR_SIZE;
        if (text[i] == '\n') { line++; loop_pos.x = pos.x; loop_pos.y -= CHAR_SIZE; }

        if (loop_pos.x / CHAR_SIZE >= MAX_LINE_CHARS) continue;

        CON_DrawChar(loop_pos, text[i]);
    }
}