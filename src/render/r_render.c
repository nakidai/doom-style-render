#include "../cl_def.h"

render_state_t g_cRenderstate;
extern state_t g_cState;
extern vidstate_t g_cVidstate;
extern playerstate_t g_cPlayerstate;

// rotate vector v by angle a
static inline v2 R_Rotate(v2 v, f32 a) {
    return (v2) {
        (v.x * cos(a)) - (v.y * sin(a)),
            (v.x * sin(a)) + (v.y * cos(a)),
    };
}

// see: https://en.wikipedia.org/wiki/Line–line_intersection
// compute intersection of two line segments, returns (NAN, NAN) if there is
// no intersection.
static inline v2 R_IntersectSegs(v2 a0, v2 a1, v2 b0, v2 b1) {
    const f32 d =
        ((a0.x - a1.x) * (b0.y - b1.y))
        - ((a0.y - a1.y) * (b0.x - b1.x));

    if (fabsf(d) < 0.000001f) { return (v2) { NAN, NAN }; }

    const f32
        t = (((a0.x - b0.x) * (b0.y - b1.y))
            - ((a0.y - b0.y) * (b0.x - b1.x))) / d,
        u = (((a0.x - b0.x) * (a0.y - a1.y))
            - ((a0.y - b0.y) * (a0.x - a1.x))) / d;
    return (t >= 0 && t <= 1 && u >= 0 && u <= 1) ?
        ((v2) {
        a0.x + (t * (a1.x - a0.x)),
            a0.y + (t * (a1.y - a0.y))
    })
        : ((v2) {
        NAN, NAN
    });
}

static inline u32 R_AbgrMul(u32 col, u32 a) {
    const u32
        br = ((col & 0xFF00FF) * a) >> 8,
        g = ((col & 0x00FF00) * a) >> 8;

    return 0xFF000000 | (br & 0xFF00FF) | (g & 0x00FF00);
}

// convert angle in [-(HFOV / 2)..+(HFOV / 2)] to X coordinate
static inline int R_ScreenAngleToX(f32 angle) {
    return
        ((int)(SCREEN_WIDTH / 2))
        * (1.0f - tan(((angle + (HFOV / 2.0)) / HFOV) * PI_2 - PI_4));
}

// noramlize angle to +/-PI
static inline f32 R_NormalizeAngle(f32 a) {
    return a - (TAU * floor((a + PI) / TAU));
}

// world space -> camera space (translate and rotate)
static inline v2 R_WorldPosToCamera(v2 p) {
    const v2 u = { p.x - g_cPlayerstate.camera.pos.x, p.y - g_cPlayerstate.camera.pos.y };
    return (v2) {
        u.x* g_cPlayerstate.camera.anglesin - u.y * g_cPlayerstate.camera.anglecos,
            u.x* g_cPlayerstate.camera.anglecos + u.y * g_cPlayerstate.camera.anglesin,
    };
}

