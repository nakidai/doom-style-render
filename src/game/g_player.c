#include "../cl_def.h"

extern state_t      client_state;
extern vidstate_t   video_state;
extern game_state_t game_state;

#define PLAYER_SPEED 0.1f
#define JUMP_IMPULSE 0.6f

static void G_MovePlayer(player_t* player) {
    P_UpdateObject(&player->phys_obj);

    const sector_t* player_sector = &game_state.map.sectors.arr[player->sector];
    v3* pos = &player->phys_obj.pos;
    v3* vel = &player->phys_obj.vel;

    if (pos->z < player_sector->zfloor) {
        vel->z = 0;
        pos->z = player_sector->zfloor;
    }

    if (pos->z + PLAYER_EYE_Z > player_sector->zceil) {
        vel->z = 0;
        pos->z = player_sector->zceil - PLAYER_EYE_Z;
    }

    if (player->forward) {
        P_SetVel(
            &player->phys_obj,
            (v3) {
                (PLAYER_SPEED * player->anglecos),
                (PLAYER_SPEED * player->anglesin),
                player->phys_obj.vel.z,
            }
        );
    }

    if (player->back) {
        P_SetVel(
            &player->phys_obj,
            (v3) {
                -(PLAYER_SPEED * player->anglecos),
                -(PLAYER_SPEED * player->anglesin),
                player->phys_obj.vel.z,
            }
        );
    }

    if (player->left) {
        P_AddVel(
            &player->phys_obj,
            (v3) {
                -(PLAYER_SPEED * player->anglesin),
                (PLAYER_SPEED * player->anglecos),
                0.f
            }
        );
    }

    if (player->right) {
        P_AddVel(
            &player->phys_obj,
            (v3) {
                (PLAYER_SPEED * player->anglesin),
                -(PLAYER_SPEED * player->anglecos),
                0.f
            }
        );
    }

    player->phys_obj.vel.x = clamp(
        player->phys_obj.vel.x,
        -PLAYER_SPEED, PLAYER_SPEED
    );

    player->phys_obj.vel.y = clamp(
        player->phys_obj.vel.y,
        -PLAYER_SPEED, PLAYER_SPEED
    );

    if (player->jump && player->phys_obj.pos.z == game_state.map.sectors.arr[player->sector].zfloor) {
        P_AddVel(
            &player->phys_obj,
            (v3) {
                player->phys_obj.vel.x * 0.1f,
                player->phys_obj.vel.y * 0.1f,
                JUMP_IMPULSE
            }
        );
    }
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

    player->eye_z = player->phys_obj.pos.z + PLAYER_EYE_Z;
}