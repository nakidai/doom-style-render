#include "cl_def.h"
#include <SDL2/SDL_test.h>

state_t       g_cState;
SDL_Renderer* renderer;
TTF_Font*     font;

float scale = 50.0f;
v2 pos =      { 0, 0 };

static void ME_DrawString(int x, int y, const char* line) {
#   ifndef __WIN32
    SDL_Color color = { 255, 255, 255, 255 };
    SDL_Surface *surf = TTF_RenderText_Blended(font, line, color);
    if (surf == NULL) ERROR("ME_DrawString: kakoito error");

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surf);
    if (texture == NULL) ERROR("ME_DrawString: kakoito error");

    SDL_FreeSurface(surf);
    
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = 24 * strlen(line);
    rect.h = 24;

    SDL_RenderCopy(renderer, texture, NULL, &rect);
#   else
    SDLTest_DrawString(renderer, x, y, line);
#   endif
}

bool draw_wall_num = true;
bool draw_sect_num = true;

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

        if (draw_wall_num) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);

            v2i center = { 0, 0 };
            center.x += wall->a.x;
            center.y += wall->a.y;
            center.x += wall->b.x;
            center.y += wall->b.y;

            center.x /= 2;
            center.y /= 2;

            char num[10];
            sprintf(num, "%i", i);

            ME_DrawString(
                (center.x + pos.x) * (int) scale,
                (center.y + pos.y) * (int) scale,
                num
            );
        }
    }

    if (draw_sect_num) {
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

            ME_DrawString(
                (center.x + pos.x) * (int) scale,
                (center.y + pos.y) * (int) scale,
                num
            );
        }
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

int ME_AddPoint(char* args) {
    v2i pos = { NAN, NAN };

    if (*args == 'c') {
        int x, y;
        SDL_GetMouseState(&x, &y);

        pos.x = x / scale;
        pos.y = y / scale;

        goto done;
    } else {
        if (sscanf(
            args,
            "%i %i",
            &pos.x,
            &pos.y
        ) != 2) return 2;

        goto done;
    }

    done:
        return SUCCESS;
}

int ME_Scale(char* args) {
    if (sscanf(
        args,
        "%f",
        &scale
    ) != 1) return 2;
    return SUCCESS;
}

static void ME_InitCommands(void) {
    CMD_AddCommand("ap", &ME_AddPoint);
    CMD_AddCommand("s", &ME_Scale);
}

static char console_buf[64] = {};

int main(int argc, char* argv[]) {
    SYS_Init(argc, argv);
    M_Init();
    CMD_Init();

    ASSERT(
        !SDL_Init(SDL_INIT_VIDEO),
        "V_Init: SDL init error: %s",
        SDL_GetError()
    );

#   ifndef __WIN32
    TTF_Init();
    font = TTF_OpenFont("./../../res/Sans.ttf", 24);
#   endif

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

    ME_InitCommands();

    while (true) {
        SDL_Event ev;

        while (SDL_PollEvent(&ev)) {
            switch (ev.type) {
                case SDL_QUIT: goto exit;
                case SDL_TEXTINPUT:
                    strcat(console_buf, ev.text.text);
                    break;

                case SDL_KEYDOWN: {
                    if (ev.key.keysym.sym != SDLK_RETURN) break;

                    printf("push command: %s\n", console_buf);

                    char* buf_ptr = M_TempAlloc(strlen(console_buf));
                    memcpy(buf_ptr, console_buf, strlen(console_buf));
                    memset(buf_ptr + strlen(console_buf), '\0', sizeof(console_buf) - strlen(console_buf));

                    CMD_ExecuteText(buf_ptr);
                    M_TempFree(buf_ptr);
                    memset(console_buf, '\0', sizeof(console_buf));

                    break;
                }

                default: break;
            }
        } 

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        ME_Render();

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        ME_DrawString(10, 10, console_buf);

        ME_ProcessEvents();

        SDL_RenderPresent(renderer);
    }

    exit:
    
    M_Free();
    CMD_Init();
    return 0;
}