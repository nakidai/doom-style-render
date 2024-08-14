#include "cl_def.h"

char con_buf[1024];
static char con_in[32];

int CMD_Echo(char* args) {
    char buf[64];
    snprintf(buf, sizeof(buf), "%s\n", args);

    CON_Printf(buf);
    return SUCCESS;
}

void CON_Init(void) {
    CON_DrawInit();

    CMD_AddCommand("echo", &CMD_Echo);

    CON_Printf("console init done!\n");
}

void CON_Free(void) {
    CON_DrawFree();
}

void CON_Draw(void) {
    CON_DrawString((v2i) { 10, SCREEN_HEIGHT - 10 }, con_buf);
    CON_DrawString((v2i) { 10, 10 }, con_in);
}

void CON_Printf(const char* msg) {
    strcat(con_buf, msg);
}

void CON_ProcessInput(const char* text) {
    if (strlen(con_in) >= sizeof(con_in)) return;
    strcat(con_in, text);
}

void CON_Exec(void) {
    char* buf_ptr = M_TempAlloc(strlen(con_in));
    memcpy(buf_ptr, con_in, strlen(con_in));

    char out_buf[32];
    snprintf(out_buf, sizeof(out_buf), "Command exited with code %i\n", CMD_ExecuteText(buf_ptr));
    CON_Printf(out_buf);

    memset(con_in, '\0', sizeof(con_in));
    M_TempFree(buf_ptr);
}