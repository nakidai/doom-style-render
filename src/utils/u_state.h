#ifndef _u_state_h
#define _u_state_h

#include "u_def.h"
#include "../game/g_map.h"
#include "../core/file/f_wad.h"
#include "../render/utils/r_texture.h"

typedef struct {
    bool quit;
    bool sleepy;

    f64 delta_time;

    wad_t wad;
    map_t map;

    u16 y_lo[SCREEN_WIDTH], y_hi[SCREEN_WIDTH];
} state_t;

#endif