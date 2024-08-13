#include "cl_def.h"

state_t       g_cState;
SDL_Renderer* renderer;

float scale = 50.0f;
v2 pos =      { 0, 0 };

static void ME_Render(void) {
    for (int i = 0; i < g_cState.map.walls.n; i++) {
        const wall_t* wall = &g_cState.map.walls.arr[i];

        if (wall->portal == 0) SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        else                   SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);

        SDL_RenderDrawLine(
            renderer,
            (wall->a.x + pos.x) * (int) scale,
            (wall->a.y + pos.y) * (int) scale,

            (wall->b.x + pos.x) * (int) scale,
            (wall->b.y + pos.y) * (int) scale
        );
    }

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);

    for (int i = 1; i < g_cState.map.sectors.n; i++) {
        const sector_t* sector = &g_cState.map.sectors.arr[i];
        v2i center = { 0, 0 };

        for (int j = sector->firstwall; j < sector->nwalls + sector->firstwall; j++) {
            const wall_t* wall = &g_cState.map.walls.arr[j];

            center.x += wall->a.x;
            center.y += wall->a.y;
            center.x += wall->b.x;
            center.y += wall->b.y;
        }

        center.x /= sector->nwalls * 2;
        center.y /= sector->nwalls * 2;

        char num[10];
        sprintf(num, "%i", sector->id);

        SDLTest_DrawString(
            renderer,
            (center.x + pos.x) * (int) scale,
            (center.y + pos.y) * (int) scale,
            num
        );
    }
}

#define MOVE_SPEED 0.4f

static void ME_ProcessEvents(void) {
    SDL_PumpEvents();
    const u8* keystate = SDL_GetKeyboardState(NULL);

    if (keystate[SDLK_UP & 0xFFFF])    pos.y += MOVE_SPEED;
    if (keystate[SDLK_DOWN & 0xFFFF])  pos.y -= MOVE_SPEED;
    if (keystate[SDLK_LEFT & 0xFFFF])  pos.x += MOVE_SPEED;
    if (keystate[SDLK_RIGHT & 0xFFFF]) pos.x -= MOVE_SPEED;
}

int main() {
    M_Init();

    ASSERT(
        !SDL_Init(SDL_INIT_VIDEO),
        "V_Init: SDL init error: %s",
        SDL_GetError()
    );

    SDL_Window* window = SDL_CreateWindow(
        "Haltura editor",
        SDL_WINDOWPOS_CENTERED_DISPLAY(0),
        SDL_WINDOWPOS_CENTERED_DISPLAY(0),
        INIT_WINDOW_W,
        INIT_WINDOW_H,
        0
    );

    ASSERT(
        window != NULL,
        "V_Init: failed to create SDL window: %s",
        SDL_GetError()
    );

    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED |
        SDL_RENDERER_PRESENTVSYNC
    );

    W_LoadWAD(&g_cState.wad);
    G_LoadMap(&g_cState.map, "TEST");

    while (true) {
        SDL_Event ev;

        while (SDL_PollEvent(&ev)) {
            switch (ev.type) {
                case SDL_QUIT: goto exit;
                default: break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        ME_Render();
        ME_ProcessEvents();

        SDL_RenderPresent(renderer);
    }

    exit:
    
    M_Free();
    return 0;
}