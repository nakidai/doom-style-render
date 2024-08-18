#ifndef _g_player_h
#define _g_player_h

#include "../utils/u_def.h"
#include "../utils/math/u_math.h"
#include "g_phys.h"

#define PLAYER_EYE_Z 1.1f

typedef struct {
    int sector;
    f32 angle, vert_angle, anglecos, anglesin, eye_z;
    phys_obj_t phys_obj;

    bool forward;
    bool back;
    bool left;
    bool right;
    bool jump;
} player_t;

void G_InitPlayer(player_t* player);
void G_UpdatePlayer(player_t* player);

#endif