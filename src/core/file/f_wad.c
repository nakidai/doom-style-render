#include "../../cl_def.h"

#ifdef DEBUG_PRINT_INIT
#define PRINT(...) ({ printf(__VA_ARGS__); })
#else
#define PRINT(...)
#endif

u16 W_Read2Bytes(const u8* data, int offset) {
    u16 value;
    memcpy(&value, data + offset, sizeof(u16));
    return value;
}

u32 W_Read4Bytes(const u8* data, int offset) {
    u32 value;
    memcpy(&value, data + offset, sizeof(u32));
    return value;
}

static inline const char* W_FindWadPath(void) {
    return "../res/test.wad";
}

static void W_ReadWAD(wad_t* wad) {
    const char* path = W_FindWadPath();
    FILE* file = fopen(path, "rb");

    ASSERT(
        file != NULL,
        "W_ReadWAD: file %s not found (how come W_FindWadPath didn't throw an error?)\n",
        path
    )

    fseek(file, 0L, SEEK_END);
    const usize size = ftell(file);
    rewind(file);

    wad->data = M_GlobAlloc(size);
    fread(wad->data, size, 1, file);

    fclose(file);
}

static void W_ReadHeader(const u8* data, wad_header_t* header) {
    header->WADtype[0] = data[0];
    header->WADtype[1] = data[1];
    header->WADtype[2] = data[2];
    header->WADtype[3] = data[3];
    header->WADtype[4] = '\0';

    header->dir_count = W_Read4Bytes(data, 4);
    header->dir_offset = W_Read4Bytes(data, 8);
}

static void W_CheckHeader(const wad_header_t header) {
    ASSERT(
        !strcmp(header.WADtype, "PWAD"),
        "W_CheckHeader: invalid WAD header: %s\n",
        header.WADtype
    )
}

static void W_ReadDirectory(const u8* data, int offset, wad_directory_t* directory) {
    directory->lump_offset = W_Read4Bytes(data, offset);
    directory->lump_size = W_Read4Bytes(data, offset + 4);
    
    directory->lump_name[0] = data[offset + 8];
    directory->lump_name[1] = data[offset + 9];
    directory->lump_name[2] = data[offset + 10];
    directory->lump_name[3] = data[offset + 11];
    directory->lump_name[4] = data[offset + 12];
    directory->lump_name[5] = data[offset + 13];
    directory->lump_name[6] = data[offset + 14];
    directory->lump_name[7] = data[offset + 15];
    directory->lump_name[8] = '\0';
}

void W_LoadWAD(wad_t* wad) {
    PRINT("W_LoadWAD: reading WAD file\n");
    W_ReadWAD(wad);

    PRINT("W_LoadWAD: reading header\n");
    W_ReadHeader(wad->data, &wad->header);
    W_CheckHeader(wad->header);

    PRINT("W_LoadWAD: header: type: %s\n", wad->header.WADtype);
    PRINT("W_LoadWAD: header: dir_count: %i\n", wad->header.dir_count);
    PRINT("W_LoadWAD: header: dir_offset: %i\n", wad->header.dir_offset);

    PRINT("W_LoadWAD: reading directories\n");
    for (u32 i = 0; i < wad->header.dir_count; i++) {
        wad_directory_t dir = {};

        W_ReadDirectory(
            wad->data,
            wad->header.dir_offset + i * 16,
            &dir
        );

        PRINT("W_ReadWADDirectory: reading directory %s\n", dir.lump_name);
        PRINT("W_ReadWADDirectory: offset: %i\n", dir.lump_offset);
        PRINT("W_ReadWADDirectory: size: %i\n", dir.lump_size);

        wad->dirs[i] = dir;
    }

    PRINT("W_LoadWAD: done reading\n");
}

void W_CloseWAD(wad_t* wad) {
    M_GlobFree(wad->data);
}

wad_directory_t W_FindDirectory(wad_t* wad, const char* name) {
    for (u32 i = 0; i < wad->header.dir_count; i++) {
        const wad_directory_t dir = wad->dirs[i];

        if (!strcmp(dir.lump_name, name)) {
            return dir;
        }
    }

    ERROR("W_FindDirectory: lump %s not found!\n", name);
}

void W_ReadWADDirectory(const wad_t* wad, const wad_directory_t directory, u8* buffer) {
    PRINT("W_ReadWADDirectory: reading directory %s\n", directory.lump_name);
    PRINT("W_ReadWADDirectory: offset: %i\n", directory.lump_offset);
    PRINT("W_ReadWADDirectory: size: %i\n", directory.lump_size);

    memcpy(buffer, wad->data + directory.lump_offset, directory.lump_size);
}

#undef PRINT