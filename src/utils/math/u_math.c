#include "../../cl_def.h"

v2 MATH_Rotate(v2 v, f32 a) {
    return (v2) {
        (v.x * cos(a)) - (v.y * sin(a)),
            (v.x * sin(a)) + (v.y * cos(a)),
    };
}

v2 MATH_IntersectSegs(v2 a0, v2 a1, v2 b0, v2 b1) {
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

int MATH_ScreenAngleToX(f32 angle) {
    return
        ((int)(SCREEN_WIDTH / 2))
        * (1.0f - tan(((angle + (HFOV / 2.0)) / HFOV) * PI_2 - PI_4));
}

v2 MATH_WorldPosToCamera(v2 p, v2 cam, float as, float ac) {
    const v2 u = { p.x - cam.x, p.y - cam.y };
    return (v2) {
        u.x* as - u.y * ac,
            u.x* ac + u.y * as,
    };
}