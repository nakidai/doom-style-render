#include "../cl_def.h"

extern state_t client_state;
extern vidstate_t video_state;
playerstate_t player_state;

static bool forward = false;
static bool back    = false;
static bool left    = false;
static bool right   = false;
static bool jump    = false;

int CMD_PlusForward(char* args __attribute__((unused))) {
    forward = true;
    return SUCCESS;
}

int CMD_MinusForward(char* args __attribute__((unused))) {
    forward = false;
    return SUCCESS;
}

int CMD_PlusBack(char* args __attribute__((unused))) {
    back = true;
    return SUCCESS;
}

int CMD_MinusBack(char* args __attribute__((unused))) {
    back = false;
    return SUCCESS;
}

int CMD_PlusLeft(char* args __attribute__((unused))) {
    left = true;
    return SUCCESS;
}

int CMD_MinusLeft(char* args __attribute__((unused))) {
    left = false;
    return SUCCESS;
}

int CMD_PlusRight(char* args __attribute__((unused))) {
    right = true;
    return SUCCESS;
}

int CMD_MinusRight(char* args __attribute__((unused))) {
    right = false;
    return SUCCESS;
}

int CMD_PlusJump(char* args __attribute__((unused))) {
    jump = true;
    return SUCCESS;
}

int CMD_MinusJump(char* args __attribute__((unused))) {
    jump = false;
    return SUCCESS;
}

static void G_PlayerHandleKeys(void) {
    const u8* keystate = SDL_GetKeyboardState(NULL);
    const f32 move_speed = 0.003f;

    player_state.camera.anglecos = cos(player_state.camera.angle);
    player_state.camera.anglesin = sin(player_state.camera.angle);

    if (forward) {
        P_SetVel(
            &player_state.phys_obj,
            (v3) {
                (move_speed * player_state.camera.anglecos),
                (move_speed * player_state.camera.anglesin),
                player_state.phys_obj.vel.z,
            }
        );
    }

    if (back) {
        P_SetVel(
            &player_state.phys_obj,
            (v3) {
                -(move_speed * player_state.camera.anglecos),
                -(move_speed * player_state.camera.anglesin),
                player_state.phys_obj.vel.z,
            }
        );
    }

    if (left) {
        P_AddVel(
            &player_state.phys_obj,
            (v3) {
                -(move_speed * player_state.camera.anglesin),
                (move_speed * player_state.camera.anglecos),
                0.f
            }
        );
    }

    if (right) {
        P_AddVel(
            &player_state.phys_obj,
            (v3) {
                (move_speed * player_state.camera.anglesin),
                -(move_speed * player_state.camera.anglecos),
                0.f
            }
        );
    }

    player_state.phys_obj.vel.x = clamp(
        player_state.phys_obj.vel.x,
        -move_speed, move_speed
    );

    player_state.phys_obj.vel.y = clamp(
        player_state.phys_obj.vel.y,
        -move_speed, move_speed
    );

    if (jump && player_state.phys_obj.pos.z == client_state.map.sectors.arr[player_state.sector].zfloor) {
        P_AddVel(
            &player_state.phys_obj,
            (v3) {
                player_state.phys_obj.vel.x * 0.1f,
                player_state.phys_obj.vel.y * 0.1f,
                -GRAVITY + 0.03f
            }
        );
    }

    if (keystate[SDLK_F1 & 0xFFFF]) {
        client_state.sleepy = true;
    }
}

static float sens         = 0.005f;
static float min_vert_ang = -1.0f;
static float max_vert_ang =  1.0f;

static void G_HandleMouse(void) {
    int width, height;
    SDL_GetWindowSize(video_state.window, &width, &height);

    int x, y;
    SDL_GetMouseState(&x, &y);

    player_state.camera.angle += sens * (width / 2 - x);
    player_state.camera.vert_angle -= sens * (height / 2 - y);
    player_state.camera.vert_angle = clamp(player_state.camera.vert_angle, min_vert_ang, max_vert_ang);

    SDL_WarpMouseInWindow(video_state.window, width / 2, height / 2);
}

static inline float MATH_PointSide(v2 p, v2i a, v2i b) {
    return -(((p.x - a.x) * (b.y - a.y))
           - ((p.y - a.y) * (b.x - a.x))); 
}

static bool MATH_PointInSector(const sector_t* sector, v2 p) {
    for (usize i = 0; i < sector->nwalls; i++) {
        const wall_t* _wall = &client_state.map.walls.arr[sector->firstwall + i];

        if (MATH_PointSide(p, _wall->a, _wall->b) > 0) {
            return false;
        }
    }

    return true;
}

