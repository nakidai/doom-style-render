#include "cl_def.h"

int main() {
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

    while (true) {
        SDL_Event ev;

        while (SDL_PollEvent(&ev)) {
            switch (ev.type) {
                case SDL_QUIT: goto exit;
                default: break;
            }
        }
    }

    exit:

    return 0;
}