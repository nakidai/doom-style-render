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
#include "render/utils/r_texture.h"

// state type
typedef enum {
    GS_LEVEL,   // level state
} state_type_t;

// state structure
typedef struct {
    state_type_t state; // state type

    bool quit;    // is game quit?
    bool console; // is console opened?
    bool sleepy;  // TODO: remove this and create console command

    f64 delta_time; // delta time
} state_t;

#endif