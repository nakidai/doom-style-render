// Copyright (C) by gimura 2024
// --- DESCRIPTION ---
//
// types, defines, and etc
//
// -------------------

#ifndef _u_def_h
#define _u_def_h

// including standard libraries
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <limits.h>

// including SDL
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#define DLL_EXPORT
#include <SDL2/SDL_ttf.h>

#ifdef DEV_BUILD
#define ERROR(...) ({ fprintf(stderr, __VA_ARGS__); __asm__ __volatile__("int {$}3":); exit(1); }) // error macros
#else
#define ERROR(...) ({ fprintf(stderr, __VA_ARGS__); exit(1); }) // error macros
#endif

#define ASSERT(_e, ...) if (!(_e)) { ERROR(__VA_ARGS__); }      // assert macros

// new types
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

// render sizes
#define SCREEN_WIDTH 384
#define SCREEN_HEIGHT 216

#endif