#include "../cl_def.h"

render_state_t render_state;
extern state_t client_state;
extern playerstate_t player_state;

void R_Init(void) {
    texture_t* tex = &render_state.textures[0];
    tex->size = (v2i) { 32, 32 };
    T_AllocTexture(tex);
    T_GenDebugTexture(tex);
}

void R_Free(void) {
    T_FreeTexture(&render_state.textures[0]);
}

void R_Render() {
    for (int i = 0; i < SCREEN_WIDTH; i++) {
        client_state.y_hi[i] = SCREEN_HEIGHT - 1;
        client_state.y_lo[i] = 0;
    }

    // track if sector has already been drawn
    u8 sectdraw[SECTOR_MAX];
    memset(sectdraw, 0, sizeof(sectdraw));

    // calculate edges of near/far planes (looking down +Y axis)
    const v2
        zdl = MATH_Rotate(((v2) { 0.0f, 1.0f }), +(HFOV / 2.0f)),
        zdr = MATH_Rotate(((v2) { 0.0f, 1.0f }), -(HFOV / 2.0f)),
        znl = (v2){ zdl.x * ZNEAR, zdl.y * ZNEAR },
        znr = (v2){ zdr.x * ZNEAR, zdr.y * ZNEAR },
        zfl = (v2){ zdl.x * ZFAR, zdl.y * ZFAR },
        zfr = (v2){ zdr.x * ZFAR, zdr.y * ZFAR };

    enum { QUEUE_MAX = 64 };
    struct queue_entry { int id, x0, x1; };

    struct { struct queue_entry arr[QUEUE_MAX]; usize n; } queue = {
        {{ player_state.sector, 0, SCREEN_WIDTH - 1 }},
        1
    };

    // nefig bilo mutit
    // /ban Krimax

    while (queue.n != 0) {
        // grab tail of queue
        struct queue_entry entry = queue.arr[--queue.n];

        // FIXME: fix algorism for recursive portals
        if (sectdraw[entry.id] >= 6) {
            continue;
        }

        sectdraw[entry.id]++;

        const sector_t* sector = &client_state.map.sectors.arr[entry.id];

        for (usize i = 0; i < sector->nwalls; i++) {
            const wall_t* wall =
                &client_state.map.walls.arr[sector->firstwall + i];

            // translate relative to player and rotate points around player's view
            const v2
                op0 = MATH_WorldPosToCamera(v2i_to_v2(wall->a)),
                op1 = MATH_WorldPosToCamera(v2i_to_v2(wall->b));

            // wall clipped pos
            v2 cp0 = op0, cp1 = op1;

            // both are negative -> wall is entirely behind player
            if (cp0.y <= 0 && cp1.y <= 0) {
                continue;
            }

            // angle-clip against view frustum
            f32
                ap0 = MATH_NormalizeAngle(atan2(cp0.y, cp0.x) - PI_2),
                ap1 = MATH_NormalizeAngle(atan2(cp1.y, cp1.x) - PI_2);

            // clip against view frustum if both angles are not clearly within
            // HFOV
            if (cp0.y < ZNEAR
                || cp1.y < ZNEAR
                || ap0 > +(HFOV / 2)
                || ap1 < -(HFOV / 2)) {
                const v2
                    il = MATH_IntersectSegs(cp0, cp1, znl, zfl),
                    ir = MATH_IntersectSegs(cp0, cp1, znr, zfr);

                // recompute angles if points change
                if (!isnan(il.x)) {
                    cp0 = il;
                    ap0 = MATH_NormalizeAngle(atan2(cp0.y, cp0.x) - PI_2);
                }

                if (!isnan(ir.x)) {
                    cp1 = ir;
                    ap1 = MATH_NormalizeAngle(atan2(cp1.y, cp1.x) - PI_2);
                }
            }

            if (ap0 < ap1) {
                continue;
            }

            if ((ap0 < -(HFOV / 2) && ap1 < -(HFOV / 2))
                || (ap0 > +(HFOV / 2) && ap1 > +(HFOV / 2))) {
                continue;
            }

            // "true" xs before portal clamping
            const int
                tx0 = MATH_ScreenAngleToX(ap0),
                tx1 = MATH_ScreenAngleToX(ap1);

            // bounds check against portal window
            if (tx0 > entry.x1) { continue; }
            if (tx1 < entry.x0) { continue; }

            const int wallshade =
                16 * (sin(atan2f(
                    wall->b.x - wall->a.x,
                    wall->b.y - wall->b.y)) + 1.0f);

            const int
                x0 = clamp(tx0, entry.x0, entry.x1),
                x1 = clamp(tx1, entry.x0, entry.x1);

            const f32
                z_floor = sector->zfloor,
                z_ceil = sector->zceil,
                nz_floor =
                wall->portal ? client_state.map.sectors.arr[wall->portal].zfloor : 0,
                nz_ceil =
                wall->portal ? client_state.map.sectors.arr[wall->portal].zceil : 0;

            const f32
                sy0 = ifnan((VFOV * SCREEN_HEIGHT) / cp0.y, 1e10),
                sy1 = ifnan((VFOV * SCREEN_HEIGHT) / cp1.y, 1e10);

            const f32 eye_z = player_state.camera.eye_z;
            const f32 vert_angl = player_state.camera.vert_angle * SCREEN_HEIGHT;

            const f32 wall_f0 = wall->f0;
            const f32 wall_f1 = wall->f1;

            const int
                yf0 = (SCREEN_HEIGHT / 2) + (int)(((z_floor + wall_f0) - eye_z) * sy0),
                yc0 = (SCREEN_HEIGHT / 2) + (int)((z_ceil - eye_z) * sy0),
                yf1 = (SCREEN_HEIGHT / 2) + (int)(((z_floor + wall_f1) - eye_z) * sy1),
                yc1 = (SCREEN_HEIGHT / 2) + (int)((z_ceil - eye_z) * sy1),
                nyf0 = (SCREEN_HEIGHT / 2) + (int)((nz_floor - eye_z) * sy0),
                nyc0 = (SCREEN_HEIGHT / 2) + (int)((nz_ceil - eye_z) * sy0),
                nyf1 = (SCREEN_HEIGHT / 2) + (int)((nz_floor - eye_z) * sy1),
                nyc1 = (SCREEN_HEIGHT / 2) + (int)((nz_ceil - eye_z) * sy1),
                txd = tx1 - tx0 ,
                yfd = yf1 - yf0,
                ycd = yc1 - yc0,
                nyfd = nyf1 - nyf0,
                nycd = nyc1 - nyc0;

            for (int x = x0; x <= x1; x++) {
                int shade = x == x0 || x == x1 ? 192 : (255 - wallshade);

                // calculate progress along x-axis via tx{0,1} so that walls
                // which are partially cut off due to portal edges still have
                // proper heights
                const f32 xp = ifnan((x - tx0) / (f32)txd, 0);
                // const f32 u_a = (((1.0f - xp) * z_floor) + (xp * z_ceil))
                //     / (((1.0f - xp) * znl) + (xp * znr));

                // get y coordinates for this x
                const int
                    tyf = (int)(xp * yfd) + yf0,
                    tyc = (int)(xp * ycd) + yc0,
                    yf = clamp(tyf + vert_angl, client_state.y_lo[x], client_state.y_hi[x]),
                    yc = clamp(tyc + vert_angl, client_state.y_lo[x], client_state.y_hi[x]);

                const u8 light = sector->light;

                // floor
                if (yf > client_state.y_lo[x]) {
                    D_VertLine(
                        client_state.y_lo[x],
                        yf,
                        x,
                        MATH_AbgrMul(0xFFFF0000, light));
                }

                // ceiling
                if (yc < client_state.y_hi[x]) {
                    D_VertLine(
                        yc,
                        client_state.y_hi[x],
                        x,
                        MATH_AbgrMul(0xFF00FFFF, light));
                }

                if (wall->portal) {
                    const int
                        tnyf = (int)(xp * nyfd) + nyf0,
                        tnyc = (int)(xp * nycd) + nyc0,
                        nyf = clamp(tnyf + vert_angl, client_state.y_lo[x], client_state.y_hi[x]),
                        nyc = clamp(tnyc + vert_angl, client_state.y_lo[x], client_state.y_hi[x]);

                    D_VertLine(
                        nyc,
                        yc,
                        x,
                        MATH_AbgrMul(MATH_AbgrMul(0xFF00FF00, shade), light));

                    D_VertLine(
                        yf,
                        nyf,
                        x,
                        MATH_AbgrMul(MATH_AbgrMul(0xFF0000FF, shade), light));

                    client_state.y_hi[x] = clamp(min(min(yc, nyc), client_state.y_hi[x]), 0, SCREEN_HEIGHT - 1);
                    client_state.y_lo[x] = clamp(max(max(yf, nyf), client_state.y_lo[x]), 0, SCREEN_HEIGHT - 1);
                }
                else {
                    D_VertLine(
                        yf,
                        yc,
                        x,
                        MATH_AbgrMul(MATH_AbgrMul(0xFFFFFFFF, shade), light));
                }

                if (client_state.sleepy) {
                    V_Present();
                    SDL_Delay(10);
                }
            }

            if (wall->portal) {
                ASSERT(queue.n != QUEUE_MAX, "R_Render: out of queue space");
                queue.arr[queue.n++] = (struct queue_entry){
                    .id = wall->portal,
                    .x0 = x0,
                    .x1 = x1
                };
            }
        }
    }

    client_state.sleepy = false;
}