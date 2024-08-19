#include "cl_def.h"

game_state_t      game_state;
extern state_t    client_state;
extern vidstate_t video_state;

#define MOVE_CMD(x, y) ({ x = y; return SUCCESS; })
#define P_MOV(x) MOVE_CMD(x, true)
#define M_MOV(x) MOVE_CMD(x, false)

static int CMD_PlusForward(char* args __attribute__((unused)))  { P_MOV(game_state.player.forward); }
static int CMD_MinusForward(char* args __attribute__((unused))) { M_MOV(game_state.player.forward); }

static int CMD_PlusBack(char* args __attribute__((unused)))  { P_MOV(game_state.player.back); }
static int CMD_MinusBack(char* args __attribute__((unused))) { M_MOV(game_state.player.back); }

static int CMD_PlusLeft(char* args __attribute__((unused)))  { P_MOV(game_state.player.left); }
static int CMD_MinusLeft(char* args __attribute__((unused))) { M_MOV(game_state.player.left); }

static int CMD_PlusRight(char* args __attribute__((unused)))  { P_MOV(game_state.player.right); }
static int CMD_MinusRight(char* args __attribute__((unused))) { M_MOV(game_state.player.right); }

static int CMD_PlusJump(char* args __attribute__((unused)))  { P_MOV(game_state.player.jump); }
static int CMD_MinusJump(char* args __attribute__((unused))) { M_MOV(game_state.player.jump); }

static float sens         = 0.005f;
static float min_vert_ang = -1.0f;
static float max_vert_ang =  1.0f;

static int CMD_SetSens(char* args)       { READ_FLOAT_VAR(sens); }
static int CMD_SetMinVertAng(char* args) { READ_FLOAT_VAR(min_vert_ang); }
static int CMD_SetMaxVertAng(char* args) { READ_FLOAT_VAR(max_vert_ang); }

static int CMD_SetSlowdown(char* args) { READ_FLOAT_VAR(game_state.slowdown); }
static int CMD_SetGravity(char* args)  { READ_FLOAT_VAR(game_state.gravity); }

static int CMD_TeleportPlayer(char* args) {
    v3* pos = &game_state.player.phys_obj.pos;

    if (sscanf(
        args,
        "%f %f %f",
        &pos->x,
        &pos->y,
        &pos->z
    ) != 3) return 2;

    return SUCCESS;
}

static int CMD_LoadMap(char* args) {
    G_LoadMap(&game_state.map, args);
    return SUCCESS;
}

void G_Init(void) {
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

    CMD_AddCommand("ps_gravity",  &CMD_SetGravity);
    CMD_AddCommand("ps_slowdown", &CMD_SetSlowdown);

    CMD_AddCommand("map", &CMD_LoadMap);

    game_state.slowdown = SLOWDOWN;
    game_state.gravity  = GRAVITY;

    G_InitPlayer(&game_state.player);
}

static void G_HandleMouse(void) {
    int width, height;
    SDL_GetWindowSize(video_state.window, &width, &height);

    int x, y;
    SDL_GetMouseState(&x, &y);

    player_t* player = &game_state.player;

    player->angle      += sens * (width / 2 - x);
    player->vert_angle -= sens * (height / 2 - y);
    player->vert_angle = clamp(player->vert_angle, min_vert_ang, max_vert_ang);

    SDL_WarpMouseInWindow(video_state.window, width / 2, height / 2);
}

static void G_Tick(void) {
    G_UpdatePlayer(&game_state.player);
}

void G_Update(void) {
    // TODO: move player angle calculation to player class
    player_t* player = &game_state.player;

    player->anglecos = cos(player->angle);
    player->anglesin = sin(player->angle);

    G_HandleMouse();
    if ((SDL_GetTicks() % (100 / TICK_RATE)) == 0) G_Tick();
}

void G_Render(void) {
    R_RenderPlayerView(&game_state.player); // render player view
}

void G_Free(void) {

}