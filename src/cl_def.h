// Copyright (C) by gimura 2024
// --- DESCRIPTION ---
//
// client headers included here
//
// -------------------

#ifndef _cl_def_h
#define _cl_def_h

// spec defines
#define CLIENT    // client define
#define DEV_BUILD // development build
#define PARANOID  // more checks

// library for reading .bmp files
// TODO: write own library for reading .bmp
#include "lib/cbmp/cbmp.h"

// game headers
#include "game/g_game.h"               // main game class
#include "game/g_player.h"             // player class
#include "game/g_map.h"                // map class
#include "game/g_phys.h"               // physics object class
#include "game/gfx/gfx_post_process.h" // post processing metods

// render headers
#include "render/r_render.h"        // main render header
#include "render/utils/r_texture.h" // texture class
#include "render/draw/r_draw.h"     // draw funtions

// utils header
#include "utils/u_def.h"       // macro, types constat and C headers
#include "utils/math/u_math.h" // math functions

// core files
#include "core/v_vid.h"          // video system
#include "core/file/f_conf.h"    // config files
#include "core/mem/m_alloc.h"    // memory manager
#include "core/mem/m_utils.h"    // memory utils and metrics
#include "core/sys.h"            // system utils
#include "core/keys/k_manager.h" // key manager

// console and commands
#include "cmd/cmd_exec.h"   // command executor
#include "console/c_con.h"  // console class
#include "console/c_draw.h" // char drawing

// client specific headers
#include "cl_state.h" // client state

#endif