static void G_UpdatePlayerSector(void) {
    int sectors_to_visit[SECTOR_MAX],
        n_sectors_to_visit = 0,
        i = 0,
        found = SECTOR_NONE;

    sectors_to_visit[0] = player_state.sector == SECTOR_NONE ? 1 : player_state.sector;
    n_sectors_to_visit++;

    while (i != n_sectors_to_visit) {
        const u32 sector_id = sectors_to_visit[i];
        const sector_t* sector = &client_state.map.sectors.arr[sector_id];

        if (MATH_PointInSector(sector, player_state.camera.pos)) {
            found = sector_id;
            goto done;
        }

        for (usize j = sector->firstwall; j < sector->nwalls + sector->firstwall; j++) {
            const wall_t* wall = &client_state.map.walls.arr[j];

            if (n_sectors_to_visit >= SECTOR_MAX) goto done;

            if (wall->portal != 0) {
                sectors_to_visit[n_sectors_to_visit++] = wall->portal;
            }
        }

        i++;
    }

    done:
        if (found == SECTOR_NONE) return;
        player_state.sector = found;
}

static void G_UpdateEye(void) {
    const int sector_id = player_state.sector;
    const sector_t* player_sector = &client_state.map.sectors.arr[sector_id];

    player_state.camera.eye_z = player_sector->zfloor + PLAYER_EYE_Z;
}

int CMD_SetMinVertAng(char* args);
int CMD_SetMaxVertAng(char* args);
int CMD_SetSens(char* args);
int CMD_TeleportPlayer(char* args);

void G_InitPlayer(void) {
    CMD_AddCommand("+forward", &CMD_PlusForward);
    CMD_AddCommand("-forward", &CMD_MinusForward);
    CMD_AddCommand("+back",    &CMD_PlusBack);
    CMD_AddCommand("-back",    &CMD_MinusBack);
    CMD_AddCommand("+left",    &CMD_PlusLeft);
    CMD_AddCommand("-left",    &CMD_MinusLeft);
    CMD_AddCommand("+right",   &CMD_PlusRight);
    CMD_AddCommand("-right",   &CMD_MinusRight);
    CMD_AddCommand("+jump",    &CMD_PlusJump);
    CMD_AddCommand("-jump",    &CMD_MinusJump);

    CMD_AddCommand("cl_sens",         &CMD_SetSens);
    CMD_AddCommand("cl_min_vert_ang", &CMD_SetMinVertAng);
    CMD_AddCommand("cl_max_vert_ang", &CMD_SetMaxVertAng);

    CMD_AddCommand("pl_tp", &CMD_TeleportPlayer);

    player_state.sector = 1;
    player_state.phys_obj.pos.x = 3;
    player_state.phys_obj.pos.y = 3;
    player_state.phys_obj.pos.z = 10;
    player_state.phys_obj.mass = 1.0f;

    G_UpdatePlayerSector();
    G_UpdateEye();
}

void G_UpdatePlayer(void) {
    G_HandleMouse();
    G_PlayerHandleKeys();

    P_UpdateObject(&player_state.phys_obj);

    if (player_state.phys_obj.pos.z < client_state.map.sectors.arr[player_state.sector].zfloor) {
        player_state.phys_obj.vel.z = 0;
        player_state.phys_obj.pos.z = client_state.map.sectors.arr[player_state.sector].zfloor;
    }

    if (player_state.phys_obj.pos.z + PLAYER_EYE_Z > client_state.map.sectors.arr[player_state.sector].zceil) {
        player_state.phys_obj.vel.z = 0;
        player_state.phys_obj.pos.z = client_state.map.sectors.arr[player_state.sector].zceil - PLAYER_EYE_Z;
    }

    player_state.camera.pos.x = player_state.phys_obj.pos.x;
    player_state.camera.pos.y = player_state.phys_obj.pos.y;
    player_state.camera.eye_z = player_state.phys_obj.pos.z + PLAYER_EYE_Z;

    char buf[32];
    snprintf(
        buf, sizeof(buf), "UPS: %f, %f, %f",
        player_state.phys_obj.vel.x,
        player_state.phys_obj.vel.y,
        player_state.phys_obj.vel.z
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

int CMD_TeleportPlayer(char* args) {
    if (sscanf(
        args,
        "%f %f %f",
        &player_state.phys_obj.pos.x,
        &player_state.phys_obj.pos.y,
        &player_state.phys_obj.pos.z
    ) != 3) return 2;

    return SUCCESS;
}