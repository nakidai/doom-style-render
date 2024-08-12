#ifndef _u_math_h
#define _u_math_h

#include "../u_def.h"

#define PI 3.14159265359f
#define TAU (2.0f * PI)
#define PI_2 (PI / 2.0f)
#define PI_4 (PI / 4.0f)

#define DEG2RAD(_d) ((_d) * (PI / 180.0f))
#define RAD2DEG(_d) ((_d) * (180.0f / PI))

typedef struct v2_s { f32 x, y; } v2;
typedef struct v2i_s { i32 x, y; } v2i;

#define v2_to_v2i(_v) ({ typeof(_v) __v = (_v); (v2i) { __v.x, __v.y }; })
#define v2i_to_v2(_v) ({ typeof(_v) __v = (_v); (v2) { __v.x, __v.y }; })

#define dot(_v0, _v1) ({ typeof(_v0) __v0 = (_v0), __v1 = (_v1); (__v0.x * __v1.x) + (__v0.y * __v1.y); })
#define length(_vl) ({ typeof(_vl) __vl = (_vl); sqrtf(dot(__vl, __vl)); })
#define normalize(_vn) ({ typeof(_vn) __vn = (_vn); const f32 l = length(__vn); (typeof(_vn)) { __vn.x / l, __vn.y / l }; })
#define min(_a, _b) ({ typeof(_a) __a = (_a), __b = (_b); __a < __b ? __a : __b; })
#define max(_a, _b) ({ typeof(_a) __a = (_a), __b = (_b); __a > __b ? __a : __b; })
#define clamp(_x, _mi, _ma) (min(max(_x, _mi), _ma))
#define ifnan(_x, _alt) ({ typeof(_x) __x = (_x); isnan(__x) ? (_alt) : __x; })

v2 R_IntersectSegs(v2 a0, v2 a1, v2 b0, v2 b1);
int R_ScreenAngleToX(f32 angle);
v2 R_WorldPosToCamera(v2 p);
v2 R_Rotate(v2 v, f32 a);

extern __cdecl u32 R_AbgrMul(u32 col, u32 a);
extern __cdecl f32 R_NormalizeAngle(f32 a);

#endif