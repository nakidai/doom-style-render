#ifndef _g_player_h
#define _g_player_h

#include "../utils/u_def.h"
#include "../utils/math/u_math.h"
#include "g_phys.h"

#define PLAYER_EYE_Z 1.1f

typedef struct {
    struct {
        v2 pos;
        f32 angle, vert_angle, anglecos, anglesin, eye_z;
    } camera;

    int sector;

    phys_obj_t phys_obj;
} playerstate_t;

void G_InitPlayer(void);
void G_UpdatePlayer(void);
void G_FreePlayer(void);

#endif