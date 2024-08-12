#include "../../../cl_def.h"

// noramlize angle to +/-PI
f32 R_NormalizeAngle(f32 a) {
    // TODO: fix fast realization
    // return a - (TAU * (int) ((a + PI) / TAU));
    return a - (TAU * floor((a + PI) / TAU));
}

u32 R_AbgrMul(u32 col, u32 a) {
    const u32
        br = ((col & 0xFF00FF) * a) >> 8,
        g = ((col & 0x00FF00) * a) >> 8;

    return 0xFF000000 | (br & 0xFF00FF) | (g & 0x00FF00);
}