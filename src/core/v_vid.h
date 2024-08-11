#ifndef _v_vid_h
#define _v_vid_h

#include "../utils/u_def.h"

#define INIT_WINDOW_W 1280 / 2
#define INIT_WINDOW_H 720 / 2

typedef struct {
    u32* pixels;
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture, * debug;
} vidstate_t;

void V_Init(void);
void V_Present(void);
void V_Update(void);
void V_Free(void);

#endif