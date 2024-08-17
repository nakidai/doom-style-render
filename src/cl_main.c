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

state_t              g_cState;       // game state
extern playerstate_t g_cPlayerstate; // player state
extern vidstate_t    g_cVidstate;    // video state

// console command for exit
// only for client
int CMD_ExitCommand(void) {
    g_cState.quit = true;
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

    W_LoadWAD(&g_cState.wad);         // load wad file
    G_LoadMap(&g_cState.map, "TEST"); // load TEST map

    CMD_Init(); // init command executor
    CON_Init(); // init console

    CMD_AddCommand("exit", &CMD_ExitCommand); // add command for exit

    V_Init();   // init video
    R_Init();   // render init

    P_Init();   // physics init
    G_InitPlayer(); // player init

    CMD_ExecuteText("exec ../../res/config.cfg"); // execute game config
}

// process SDL events
static void CL_CheckWindowEvents(void) {
    g_cState.event_count = 0; // sets event count no zero 
    SDL_Event ev;             // variable for event

    // event check loop
    while (SDL_PollEvent(&ev)) {
        g_cState.events[g_cState.event_count++] = ev; // write event to event array

        // check type
        switch (ev.type) {
            case SDL_QUIT: // if window close event, quit
                g_cState.quit = true;
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
    while (!g_cState.quit) {
        CL_CheckWindowEvents(); // process events

        // if quit in true, break
        if (g_cState.quit) {
            break;
        }

        // calculate delta time
        last = now;
        now = SDL_GetPerformanceCounter();

        g_cState.delta_time = (double) ((now - last) * 1000 / (double) SDL_GetPerformanceFrequency());

        KEY_Update(); // process keys
        CON_Update(); // update console

        V_Update(); // update video (clear screen buffer)
        R_Render(); // render level

        if (g_cState.state == LEVEL_STATE) {
            G_UpdatePlayer(); // if console disabled, update player and etc.
            SDL_ShowCursor(SDL_DISABLE); // don't show cursor
        } else {
            SDL_ShowCursor(SDL_ENABLE); // if console enabled, show cursor
        }

        // GFX_Diseling(192, (v2i) { 0, 0 }, (v2i) { SCREEN_WIDTH, SCREEN_HEIGHT });

        if (g_cState.state == CONSOLE_STATE) CON_Draw(); // drawing console

        if (!g_cState.sleepy) { V_Present(); }
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
    G_FreePlayer(); // free player
    W_CloseWAD(&g_cState.wad); // close wad file (free wad buffer)

    M_Free(); // free memory manager
}

// entry point for game
int main(int argc, char** argv) {
    CL_Init(argc, argv); // init game
    CL_MainLoop();       // main loop
    CL_Free();           // free memory

    return 0; // exit
}