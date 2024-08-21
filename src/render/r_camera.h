#ifndef _r_camera_h
#define _r_camera_h

#include "utils/u_def.h"
#include "game/g_world_object.h"

typedef struct {
    world_obj_t obj;
    v2 angle;
} camera_t;

#endif