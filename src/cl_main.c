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

    W_LoadWAD(&g_cState.wad);
    G_LoadMap(&g_cState.map, "TEST");

    V_Init();
    CON_Init();
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

            case SDL_TEXTINPUT:
                if (g_cState.state != CONSOLE_STATE) break;
                CON_ProcessInput(ev.text.text);
                break;

            case SDL_KEYDOWN:
                if (ev.key.keysym.sym == SDLK_RETURN) CON_Exec();
                if (ev.key.keysym.sym != SDLK_DELETE) break;
            
                if (g_cState.state == CONSOLE_STATE) g_cState.state = LEVEL_STATE;
                else                                 g_cState.state = CONSOLE_STATE;
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

        G_UpdatePlayer();

        V_Update();
        R_Render();

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