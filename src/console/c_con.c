// Copyright (C) by gimura 2024
// --- PREFIX ---
//
// CON_* - console functions
//
// --- DESCRIPTION ---
//
// engine console
//
// -------------------

#include "cl_def.h"

char con_buf[1024];     // console output buffer
static char con_in[32]; // console input buffer

extern vidstate_t   video_state;  // link video state
extern game_state_t game_state;   // link game state

// init console
void CON_Init(void) {
    CON_DrawInit(); // init char drawing (load charset)
    CON_Printf("console init done!\n"); // print done to console
}

// free console
void CON_Free(void) {
    CON_DrawFree(); // free charset
}

// draw console
void CON_Draw(void) {
    GFX_Blackout(128, (v2i) { 0, 0 }, (v2i) { SCREEN_WIDTH, SCREEN_HEIGHT }); // black screen buffer

    CON_DrawString((v2i) { 5, SCREEN_HEIGHT - 10 }, con_buf); // draw console buffer

    // add "]" to start con_in buffer
    char buf[64];
    snprintf(buf, sizeof(buf), "] %s", con_in);

    CON_DrawString((v2i) { 10, 10 }, buf); // draw line
}

// print to console
void CON_Printf(const char* msg) {
    strcat(con_buf, msg); // add message to back
}

// process input
static void CON_ProcessInput(const char* text) {
    if (strlen(con_in) >= sizeof(con_in)) return; // if input buffer string size > max length of input buffer, return
    strcat(con_in, text);                         // add new text to input buffer
}

// exec command in input buffer
static void CON_Exec(void) {
    if (strlen(con_in) == 0) return; // if command buffer is NULL, return

    // execute command
    char out_buf[32];
    snprintf(out_buf, sizeof(out_buf), "Command exited with code %i\n", CMD_ExecuteText(con_in));
    CON_Printf(out_buf);

    // free console input buffer
    memset(con_in, '\0', sizeof(con_in));
}

extern usize     event_count; // app event count
extern SDL_Event events[128]; // app event list

// update console
void CON_Update(void) {
    for (usize i = 0; i < event_count; i++) {
        SDL_Event ev = events[i]; // get event
        
        switch (ev.type) {
            case SDL_TEXTINPUT: // if text input
                CON_ProcessInput(ev.text.text); // process input
                break;

            case SDL_KEYDOWN: // if key down
                if (ev.key.keysym.sym == SDLK_RETURN) CON_Exec(); // if key ENTER, execute command buffer
                break;
            
            default:
                break;
        }
    }
}