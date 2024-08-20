#include "cl_def.h"

cmd_var_t gravity  = { "sv_gravity",  "", 0, GRAVITY };
cmd_var_t axel     = { "sv_axel",     "", 0, AXEL };
cmd_var_t air_axel = { "sv_air_axel", "", 0, AIR_AXEL }; 

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
    P_AddVel(obj, (v3) { 0, 0, gravity.floating * obj->mass });

    if (obj->floored) P_AccelerateObject(obj, axel.floating);
    else              P_AccelerateObject(obj, air_axel.floating);

    obj->pos.x += obj->vel.x;
    obj->pos.y += obj->vel.y;
    obj->pos.z += obj->vel.z;
}