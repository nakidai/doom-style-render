#include "cl_def.h"

extern game_state_t game_state;

static inline float MATH_PointSide(v2 p, v2i a, v2i b) {
    return -(((p.x - a.x) * (b.y - a.y))
           - ((p.y - a.y) * (b.x - a.x))); 
}

static bool MATH_PointInSector(const sector_t* sector, v2 p) {
    for (usize i = 0; i < sector->nwalls; i++) {
        const wall_t* _wall = &game_state.map.walls.arr[sector->firstwall + i];

        if (MATH_PointSide(p, _wall->a, _wall->b) > 0) {
            return false;
        }
    }

    return true;
}

void G_UpdateObjectSector(world_obj_t* obj) {
    int sectors_to_visit[SECTOR_MAX],
        n_sectors_to_visit = 0,
        i = 0,
        found = SECTOR_NONE;

    sectors_to_visit[0] = obj->sector == SECTOR_NONE ? 1 : obj->sector;
    n_sectors_to_visit++;

    while (i != n_sectors_to_visit) {
        const u32 sector_id = sectors_to_visit[i];
        const sector_t* sector = &game_state.map.sectors.arr[sector_id];

        if (MATH_PointInSector(sector, (v2) { obj->pos.x, obj->pos.y })) {
            found = sector_id;
            goto done;
        }

        for (usize j = sector->firstwall; j < sector->nwalls + sector->firstwall; j++) {
            const wall_t* wall = &game_state.map.walls.arr[j];

            if (n_sectors_to_visit >= SECTOR_MAX) goto done;

            if (wall->portal != 0) {
                sectors_to_visit[n_sectors_to_visit++] = wall->portal;
            }
        }

        i++;
    }

    done:
        if (found == SECTOR_NONE) return;
        obj->sector = found;
}