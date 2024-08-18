#ifndef _g_phys_h
#define _g_phys_h

#include "utils/u_def.h"
#include "utils/math/u_math.h"

#define SLOWDOWN 1.1f
#define GRAVITY  -0.03f

typedef struct {
    v3 vel;
    v3 pos;
    float mass;
} phys_obj_t;

void P_AddVel(phys_obj_t* obj, v3 vel);
void P_SetVel(phys_obj_t* obj, v3 vel);

void P_UpdateObject(phys_obj_t* obj);

#endif