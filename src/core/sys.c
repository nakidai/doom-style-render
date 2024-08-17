#include "../cl_def.h"

static int    argc;
static char** argv;

void SYS_Init(int _argc, char** _argv) {
    argc = _argc;
    argv = _argv;
}

char* SYS_GetArg(const char* name) {
    char* out = NULL;

    for (int i = 0; i < argc; i++) {
        if (strcmp(name, argv[i]) == 0) {
            if (i + 1 >= argc) goto done;

            out = argv[i + 1];
            goto done;
        }
    }

    done: return out;
}

#ifdef DEV_BUILD
#define DEFAULT_BASE_DIR "./../../"
#define DEFAULT_GAME_DIR "res/"
#else
#define DEFAULT_BASE_DIR "./"
#define DEFAULT_GAME_DIR "game/"
#endif

char* SYS_GetGameDir(void) {
    char out[64];
    memset(out, '\0', sizeof(out));

    const char* base_dir = SYS_GetArg("-baseDir");
    const char* game_dir = SYS_GetArg("-gameDir");

    strcat(out, base_dir == NULL ? DEFAULT_BASE_DIR : base_dir);
    strcat(out, game_dir == NULL ? DEFAULT_GAME_DIR : game_dir);

    char* buf = M_TempAlloc(strlen(out));
    memcpy(buf, out, strlen(out));

    return buf;
}

u8* SYS_ReadFile(const char* name) {
    char  path[64];
    char* game_dir = SYS_GetGameDir();

    sprintf(path, "%s%s", game_dir, name);

    M_TempFree(game_dir);

    FILE* file = fopen(path, "rb");
    if (file == NULL) ERROR("SYS_ReadFile: file %s not found!", path);

    fseek(file, 0L, SEEK_END);
    const usize size = ftell(file);
    rewind(file);

    u8* data = M_TempAlloc(size);
    fread(data, size, 1, file);
    fclose(file);

    return data;
}

usize SYS_ReadFileSize(const char* name) {
    char  path[64];
    char* game_dir = SYS_GetGameDir();

    sprintf(path, "%s%s", game_dir, name);

    M_TempFree(game_dir);

    FILE* file = fopen(path, "rb");
    if (file == NULL) ERROR("SYS_ReadFileSize: file %s not found!", path);

    fseek(file, 0L, SEEK_END);
    const usize size = ftell(file);
    fclose(file);

    return size;
}