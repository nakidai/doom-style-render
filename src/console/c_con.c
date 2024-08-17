#include "cl_def.h"

char con_buf[1024];
static char con_in[32];

cmd_var_t test_variable = { "test", "this is test variable" };

extern state_t    g_cState;
extern vidstate_t g_cVidstate;

int CMD_ToggleConsole(char* args) {
    g_cState.state = g_cState.state == CONSOLE_STATE ? LEVEL_STATE : CONSOLE_STATE;
    return SUCCESS;
}

void CON_Init(void) {
    CON_DrawInit();

    CMD_AddCommand("toggle_console", &CMD_ToggleConsole);

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

static void CON_ProcessInput(const char* text) {
    if (strlen(con_in) >= sizeof(con_in)) return;
    strcat(con_in, text);
}

static void CON_Exec(void) {
    if (strlen(con_in) == 0) return;

    char out_buf[32];
    snprintf(out_buf, sizeof(out_buf), "Command exited with code %i\n", CMD_ExecuteText(con_in));
    CON_Printf(out_buf);

    memset(con_in, '\0', sizeof(con_in));
}

void CON_Update(void) {
    for (usize i = 0; i < g_cState.event_count; i++) {
        SDL_Event ev = g_cState.events[i];
        
        switch (ev.type) {
            case SDL_TEXTINPUT:
                if (g_cState.state != CONSOLE_STATE) break;
                CON_ProcessInput(ev.text.text);
                break;

            case SDL_KEYDOWN:
                if (ev.key.keysym.sym == SDLK_RETURN) CON_Exec();
                break;
            
            default:
                break;
        }
    }
}