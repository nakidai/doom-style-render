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

#define AXEL     0.1f
#define AIR_AXEL 0.01f

static void P_AccelerateObject(phys_obj_t* obj, float axel) {
    P_AddVel(
        obj,
        (v3) {
            -(axel * obj->vel.x),
            -(axel * obj->vel.y),
            0.f
        }
    );
}

extern state_t      client_state;
extern game_state_t game_state;

void P_UpdateObject(phys_obj_t* obj) {
    P_AddVel(obj, (v3) { 0, 0, game_state.gravity * obj->mass });

    if (obj->floored) P_AccelerateObject(obj, AXEL);
    else              P_AccelerateObject(obj, AIR_AXEL);

    obj->pos.x += obj->vel.x;
    obj->pos.y += obj->vel.y;
    obj->pos.z += obj->vel.z;
}