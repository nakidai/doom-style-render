#ifndef _g_game_h
#define _g_game_h

#include "utils/u_def.h"
#include "g_player.h"
#include "g_map.h"

#define TICK_RATE 60

typedef struct {
    player_t player;
    map_t    map;
} game_state_t;

void G_Init(void);

void G_Update(void);
void G_Render(void);

void G_Free(void);

#endif