void R_Render() {
    for (int i = 0; i < SCREEN_WIDTH; i++) {
        g_cState.y_hi[i] = SCREEN_HEIGHT - 1;
        g_cState.y_lo[i] = 0;
    }

    // track if sector has already been drawn
    bool sectdraw[SECTOR_MAX];
    memset(sectdraw, 0, sizeof(sectdraw));

    // calculate edges of near/far planes (looking down +Y axis)
    const v2
        zdl = R_Rotate(((v2) { 0.0f, 1.0f }), +(HFOV / 2.0f)),
        zdr = R_Rotate(((v2) { 0.0f, 1.0f }), -(HFOV / 2.0f)),
        znl = (v2){ zdl.x * ZNEAR, zdl.y * ZNEAR },
        znr = (v2){ zdr.x * ZNEAR, zdr.y * ZNEAR },
        zfl = (v2){ zdl.x * ZFAR, zdl.y * ZFAR },
        zfr = (v2){ zdr.x * ZFAR, zdr.y * ZFAR };

    enum { QUEUE_MAX = 64 };
    struct queue_entry { int id, x0, x1; };

    struct { struct queue_entry arr[QUEUE_MAX]; usize n; } queue = {
        {{ g_cPlayerstate.sector, 0, SCREEN_WIDTH - 1 }},
        1
    };

    // nefig bilo mutit
    // /ban Krimax

    while (queue.n != 0) {
        // grab tail of queue
        struct queue_entry entry = queue.arr[--queue.n];

        if (sectdraw[entry.id]) {
            continue;
        }

        sectdraw[entry.id] = true;

        const sector_t* sector = &g_cState.map.sectors.arr[entry.id];

        for (usize i = 0; i < sector->nwalls; i++) {
            const wall_t* wall =
                &g_cState.map.walls.arr[sector->firstwall + i];

            // translate relative to player and rotate points around player's view
            const v2
                op0 = R_WorldPosToCamera(v2i_to_v2(wall->a)),
                op1 = R_WorldPosToCamera(v2i_to_v2(wall->b));

            // wall clipped pos
            v2 cp0 = op0, cp1 = op1;

            // both are negative -> wall is entirely behind player
            if (cp0.y <= 0 && cp1.y <= 0) {
                continue;
            }

            // angle-clip against view frustum
            f32
                ap0 = R_NormalizeAngle(atan2(cp0.y, cp0.x) - PI_2),
                ap1 = R_NormalizeAngle(atan2(cp1.y, cp1.x) - PI_2);

            // clip against view frustum if both angles are not clearly within
            // HFOV
            if (cp0.y < ZNEAR
                || cp1.y < ZNEAR
                || ap0 > +(HFOV / 2)
                || ap1 < -(HFOV / 2)) {
                const v2
                    il = R_IntersectSegs(cp0, cp1, znl, zfl),
                    ir = R_IntersectSegs(cp0, cp1, znr, zfr);

                // recompute angles if points change
                if (!isnan(il.x)) {
                    cp0 = il;
                    ap0 = R_NormalizeAngle(atan2(cp0.y, cp0.x) - PI_2);
                }

                if (!isnan(ir.x)) {
                    cp1 = ir;
                    ap1 = R_NormalizeAngle(atan2(cp1.y, cp1.x) - PI_2);
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
                tx0 = R_ScreenAngleToX(ap0),
                tx1 = R_ScreenAngleToX(ap1);

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
                wall->portal ? g_cState.map.sectors.arr[wall->portal].zfloor : 0,
                nz_ceil =
                wall->portal ? g_cState.map.sectors.arr[wall->portal].zceil : 0;

            const f32
                sy0 = ifnan((VFOV * SCREEN_HEIGHT) / cp0.y, 1e10),
                sy1 = ifnan((VFOV * SCREEN_HEIGHT) / cp1.y, 1e10);

            const f32 eye_z = g_cPlayerstate.camera.eye_z;
            const f32 vert_angl = g_cPlayerstate.camera.vert_angle * SCREEN_HEIGHT;

            const int
                yf0 = (SCREEN_HEIGHT / 2) + (int)((z_floor - eye_z) * sy0),
                yc0 = (SCREEN_HEIGHT / 2) + (int)((z_ceil - eye_z) * sy0),
                yf1 = (SCREEN_HEIGHT / 2) + (int)((z_floor - eye_z) * sy1),
                yc1 = (SCREEN_HEIGHT / 2) + (int)((z_ceil - eye_z) * sy1),
                nyf0 = (SCREEN_HEIGHT / 2) + (int)((nz_floor - eye_z) * sy0),
                nyc0 = (SCREEN_HEIGHT / 2) + (int)((nz_ceil - eye_z) * sy0),
                nyf1 = (SCREEN_HEIGHT / 2) + (int)((nz_floor - eye_z) * sy1),
                nyc1 = (SCREEN_HEIGHT / 2) + (int)((nz_ceil - eye_z) * sy1),
                txd = tx1 - tx0,
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
                    yf = clamp(tyf + vert_angl, g_cState.y_lo[x], g_cState.y_hi[x]),
                    yc = clamp(tyc + vert_angl, g_cState.y_lo[x], g_cState.y_hi[x]);

                // floor
                if (yf > g_cState.y_lo[x]) {
                    D_VertLine(
                        g_cState.y_lo[x],
                        yf,
                        x,
                        0xFFFF0000);
                }

                // ceiling
                if (yc < g_cState.y_hi[x]) {
                    D_VertLine(
                        yc,
                        g_cState.y_hi[x],
                        x,
                        0xFF00FFFF);
                }

                if (wall->portal) {
                    const int
                        tnyf = (int)(xp * nyfd) + nyf0,
                        tnyc = (int)(xp * nycd) + nyc0,
                        nyf = clamp(tnyf + vert_angl, g_cState.y_lo[x], g_cState.y_hi[x]),
                        nyc = clamp(tnyc + vert_angl, g_cState.y_lo[x], g_cState.y_hi[x]);

                    D_VertLine(
                        nyc,
                        yc,
                        x,
                        R_AbgrMul(0xFF00FF00, shade));

                    D_VertLine(
                        yf,
                        nyf,
                        x,
                        R_AbgrMul(0xFF0000FF, shade));

                    g_cState.y_hi[x] = clamp(min(min(yc, nyc), g_cState.y_hi[x]), 0, SCREEN_HEIGHT - 1);
                    g_cState.y_lo[x] = clamp(max(max(yf, nyf), g_cState.y_lo[x]), 0, SCREEN_HEIGHT - 1);
                }
                else {
                    // textured_verline(
                    //     x, yf, yc,
                    //     yf, yc, u_a, state.debug_texture);

                    D_VertLine(
                        yf,
                        yc,
                        x,
                        R_AbgrMul(0xFFFFFFFF, shade));

                    // D_TextLine(
                    //     yf,
                    //     yc,
                    //     x,
                    //     0);
                }

                if (g_cState.sleepy) {
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

    g_cState.sleepy = false;
}

void R_Init(void) {
    texture_t* tex = &g_cRenderstate.textures[0];
    tex->size = (v2i) { 32, 32 };
    T_AllocTexture(tex);
    T_GenDebugTexture(tex);
}

void R_Free(void) {
    T_FreeTexture(&g_cRenderstate.textures[0]);
}