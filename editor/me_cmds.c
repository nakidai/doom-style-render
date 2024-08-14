#include "me_cmds.h"

extern float   scale;
extern v2i     pos;
extern bool    draw_wall_num;
extern bool    draw_sect_num;
extern int     selected_wall;
extern int     selected_point;
extern state_t g_cState;
extern v2i     points[64];
extern usize   point_count;

int ME_AddPoint(char* args);
int ME_Scale(char* args);
int ME_Teleport(char* args);
int ME_DrawWallNum(char* args);
int ME_DrawSectNum(char* args);
int ME_SelectWall(char* args);
int ME_SelectPoint(char* args);
int ME_ClearPoints(char* args);
int ME_LoadMapFromWAD(char* args);
int ME_AddWall(char* args);
int ME_AddWallPortal(char* args);
int ME_AddSector(char* args);

void ME_SetupCommands(void) {
    CMD_AddCommand("ap",  &ME_AddPoint);
    CMD_AddCommand("s",   &ME_Scale);
    CMD_AddCommand("tp",  &ME_Teleport);
    CMD_AddCommand("dsn", &ME_DrawSectNum);
    CMD_AddCommand("dwn", &ME_DrawWallNum);
    CMD_AddCommand("sw",  &ME_SelectWall);
    CMD_AddCommand("sp",  &ME_SelectPoint);
    CMD_AddCommand("lmw", &ME_LoadMapFromWAD);
    CMD_AddCommand("cp",  &ME_ClearPoints);
    CMD_AddCommand("aw",  &ME_AddWall);
    CMD_AddCommand("awp", &ME_AddWallPortal);
    CMD_AddCommand("as",  &ME_AddSector);
}

int ME_AddPoint(char* args) {
    v2i pos_in = { NAN, NAN };

    if (*args == 'c') {
        int x, y;
        SDL_GetMouseState(&x, &y);

        pos_in.x = (x / scale) + pos.x;
        pos_in.y = (y / scale) + pos.y;

        goto done;
    } else {
        if (sscanf(
            args,
            "%i %i",
            &pos_in.x,
            &pos_in.y
        ) != 2) return 2;

        goto done;
    }

    done:
        if (pos.x == NAN && pos.y == NAN) return 2;

        points[point_count++] = pos_in;

        return SUCCESS;
}

int ME_Scale(char* args) {
    if (sscanf(
        args,
        "%f",
        &scale
    ) != 1) return 2;
    return SUCCESS;
}

int ME_Teleport(char* args) {
    if (sscanf(
        args,
        "%i %i",
        &pos.x,
        &pos.y
    ) != 2) return 2;
    return SUCCESS;
}

int ME_DrawWallNum(char* args) {
    draw_wall_num = !draw_wall_num;
    return SUCCESS;
}

int ME_DrawSectNum(char* args) {
    draw_sect_num = !draw_sect_num;
    return SUCCESS;
}

int ME_SelectWall(char* args) {
    if (sscanf(
        args,
        "%i",
        &selected_wall
    ) != 1) return 2;
    return SUCCESS;
}

int ME_LoadMapFromWAD(char* args) {
    G_LoadMap(&g_cState.map, args);
    return SUCCESS;
}

int ME_SelectPoint(char* args) {
    if (sscanf(
        args,
        "%i",
        &selected_point
    ) != 1) return 2;
    return SUCCESS;
}

int ME_ClearPoints(char* args) {
    point_count = 0;
    return SUCCESS;
}

int ME_AddWall(char* args) {
    int p0, p1;
    if (sscanf(
        args,
        "%i %i",
        &p0, &p1
    ) != 2) return 2;

    wall_t* wall = &g_cState.map.walls.arr[g_cState.map.walls.n++];

    wall->a = points[p0];
    wall->b = points[p1];
    wall->portal = 0;

    return SUCCESS;
}

int ME_AddWallPortal(char* args) {
    int p0, p1, portal;
    if (sscanf(
        args,
        "%i %i %i",
        &p0, &p1, &portal
    ) != 2) return 2;

    wall_t* wall = &g_cState.map.walls.arr[g_cState.map.walls.n++];

    wall->a = points[p0];
    wall->b = points[p1];
    wall->portal = portal;

    return SUCCESS;
}

int ME_AddSector(char* args) {
    int id, fw, nw, light;
    f32 cell, floor;
    if (sscanf(
        args,
        "%i %i %i %i %f %f",
        &id, &fw, &nw,
        &light,
        &cell,
        &floor
    ) != 6) return 2;

    sector_t* sect = &g_cState.map.sectors.arr[g_cState.map.sectors.n++];
    sect->id = id;
    sect->firstwall = fw;
    sect->nwalls = nw;
    sect->light = light;
    sect->zceil = cell;
    sect->zfloor = floor;

    return SUCCESS;
}