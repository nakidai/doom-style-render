#ifndef _g_world_object_h
#define _g_world_object_h

#include "utils/u_def.h"

typedef struct {
    v3 pos;
    u16 sector;
} world_obj_t;

void G_UpdateObjectSector(world_obj_t* obj);

#endif