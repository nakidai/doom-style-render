#include "../cl_def.h"

vidstate_t g_cVidstate;
u32*       g_pVidbuf;

void V_Init(void) {
    ASSERT(
        !SDL_Init(SDL_INIT_VIDEO),
        "V_Init: SDL init error: %s",
        SDL_GetError()
    );

    g_cVidstate.window = SDL_CreateWindow(
        "Haltura engine",
        SDL_WINDOWPOS_CENTERED_DISPLAY(0),
        SDL_WINDOWPOS_CENTERED_DISPLAY(0),
        INIT_WINDOW_W,
        INIT_WINDOW_H,
        0
    );

    ASSERT(
        g_cVidstate.window != NULL,
        "V_Init: failed to create SDL window: %s",
        SDL_GetError()
    );

    g_cVidstate.renderer = SDL_CreateRenderer(
        g_cVidstate.window,
        -1,
        SDL_RENDERER_ACCELERATED |
        SDL_RENDERER_PRESENTVSYNC
    );

    g_cVidstate.texture = SDL_CreateTexture(
        g_cVidstate.renderer,
        SDL_PIXELFORMAT_ABGR8888,
        SDL_TEXTUREACCESS_STREAMING,
        SCREEN_WIDTH,
        SCREEN_HEIGHT
    );

    g_cVidstate.debug = SDL_CreateTexture(
        g_cVidstate.renderer,
        SDL_PIXELFORMAT_ABGR8888,
        SDL_TEXTUREACCESS_TARGET,
        128,
        128
    );

    g_cVidstate.pixels = M_GlobAlloc(SCREEN_WIDTH * SCREEN_HEIGHT * 4);
    g_pVidbuf = g_cVidstate.pixels;
}

void V_Present(void) {
    void* px;
    int pitch;

    SDL_LockTexture(g_cVidstate.texture, NULL, &px, &pitch);
    {
        for (usize y = 0; y < SCREEN_HEIGHT; y++) {
            memcpy(
                &((u8*)px)[y * pitch],
                &g_cVidstate.pixels[y * SCREEN_WIDTH],
                SCREEN_WIDTH * 4
            );
        }
    }
    SDL_UnlockTexture(g_cVidstate.texture);

    SDL_SetRenderTarget(g_cVidstate.renderer, NULL);
    SDL_SetRenderDrawColor(g_cVidstate.renderer, 0, 0, 0, 0xFF);
    SDL_SetRenderDrawBlendMode(g_cVidstate.renderer, SDL_BLENDMODE_NONE);

    SDL_RenderClear(g_cVidstate.renderer);
    SDL_RenderCopyEx(
        g_cVidstate.renderer,
        g_cVidstate.texture,
        NULL,
        NULL,
        0.0,
        NULL,
        SDL_FLIP_VERTICAL
    );

    SDL_SetTextureBlendMode(g_cVidstate.debug, SDL_BLENDMODE_BLEND);
    SDL_RenderCopy(g_cVidstate.renderer, g_cVidstate.debug, NULL, &((SDL_Rect) { 0, 0, 512, 512 }));
    SDL_RenderPresent(g_cVidstate.renderer);
}

void V_Update(void) {
#   ifdef DEV_BUILD
    memset(g_cVidstate.pixels, 0, SCREEN_WIDTH * SCREEN_HEIGHT * 4);
#   endif
}

void V_Free(void) {
    SDL_DestroyTexture(g_cVidstate.debug);
    SDL_DestroyTexture(g_cVidstate.texture);
    SDL_DestroyRenderer(g_cVidstate.renderer);
    SDL_DestroyWindow(g_cVidstate.window);

    M_GlobFree(g_cVidstate.pixels);
}