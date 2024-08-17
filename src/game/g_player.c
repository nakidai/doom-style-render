#include "../cl_def.h"

extern state_t g_cState;
extern vidstate_t g_cVidstate;
playerstate_t g_cPlayerstate;

static bool forward = false;
static bool back    = false;
static bool left    = false;
static bool right   = false;
static bool jump    = false;

int CMD_PlusForward(void) {
    forward = true;
    return SUCCESS;
}

int CMD_MinusForward(void) {
    forward = false;
    return SUCCESS;
}

int CMD_PlusBack(void) {
    back = true;
    return SUCCESS;
}

int CMD_MinusBack(void) {
    back = false;
    return SUCCESS;
}

int CMD_PlusLeft(void) {
    left = true;
    return SUCCESS;
}

int CMD_MinusLeft(void) {
    left = false;
    return SUCCESS;
}

int CMD_PlusRight(void) {
    right = true;
    return SUCCESS;
}

int CMD_MinusRight(void) {
    right = false;
    return SUCCESS;
}

int CMD_PlusJump(void) {
    jump = true;
    return SUCCESS;
}

int CMD_MinusJump(void) {
    jump = false;
    return SUCCESS;
}

static void G_PlayerHandleKeys(void) {
    const u8* keystate = SDL_GetKeyboardState(NULL);
    const f32 move_speed = 0.003f;

    g_cPlayerstate.camera.anglecos = cos(g_cPlayerstate.camera.angle);
    g_cPlayerstate.camera.anglesin = sin(g_cPlayerstate.camera.angle);

    if (forward) {
        P_SetVel(
            &g_cPlayerstate.phys_obj,
            (v3) {
                (move_speed * g_cPlayerstate.camera.anglecos),
                (move_speed * g_cPlayerstate.camera.anglesin),
                g_cPlayerstate.phys_obj.vel.z,
            }
        );
    }

    if (back) {
        P_SetVel(
            &g_cPlayerstate.phys_obj,
            (v3) {
                -(move_speed * g_cPlayerstate.camera.anglecos),
                -(move_speed * g_cPlayerstate.camera.anglesin),
                g_cPlayerstate.phys_obj.vel.z,
            }
        );
    }

    if (left) {
        P_AddVel(
            &g_cPlayerstate.phys_obj,
            (v3) {
                -(move_speed * g_cPlayerstate.camera.anglesin),
                (move_speed * g_cPlayerstate.camera.anglecos),
                0.f
            }
        );
    }

    if (right) {
        P_AddVel(
            &g_cPlayerstate.phys_obj,
            (v3) {
                (move_speed * g_cPlayerstate.camera.anglesin),
                -(move_speed * g_cPlayerstate.camera.anglecos),
                0.f
            }
        );
    }

    g_cPlayerstate.phys_obj.vel.x = clamp(
        g_cPlayerstate.phys_obj.vel.x,
        -move_speed, move_speed
    );

    g_cPlayerstate.phys_obj.vel.y = clamp(
        g_cPlayerstate.phys_obj.vel.y,
        -move_speed, move_speed
    );

    if (jump && g_cPlayerstate.phys_obj.pos.z == g_cState.map.sectors.arr[g_cPlayerstate.sector].zfloor) {
        P_AddVel(
            &g_cPlayerstate.phys_obj,
            (v3) {
                g_cPlayerstate.phys_obj.vel.x * 0.1f,
                g_cPlayerstate.phys_obj.vel.y * 0.1f,
                -GRAVITY + 0.03f
            }
        );
    }

    if (keystate[SDLK_F1 & 0xFFFF]) {
        g_cState.sleepy = true;
    }
}

static float sens         = 0.005f;
static float min_vert_ang = -1.0f;
static float max_vert_ang =  1.0f;

