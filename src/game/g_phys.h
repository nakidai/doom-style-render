#ifndef _g_phys_h
#define _g_phys_h

#include "utils/u_def.h"
#include "utils/math/u_math.h"
#include "g_world_object.h"

#define GRAVITY  -0.02f
#define AXEL     0.1f
#define AIR_AXEL 0.01f

typedef struct {
    world_obj_t obj;
    v3 vel;

    bool floored;

    float mass;
    float height;
} phys_obj_t;

void P_AddVel(phys_obj_t* obj, v3 vel);
void P_SetVel(phys_obj_t* obj, v3 vel);

void P_UpdateObject(phys_obj_t* obj);

#endif