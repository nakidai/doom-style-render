#include "cl_def.h"

state_t              g_cState;
extern playerstate_t g_cPlayerstate;
extern vidstate_t    g_cVidstate;

static void CL_Init(int argc, char** argv) {
#   ifdef DEBUG_PRINT_INIT
    printf("CL_Init\n");
#   endif
    
    SYS_Init(argc, argv);
    M_Init();

    srand(time(NULL));

    KEY_Init();

    W_LoadWAD(&g_cState.wad);
    G_LoadMap(&g_cState.map, "TEST");

    V_Init();
    CON_Init();
    R_Init();
    P_Init();
    G_InitPlayer();

    CMD_ExecuteText("exec ../../res/config.cfg");
}

static void CL_CheckWindowEvents(void) {
    g_cState.event_count = 0;
    SDL_Event ev;

    while (SDL_PollEvent(&ev)) {
        g_cState.events[g_cState.event_count++] = ev;

        switch (ev.type) {
            case SDL_QUIT:
                g_cState.quit = true;
                break;
            
            default:
                break;
        }
    }
}

static void CL_MainLoop(void) {
    u64 now = SDL_GetPerformanceCounter(),
        last = 0;

    while (!g_cState.quit) {
        CL_CheckWindowEvents();

        if (g_cState.quit) {
            break;
        }

        last = now;
        now = SDL_GetPerformanceCounter();

        g_cState.delta_time = (double) ((now - last) * 1000 / (double) SDL_GetPerformanceFrequency());

        KEY_Update();
        CON_Update();

        V_Update();
        R_Render();

        if (g_cState.state == LEVEL_STATE) {
            G_UpdatePlayer();
            SDL_ShowCursor(SDL_DISABLE);
        } else {
            SDL_ShowCursor(SDL_ENABLE);
        }

        // GFX_Diseling(192, (v2i) { 0, 0 }, (v2i) { SCREEN_WIDTH, SCREEN_HEIGHT });

        if (g_cState.state == CONSOLE_STATE) CON_Draw();

        if (!g_cState.sleepy) { V_Present(); }
    }
}

static void CL_Free(void) {
#   ifdef DEBUG_PRINT_INIT
    printf("CL_Free\n");
#   endif

    CON_Free();
    V_Free();
    R_Free();
    G_FreePlayer();
    W_CloseWAD(&g_cState.wad);

    M_Free();
}

int main(int argc, char** argv) {
    CL_Init(argc, argv);
    CL_MainLoop();
    CL_Free();

    return 0;
}