static void G_HandleMouse(void) {
    int width, height;
    SDL_GetWindowSize(g_cVidstate.window, &width, &height);

    int x, y;
    SDL_GetMouseState(&x, &y);

    g_cPlayerstate.camera.angle += sens * (width / 2 - x);
    g_cPlayerstate.camera.vert_angle -= sens * (height / 2 - y);
    g_cPlayerstate.camera.vert_angle = clamp(g_cPlayerstate.camera.vert_angle, min_vert_ang, max_vert_ang);

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
            // g_cPlayerstate.sector = 1;
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

int CMD_SetMinVertAng(char* args);
int CMD_SetMaxVertAng(char* args);
int CMD_SetSens(char* args);

void G_InitPlayer(void) {
    CMD_AddCommand("+forward", (cmd_fn_t) &CMD_PlusForward);
    CMD_AddCommand("-forward", (cmd_fn_t) &CMD_MinusForward);
    CMD_AddCommand("+back",    (cmd_fn_t) &CMD_PlusBack);
    CMD_AddCommand("-back",    (cmd_fn_t) &CMD_MinusBack);
    CMD_AddCommand("+left",    (cmd_fn_t) &CMD_PlusLeft);
    CMD_AddCommand("-left",    (cmd_fn_t) &CMD_MinusLeft);
    CMD_AddCommand("+right",   (cmd_fn_t) &CMD_PlusRight);
    CMD_AddCommand("-right",   (cmd_fn_t) &CMD_MinusRight);
    CMD_AddCommand("+jump",    (cmd_fn_t) &CMD_PlusJump);
    CMD_AddCommand("-jump",    (cmd_fn_t) &CMD_MinusJump);

    CMD_AddCommand("cl_sens",         &CMD_SetSens);
    CMD_AddCommand("cl_min_vert_ang", &CMD_SetMinVertAng);
    CMD_AddCommand("cl_max_vert_ang", &CMD_SetMaxVertAng);

    g_cPlayerstate.sector = 1;
    g_cPlayerstate.phys_obj.pos.x = 3;
    g_cPlayerstate.phys_obj.pos.y = 3;
    g_cPlayerstate.phys_obj.pos.z = 10;
    g_cPlayerstate.phys_obj.mass = 1.0f;

    G_UpdatePlayerSector();
    G_UpdateEye();
}

void G_UpdatePlayer(void) {
    G_HandleMouse();
    G_PlayerHandleKeys();

    P_UpdateObject(&g_cPlayerstate.phys_obj);

    if (g_cPlayerstate.phys_obj.pos.z < g_cState.map.sectors.arr[g_cPlayerstate.sector].zfloor) {
        g_cPlayerstate.phys_obj.vel.z = 0;
        g_cPlayerstate.phys_obj.pos.z = g_cState.map.sectors.arr[g_cPlayerstate.sector].zfloor;
    }

    if (g_cPlayerstate.phys_obj.pos.z + PLAYER_EYE_Z > g_cState.map.sectors.arr[g_cPlayerstate.sector].zceil) {
        g_cPlayerstate.phys_obj.vel.z = 0;
        g_cPlayerstate.phys_obj.pos.z = g_cState.map.sectors.arr[g_cPlayerstate.sector].zceil - PLAYER_EYE_Z;
    }

    g_cPlayerstate.camera.pos.x = g_cPlayerstate.phys_obj.pos.x;
    g_cPlayerstate.camera.pos.y = g_cPlayerstate.phys_obj.pos.y;
    g_cPlayerstate.camera.eye_z = g_cPlayerstate.phys_obj.pos.z + PLAYER_EYE_Z;

    char buf[32];
    snprintf(
        buf, sizeof(buf), "UPS: %f, %f, %f",
        g_cPlayerstate.phys_obj.vel.x,
        g_cPlayerstate.phys_obj.vel.y,
        g_cPlayerstate.phys_obj.vel.z
    );

    CON_DrawString((v2i) { 1, 30 }, buf);

    G_UpdatePlayerSector();
}

void G_FreePlayer(void) {}

int CMD_SetSens(char* args) {
    READ_FLOAT_VAR(sens);
}

int CMD_SetMinVertAng(char* args) {
    READ_FLOAT_VAR(min_vert_ang);
}

int CMD_SetMaxVertAng(char* args) {
    READ_FLOAT_VAR(max_vert_ang);
}