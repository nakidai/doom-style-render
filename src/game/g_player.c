#include "../cl_def.h"

extern state_t g_cState;
extern vidstate_t g_cVidstate;
playerstate_t g_cPlayerstate;

static void G_PlayerHandleKeys(void) {
    const u8* keystate = SDL_GetKeyboardState(NULL);
    const f32 move_speed = 0.3f * 0.016f;

    g_cPlayerstate.camera.anglecos = cos(g_cPlayerstate.camera.angle);
    g_cPlayerstate.camera.anglesin = sin(g_cPlayerstate.camera.angle);

    if (keystate[SDLK_UP & 0xFFFF]) {
        P_SetVel(
            &g_cPlayerstate.phys_obj,
            (v3) {
                (move_speed * g_cPlayerstate.camera.anglecos),
                (move_speed * g_cPlayerstate.camera.anglesin),
                g_cPlayerstate.phys_obj.vel.z,
            }
        );
    }

    if (keystate[SDLK_DOWN & 0xFFFF]) {
        P_SetVel(
            &g_cPlayerstate.phys_obj,
            (v3) {
                -(move_speed * g_cPlayerstate.camera.anglecos),
                -(move_speed * g_cPlayerstate.camera.anglesin),
                g_cPlayerstate.phys_obj.vel.z,
            }
        );
    }

    if (keystate[SDLK_LEFT & 0xFFFF] && g_cPlayerstate.phys_obj.pos.z == g_cState.map.sectors.arr[g_cPlayerstate.sector].zfloor) {
        P_AddVel(
            &g_cPlayerstate.phys_obj,
            (v3) {
                0.0f, 0.0f,
                -GRAVITY + 0.03f
            }
        );
    }

    if (keystate[SDLK_F1 & 0xFFFF]) {
        g_cState.sleepy = true;
    }

    if (keystate[SDLK_ESCAPE & 0xFFFF]) {
        g_cState.quit = true;
    }
}

#define SENS 0.01f
#define MIN_VERT_ANG -0.5
#define MAX_VERT_ANG  0.5

static void G_HandleMouse(void) {
    int width, height;
    SDL_GetWindowSize(g_cVidstate.window, &width, &height);

    int x, y;
    SDL_GetMouseState(&x, &y);

    g_cPlayerstate.camera.angle += 0.01f * (width / 2 - x);
    g_cPlayerstate.camera.vert_angle -= 0.01f * (height / 2 - y);
    g_cPlayerstate.camera.vert_angle = clamp(g_cPlayerstate.camera.vert_angle, MIN_VERT_ANG, MAX_VERT_ANG);

    SDL_WarpMouseInWindow(g_cVidstate.window, width / 2, height / 2);
}

static inline float G_PointSide(v2 p, v2i a, v2i b) {
    return -(((p.x - a.x) * (b.y - a.y))
           - ((p.y - a.y) * (b.x - a.x))); 
}

// point is in sector if it is on the left side of all walls
static bool G_PointInSector(const sector_t* sector, v2 p) {
    for (usize i = 0; i < sector->nwalls; i++) {
        const wall_t* _wall = &g_cState.map.walls.arr[sector->firstwall + i];

        if (G_PointSide(p, _wall->a, _wall->b) > 0) {
            return false;
        }
    }

    return true;
}

static void G_UpdatePlayerSector(void) {
    // BFS neighbors in a circular queue, player is likely to be in one
    // of the neighboring sectors
    enum { QUEUE_MAX = 64 };
    int
        queue[QUEUE_MAX] = { g_cPlayerstate.sector },
        i = 0,
        n = 1,
        found = SECTOR_NONE;

    while (n != 0) {
        // get front of queue and advance to next
        const int id = queue[i];
        i = (i + 1) % (QUEUE_MAX);
        n--;

        const sector_t* sector = &g_cState.map.sectors.arr[id];

        if (G_PointInSector(sector, g_cPlayerstate.camera.pos)) {
            found = id;
            break;
        }

        // check neighbors
        for (usize j = 0; j < sector->nwalls; j++) {
            const wall_t* wall =
                &g_cState.map.walls.arr[sector->firstwall + j];

            if (wall->portal) {
                if (n == QUEUE_MAX) {
                    // fprintf(stderr, "out of queue space!");
                    goto done;
                }

                queue[(i + n) % QUEUE_MAX] = wall->portal;
                n++;
            }
        }
    }

    done:
        if (!found) {
            g_cPlayerstate.sector = 1;
        }
        else {
            g_cPlayerstate.sector = found;
        }
}

static void G_UpdateEye(void) {
    const int sector_id = g_cPlayerstate.sector;
    const sector_t* player_sector = &g_cState.map.sectors.arr[sector_id];

    g_cPlayerstate.camera.eye_z = player_sector->zfloor + PLAYER_EYE_Z;
}

void G_InitPlayer(void) {
    g_cPlayerstate.sector = 1;
    g_cPlayerstate.phys_obj.pos.x = 3;
    g_cPlayerstate.phys_obj.pos.y = 3;
    g_cPlayerstate.phys_obj.pos.z = 10;

    G_UpdatePlayerSector();
    G_UpdateEye();

    SDL_ShowCursor(SDL_DISABLE);
}

void G_UpdatePlayer(void) {
    G_HandleMouse();
    G_PlayerHandleKeys();

    P_AddVel(&g_cPlayerstate.phys_obj, (v3) { 0, 0, GRAVITY });
    P_UpdateObject(&g_cPlayerstate.phys_obj);

    if (g_cPlayerstate.phys_obj.pos.z < g_cState.map.sectors.arr[g_cPlayerstate.sector].zfloor) {
        g_cPlayerstate.phys_obj.vel.z = 0;
        g_cPlayerstate.phys_obj.pos.z = g_cState.map.sectors.arr[g_cPlayerstate.sector].zfloor;
    }

    g_cPlayerstate.camera.pos.x = g_cPlayerstate.phys_obj.pos.x;
    g_cPlayerstate.camera.pos.y = g_cPlayerstate.phys_obj.pos.y;
    g_cPlayerstate.camera.eye_z = g_cPlayerstate.phys_obj.pos.z + PLAYER_EYE_Z;

    G_UpdatePlayerSector();
}

void G_FreePlayer(void) {}