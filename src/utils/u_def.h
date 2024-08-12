#ifndef _u_def_h
#define _u_def_h

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <string.h>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#define DEV_BUILD
#define PARANOID

#ifdef DEV_BUILD
#define DEBUG_PRINT_INIT
// #define DEBUG_PRINT_RUN
#endif

#define ERROR(...) ({ fprintf(stderr, __VA_ARGS__); exit(1); })
#define ASSERT(_e, ...) if (!(_e)) { ERROR(__VA_ARGS__); }

typedef float    f32;
typedef double   f64;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef size_t   usize;

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

#define SCREEN_WIDTH 384
#define SCREEN_HEIGHT 216

#endif