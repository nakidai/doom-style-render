#ifndef _g_map_h
#define _g_map_h

#include "../utils/u_def.h"
#include "../utils/math/u_math.h"

typedef struct {
    v2i a, b;
    int portal;
} wall_t;

// sector id for "no sector"
#define SECTOR_NONE 0
#define SECTOR_MAX 128

typedef struct {
    int id;
    usize firstwall, nwalls;
    f32 zfloor, zceil;
    u8 light;
} sector_t;

typedef struct {
    struct { sector_t arr[32]; usize n; } sectors;
    struct { wall_t arr[128]; usize n; } walls;
} map_t;

void G_LoadMap(map_t* map, const char* name);

#endif