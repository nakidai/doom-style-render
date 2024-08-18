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

extern state_t      client_state;
extern game_state_t game_state;

void P_UpdateObject(phys_obj_t* obj) {
    obj->vel.x /= game_state.slowdown;
    obj->vel.y /= game_state.slowdown;
    obj->vel.z /= game_state.slowdown;

    P_AddVel(obj, (v3) { 0, 0, game_state.gravity * obj->mass });

    obj->pos.x += obj->vel.x;
    obj->pos.y += obj->vel.y;
    obj->pos.z += obj->vel.z;
}

void P_Init(void) {
    
}