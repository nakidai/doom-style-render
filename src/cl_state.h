// Copyright (C) by gimura 2024
// --- DESCRIPTION ---
//
// client state structures and enums
//
// -------------------

#ifndef _u_state_h
#define _u_state_h

#include "utils/u_def.h"
#include "game/g_map.h"
#include "core/file/f_wad.h"
#include "render/utils/r_texture.h"

// state type
typedef enum {
    LEVEL_STATE,   // level state
    CONSOLE_STATE, // console opened
} state_type_t;

// state structure
typedef struct {
    state_type_t state; // state type

    SDL_Event events[128]; // events
    usize     event_count; // event count

    bool quit;   // is game quit?
    bool sleepy; // TODO: remove this and create console command

    f64 delta_time; // delta time

    wad_t wad; // current wad file
    map_t map; // current map

    u16 y_lo[SCREEN_WIDTH], y_hi[SCREEN_WIDTH]; // TODO: move it to render state
} state_t;

#endif