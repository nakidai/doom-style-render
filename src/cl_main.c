// Copyright (C) by gimura 2024
// --- PREFIX ---
//
// CL_* - client code
//
// --- DESCRIPTION ---
//
// main file of client
//
// -------------------

#include "cl_def.h"

state_t             client_state; // client state
extern game_state_t game_state;   // link game state

// console command for exit
// only for client
int CMD_ExitCommand(char* args __attribute__((unused))) {
    client_state.quit = true;
    return SUCCESS;
}

// debug interrupt call
int CMD_DebugBreak(char* args __attribute__((unused))) {
    __asm__ __volatile__("int {$}3":);
    return SUCCESS;
}

// client initialization
static void CL_Init(int argc, char** argv) {
#   ifdef DEBUG_PRINT_INIT
    printf("CL_Init\n");
#   endif
    
    SYS_Init(argc, argv); // init SYS module
    M_Init(); // init memory manager

    srand(time(NULL)); // init random

    KEY_Init(); // init key manager

    CMD_Init(); // init command executor
    CON_Init(); // init console

    CMD_AddCommand("exit",        &CMD_ExitCommand); // add command for exit
    CMD_AddCommand("debug_break", &CMD_DebugBreak);  // add command for debug break

    V_Init();   // init video
    R_Init();   // render init

    G_Init();   // init game

    CMD_ExecuteText("exec game.cfg"); // execute game config
}

// process SDL events
static void CL_CheckWindowEvents(void) {
    client_state.event_count = 0; // sets event count no zero 
    SDL_Event ev;             // variable for event

    // event check loop
    while (SDL_PollEvent(&ev)) {
        client_state.events[client_state.event_count++] = ev; // write event to event array

        // check type
        switch (ev.type) {
            case SDL_QUIT: // if window close event, quit
                client_state.quit = true;
                break;
            
            default: // else other, continue
                break;
        }
    }
}

// main loop
static void CL_MainLoop(void) {
    // init variable for calculate delta time
    u64 now = SDL_GetPerformanceCounter(),
        last = 0;

    // main loop
    while (!client_state.quit) {
        CL_CheckWindowEvents(); // process events

        // if quit in true, break
        if (client_state.quit) {
            break;
        }

        // calculate delta time
        last = now;
        now = SDL_GetPerformanceCounter();

        client_state.delta_time = (double) ((now - last) * 1000 / (double) SDL_GetPerformanceFrequency());

        KEY_Update(); // process keys
        CON_Update(); // update console

        V_Update(); // update video (clear screen buffer)
        R_RenderPlayerView(&game_state.player); // render player view

        if (client_state.state == LEVEL_STATE) {
            G_Update(); // if console disabled, update game
            SDL_ShowCursor(SDL_DISABLE); // don't show cursor
        } else {
            SDL_ShowCursor(SDL_ENABLE); // if console enabled, show cursor
        }

        // GFX_Diseling(192, (v2i) { 0, 0 }, (v2i) { SCREEN_WIDTH, SCREEN_HEIGHT });

        if (client_state.state == CONSOLE_STATE) CON_Draw(); // drawing console

        if (!client_state.sleepy) { V_Present(); }
    }
}

// free memory and quit processes
static void CL_Free(void) {
#   ifdef DEBUG_PRINT_INIT
    printf("CL_Free\n");
#   endif

    CON_Free(); // free console
    V_Free();   // free video buffer
    R_Free();   // free renderer

    M_Free(); // free memory manager
}

// entry point for game
int main(int argc, char** argv) {
    CL_Init(argc, argv); // init game
    CL_MainLoop();       // main loop
    CL_Free();           // free memory

    return 0; // exit
}