#include "cl_def.h"

void P_AddVel(phys_obj_t* obj, v3 vel) {
    obj->vel.x += vel.x;
    obj->vel.y += vel.y;
    obj->vel.z += vel.z;
}

void P_SetVel(phys_obj_t* obj, v3 vel) {
    obj->vel.x = vel.x;
    obj->vel.y = vel.y;
    obj->vel.z = vel.z;
}

extern state_t g_cState;

void P_UpdateObject(phys_obj_t* obj) {
    obj->vel.x /= SLOWDOWN;
    obj->vel.y /= SLOWDOWN;
    obj->vel.z /= SLOWDOWN;

    obj->pos.x += obj->vel.x * g_cState.delta_time;
    obj->pos.y += obj->vel.y * g_cState.delta_time;
    obj->pos.z += obj->vel.z * g_cState.delta_time;
}