#include "../cl_def.h"

extern state_t      client_state;
extern vidstate_t   video_state;
extern game_state_t game_state;

cmd_var_t min_player_speed    = { "sv_min_player_speed",    "", 0, MIN_PLAYER_SPEED };
cmd_var_t regular_impulse     = { "sv_regular_impulse",     "", 0, REGULAR_IMPULSE };
cmd_var_t floor_jump_box_size = { "sv_floor_jump_box_size", "", 0, FLOOR_JUMP_BOX_SIZE };
cmd_var_t jump_impulse        = { "sv_jump_impulse",        "", 0, JUMP_IMPULSE };
cmd_var_t player_eye          = { "sv_player_eye",          "", 0, PLAYER_EYE_Z };

static void G_MovePlayer(player_t* player) {
    const sector_t* player_sector = &game_state.map.sectors.arr[player->sector];

    v3* pos = &player->phys_obj.pos;
    v3* vel = &player->phys_obj.vel;

    phys_obj_t* phys_obj = &player->phys_obj;

    const bool floored = pos->z <= player_sector->zfloor + floor_jump_box_size.floating;
    phys_obj->floored = floored;

    P_UpdateObject(phys_obj);

    if (pos->z < player_sector->zfloor) {
        vel->z = 0;
        pos->z = player_sector->zfloor;
    }

    if (pos->z + player_eye.floating > player_sector->zceil) {
        vel->z = 0;
        pos->z = player_sector->zceil - player_eye.floating;
    }

    const float impulse = (
        fabsf(vel->x) < min_player_speed.floating &&
        fabsf(vel->y) < min_player_speed.floating &&
        fabsf(vel->z) < min_player_speed.floating
    ) ? min_player_speed.floating : regular_impulse.floating;

    const float anglecos = player->anglecos;
    const float anglesin = player->anglesin;

    if (player->forward) P_AddVel(
        phys_obj,
        (v3) {
            (impulse * anglecos),
            (impulse * anglesin),
            0.f
        }
    );

    if (player->back) P_AddVel(
        phys_obj,
        (v3) {
            -(impulse * anglecos),
            -(impulse * anglesin),
            0.f
        }
    );

    if (player->left) P_AddVel(
        phys_obj,
        (v3) {
            -(impulse * anglesin),
             (impulse * anglecos),
            0.f
        }
    );

    if (player->right) P_AddVel(
        phys_obj,
        (v3) {
             (impulse * anglesin),
            -(impulse * anglecos),
            0.f
        }
    );

    if (player->jump && floored) P_AddVel(
        phys_obj,
        (v3) {
            0.f, 0.f,
            jump_impulse.floating
        }
    );
}

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

static void G_UpdatePlayerSector(player_t* player) {
    int sectors_to_visit[SECTOR_MAX],
        n_sectors_to_visit = 0,
        i = 0,
        found = SECTOR_NONE;

    sectors_to_visit[0] = player->sector == SECTOR_NONE ? 1 : player->sector;
    n_sectors_to_visit++;

    while (i != n_sectors_to_visit) {
        const u32 sector_id = sectors_to_visit[i];
        const sector_t* sector = &game_state.map.sectors.arr[sector_id];

        if (MATH_PointInSector(sector, (v2) { player->phys_obj.pos.x, player->phys_obj.pos.y })) {
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
        player->sector = found;
}

static void G_FindPlayerSpawn(player_t* player) {
    player->phys_obj.pos.x = 3;
    player->phys_obj.pos.y = 3;
    player->phys_obj.pos.z = 10;
}

void G_InitPlayer(player_t* player) {
    player->sector = 1;
    player->phys_obj.mass = 1.0f;

    G_FindPlayerSpawn(player);
    G_UpdatePlayerSector(player);
}

void G_UpdatePlayer(player_t* player) {
    G_MovePlayer(player);
    G_UpdatePlayerSector(player);

    player->eye_z = player->phys_obj.pos.z + player_eye.floating;
}