#include "../cl_def.h"

typedef struct {
    char geo_txt_dir[8];
    char geo_bin_dir[8];
} map_header_t;

extern state_t g_cState;

static void G_LoadMapHeader(map_header_t* header, const char* name) {
    char map_name[8];
    snprintf(map_name, sizeof(map_name), "MD_%s", name);

    const wad_directory_t dir = W_FindDirectory(&g_cState.wad, map_name);
    u8* buf = M_TempAlloc(dir.lump_size);
    memset(buf, '\0', dir.lump_size);
    W_ReadWADDirectory(&g_cState.wad, dir, buf);
    buf[dir.lump_size] = '\0';

    const char* geo_txt_dir = C_GetVariable((char*) buf, "geo_txt");
    const char* geo_bin_dir = C_GetVariable((char*) buf, "geo_bin");

    M_TempFree(buf);

    if (geo_bin_dir != NULL) goto bin;
    if (geo_txt_dir != NULL) goto text;
    goto fail;

    bin:
        memcpy(header->geo_bin_dir, geo_bin_dir, strlen(geo_bin_dir));
        goto done;

    text:
        memcpy(header->geo_txt_dir, geo_txt_dir, strlen(geo_txt_dir));
        goto done;

    fail:
        if (geo_bin_dir != NULL) M_TempFree((void*) geo_bin_dir);
        if (geo_txt_dir != NULL) M_TempFree((void*) geo_txt_dir);

        ERROR("G_LoadHeader: in %s map header not found geometry variable (geo_bin or geo_txt)", name);

    done:
        if (geo_bin_dir != NULL) M_TempFree((void*) geo_bin_dir);
        if (geo_txt_dir != NULL) M_TempFree((void*) geo_txt_dir);
}

static void G_LoadTextWalls(map_t* map, const char* source);
static void G_LoadTextSectors(map_t* map, const char* source);

static void G_LoadTextGeometry(map_t* map, const map_header_t header) {
    const wad_directory_t dir = W_FindDirectory(&g_cState.wad, header.geo_txt_dir);
    u8* buf = M_TempAlloc(dir.lump_size);
    W_ReadWADDirectory(&g_cState.wad, dir, buf);

    u8* s_buf = M_TempAlloc(dir.lump_size);
    memcpy(s_buf, buf, dir.lump_size);

    G_LoadTextWalls(map, (char*) buf);
    G_LoadTextSectors(map, (char*) s_buf);

    M_TempFree(buf);
    M_TempFree(s_buf);
}

static void G_LoadTextWalls(map_t* map, const char* source) {
    bool fail = false;
    conf_array_t array = C_GetArray((char*) source, "line");
    map->walls.n = array.s;

    for (usize i = 0; i < array.s; i++) {
        wall_t* wall = &map->walls.arr[i];
        if (sscanf(
            array.d[i], "%d %d %d %d %d",
            &wall->a.x,
            &wall->a.y,
            &wall->b.x,
            &wall->b.y,
            &wall->portal
        ) != 5) fail = true;
    }

    for (usize i = 0; i < array.s; i++) M_TempFree((void*) array.d[i]);
    ASSERT(!fail, "G_LoadTextWalls: invalid wall format");
}

static void G_LoadTextSectors(map_t* map, const char* source) {
    bool fail = false;
    conf_array_t array = C_GetArray((char*) source, "sect");
    map->sectors.n = array.s + 1;

    for (usize i = 0; i < array.s; i++) {
        sector_t* sector = &map->sectors.arr[i + 1];
        if (sscanf(
            array.d[i], "%d %zu %zu %f %f",
            &sector->id,
            &sector->firstwall,
            &sector->nwalls,
            &sector->zfloor,
            &sector->zceil
        ) != 5) fail = true;
    }

    for (usize i = 0; i < array.s; i++) M_TempFree((void*) array.d[i]);
    ASSERT(!fail, "G_LoadTextSectors: invalid sector format");
}

// static void G_LoadBinGeometry(map_t* map, const map_header_t header) {
//     ERROR("G_LoadBinGeometry: not implemented");
// }

void G_LoadMap(map_t* map, const char* name) {
    map_header_t header = {};

    G_LoadMapHeader(&header, name);

    // if (*header.geo_bin_dir != '\0') G_LoadBinGeometry(map, header);
    if (*header.geo_bin_dir != '\0') ERROR("G_LoadBinGeometry: not implemented");
    if (*header.geo_txt_dir != '\0') G_LoadTextGeometry(map, header);
}