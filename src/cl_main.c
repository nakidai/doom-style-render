#include "cl_def.h"

state_t g_cState;
extern playerstate_t g_cPlayerstate;

static void CL_Init(int argc, char** argv) {
#   ifdef DEBUG_PRINT_INIT
    printf("CL_Init\n");
#   endif
    
    SYS_Init(argc, argv);
    M_Init();

    srand(time(NULL));

    W_LoadWAD(&g_cState.wad);
    G_LoadMap(&g_cState.map, "TEST");

    V_Init();
    R_Init();
    G_InitPlayer();
}

static void CL_CheckWindowEvents(void) {
    SDL_Event ev;

    while (SDL_PollEvent(&ev)) {
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
    while (!g_cState.quit) {
        CL_CheckWindowEvents();

        if (g_cState.quit) {
            break;
        }

        G_UpdatePlayer();

        V_Update();
        R_Render();
        if (!g_cState.sleepy) { V_Present(); }
    }
}

static void CL_Free(void) {
#   ifdef DEBUG_PRINT_INIT
    printf("CL_Free\n");
#   endif

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