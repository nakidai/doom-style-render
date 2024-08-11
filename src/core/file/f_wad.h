#ifndef _f_wad_h
#define _f_wad_h

#include "../../utils/u_def.h"

#define MAX_DIRECTORIES 1024

typedef struct {
    char WADtype[5];
    u32 dir_count;
    u32 dir_offset;
} wad_header_t;

typedef struct {
    char lump_name[9];
    u32 lump_offset;
    u32 lump_size;
} wad_directory_t;

typedef struct { 
    u8* data;

    wad_header_t header;
    wad_directory_t dirs[MAX_DIRECTORIES];
} wad_t;

void W_LoadWAD(wad_t* wad);
void W_CloseWAD(wad_t* wad);

u16 W_Read2Bytes(const u8* data, int offset);
u32 W_Read4Bytes(const u8* data, int offset);

wad_directory_t W_FindDirectory(wad_t* wad, const char* name);
void W_ReadWADDirectory(const wad_t* wad, const wad_directory_t directory, u8* buffer);

#endif