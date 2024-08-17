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

cmd_var_t test_variable = { "test", "this is test variable" }; // variable for testing

extern state_t    g_cState;    // link game state
extern vidstate_t g_cVidstate; // link video state

// command for toggle console
int CMD_ToggleConsole(void) {
    // if console opened, close, else closed open
    g_cState.state = g_cState.state == CONSOLE_STATE ? LEVEL_STATE : CONSOLE_STATE;
    return SUCCESS;
}

// command for open map
int CMD_LoadMap(char* args) {
    G_LoadMap(&g_cState.map, args);
    return SUCCESS;
}

// init console
void CON_Init(void) {
    CON_DrawInit(); // init char drawing (load charset)

    CMD_AddCommand("toggle_console", (cmd_fn_t) &CMD_ToggleConsole); // add toggle console command
    CMD_AddCommand("map",                       &CMD_LoadMap);       // add map console command
    CMD_AddVariable(&test_variable);                                 // add test variable

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

// update console
void CON_Update(void) {
    for (usize i = 0; i < g_cState.event_count; i++) {
        SDL_Event ev = g_cState.events[i]; // get event
        
        switch (ev.type) {
            case SDL_TEXTINPUT: // if text input
                if (g_cState.state != CONSOLE_STATE) break; // if console not opened, break
                CON_ProcessInput(ev.text.text);             // process input
                break;

            case SDL_KEYDOWN: // if key down
                if (ev.key.keysym.sym == SDLK_RETURN) CON_Exec(); // if key ENTER, execute command buffer
                break;
            
            default:
                break;
        }
    }
}