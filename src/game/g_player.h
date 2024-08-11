#ifndef _g_player_h
#define _g_player_h

#include "../utils/u_def.h"

#define PLAYER_EYE_Z 1.1f

typedef struct {
    struct {
        v2 pos;
        f32 angle, vert_angle, anglecos, anglesin, eye_z;
    } camera;
    int sector;
} playerstate_t;

void G_InitPlayer(void);
void G_UpdatePlayer(void);
void G_FreePlayer(void);

#endif