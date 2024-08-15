#include "cl_def.h"

char con_buf[1024];
static char con_in[32];

cmd_var_t test_variable = { "test", "this is test variable" };

extern state_t    g_cState;
extern vidstate_t g_cVidstate;

int CMD_Echo(char* args) {
    char buf[64];
    snprintf(buf, sizeof(buf), "%s\n", args);

    CON_Printf(buf);
    return SUCCESS;
}

int CMD_LoadMap(char* args) {
    G_LoadMap(&g_cState.map, args);
    
    return SUCCESS;
}

int CMD_ExecCommand(char* args) {
    FILE* file = fopen(args, "r");

    if (file == NULL) return 2;

    char line[64];
    while (fgets(line, 64, file)) {
        if (line[strlen(line)] == '\n') line[strlen(line)] = '\0';
        CMD_ExecuteText(line);
    }

    fclose(file);
    
    return SUCCESS;
}

void CON_Init(void) {
    CON_DrawInit();

    CMD_AddCommand("echo", &CMD_Echo);
    CMD_AddCommand("map",  &CMD_LoadMap);
    CMD_AddCommand("exec", &CMD_ExecCommand);

    CMD_AddVariable(&test_variable);

    CON_Printf("console init done!\n");
}

void CON_Free(void) {
    CON_DrawFree();
}

void CON_Draw(void) {
    GFX_Blackout(128, (v2i) { 0, 0 }, (v2i) { SCREEN_WIDTH, SCREEN_HEIGHT });

    CON_DrawString((v2i) { 5, SCREEN_HEIGHT - 10 }, con_buf);

    char buf[64];
    snprintf(buf, sizeof(buf), "] %s", con_in);

    CON_DrawString((v2i) { 10, 10 }, buf);
}

void CON_Printf(const char* msg) {
    strcat(con_buf, msg);
}

void CON_ProcessInput(const char* text) {
    if (strlen(con_in) >= sizeof(con_in)) return;
    strcat(con_in, text);
}

void CON_Exec(void) {
    if (strlen(con_in) == 0) return;

    char* buf_ptr = M_TempAlloc(strlen(con_in));
    memcpy(buf_ptr, con_in, strlen(con_in));
    memset(buf_ptr + strlen(con_in), '\0', sizeof(con_in) - strlen(con_in));

    char out_buf[32];
    snprintf(out_buf, sizeof(out_buf), "Command exited with code %i\n", CMD_ExecuteText(buf_ptr));
    CON_Printf(out_buf);

    memset(con_in, '\0', sizeof(con_in));
    M_TempFree(buf_ptr);
}