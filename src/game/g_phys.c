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
    G_UpdateObjectSector(&obj->obj);

    P_AddVel(obj, (v3) { 0, 0, gravity.floating * obj->mass });

    v3* pos = &obj->obj.pos;
    v3* vel = &obj->vel;

    const sector_t* obj_sector = &game_state.map.sectors.arr[obj->obj.sector];
    const bool floored = pos->z <= obj_sector->zfloor;
    obj->floored = floored;

    if (pos->z < obj_sector->zfloor) {
        vel->z = 0;
        pos->z = obj_sector->zfloor;
    }

    if (pos->z + obj->height > obj_sector->zceil) {
        vel->z = 0;
        pos->z = obj_sector->zceil - obj->height;
    }

    if (obj->floored) P_AccelerateObject(obj, axel.floating);
    else              P_AccelerateObject(obj, air_axel.floating);

    obj->obj.pos.x += obj->vel.x;
    obj->obj.pos.y += obj->vel.y;
    obj->obj.pos.z += obj->vel.z;
}