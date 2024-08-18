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

extern state_t client_state;

static float gravity  = GRAVITY;
static float slowdown = SLOWDOWN;

void P_UpdateObject(phys_obj_t* obj) {
    obj->vel.x /= slowdown;
    obj->vel.y /= slowdown;
    obj->vel.z /= slowdown;

    P_AddVel(obj, (v3) { 0, 0, gravity * obj->mass });

    obj->pos.x += obj->vel.x * client_state.delta_time;
    obj->pos.y += obj->vel.y * client_state.delta_time;
    obj->pos.z += obj->vel.z * client_state.delta_time;
}

int CMD_SetSlowdown(char* args);
int CMD_SetGravity(char* args);

void P_Init(void) {
    CMD_AddCommand("ps_gravity",  &CMD_SetGravity);
    CMD_AddCommand("ps_slowdown", &CMD_SetSlowdown);
}

int CMD_SetSlowdown(char* args) {
    READ_FLOAT_VAR(slowdown);
}

int CMD_SetGravity(char* args) {
    READ_FLOAT_VAR(gravity);
}