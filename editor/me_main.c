#include "me_def.h"

usize     event_count = 0; // app event count
SDL_Event events[128];     // app event list
bool      quit = false;

editor_state_t    editor_state;
extern vidstate_t video_state;

static void ME_DrawLine(v2i a, v2i b, u32 color) {
    a.x = clamp(a.x, 1, SCREEN_WIDTH);
    a.y = clamp(a.y, 1, SCREEN_HEIGHT);

    b.x = clamp(b.x, 1, SCREEN_WIDTH);
    b.y = clamp(b.y, 1, SCREEN_HEIGHT);

    int x1 = floor(a.x);
    int x2 = floor(b.x);

    if (x1 > x2) {
        int temp_i = x1;
        x1 = x2;
        x2 = temp_i;

        v2i temp_v = a;
        a = b;
        b = temp_v;
    }

    float d = (b.y - a.y) / clamp(x2 - x1, 1, SCREEN_WIDTH);
    float y = a.y;
    
    for (register int i = x1; i <= x2; i++) {
        int pix_y = floor(y);
        video_state.pixels[pix_y * SCREEN_WIDTH + i] = color;
        y += d;
    }
}

#define GIRD_SIZE 512

static void ME_DrawGrid(void) {
    for (int i = 0; i < GIRD_SIZE; i += editor_state.grid_res) {
        D_VertLine(
            clamp(editor_state.pix_pos.y,             0, SCREEN_HEIGHT),
            clamp(editor_state.pix_pos.y + GIRD_SIZE, 0, SCREEN_HEIGHT),
            clamp(editor_state.pix_pos.x + i,         0, SCREEN_WIDTH),
            0xFFFFFFFF
        );

        D_HorsLine(
            clamp(editor_state.pix_pos.x,             0, SCREEN_WIDTH),
            clamp(editor_state.pix_pos.x + GIRD_SIZE, 0, SCREEN_WIDTH),
            clamp(editor_state.pix_pos.y + i,         0, SCREEN_HEIGHT),
            0xFFFFFFFF
        );
    }
}

static void ME_DrawMap(void) {
    for (int i = 0; i < editor_state.walls.n; i++) {
        const wall_t* wall = &editor_state.walls.arr[i];

        ME_DrawLine(
            (v2i) {
                clamp(wall->a.x + editor_state.pix_pos.x, 0, SCREEN_WIDTH),
                clamp(wall->a.y + editor_state.pix_pos.y, 0, SCREEN_HEIGHT),
            },
            (v2i) {
                clamp(wall->b.x + editor_state.pix_pos.x, 0, SCREEN_WIDTH),
                clamp(wall->b.y + editor_state.pix_pos.y, 0, SCREEN_HEIGHT),
            },
            0xFF00FF00
        );
    }
}

static void ME_Draw(void) {
    ME_DrawGrid();
    ME_DrawMap();
}

#define MOVE_CMD(x, y) ({ x = y; return SUCCESS; })
#define P_MOV(x) MOVE_CMD(x, true)
#define M_MOV(x) MOVE_CMD(x, false)

static int CMD_PlusForward(char* args __attribute__((unused)))  { P_MOV(editor_state.forward); }
static int CMD_MinusForward(char* args __attribute__((unused))) { M_MOV(editor_state.forward); }

static int CMD_PlusBack(char* args __attribute__((unused)))  { P_MOV(editor_state.back); }
static int CMD_MinusBack(char* args __attribute__((unused))) { M_MOV(editor_state.back); }

static int CMD_PlusLeft(char* args __attribute__((unused)))  { P_MOV(editor_state.left); }
static int CMD_MinusLeft(char* args __attribute__((unused))) { M_MOV(editor_state.left); }

static int CMD_PlusRight(char* args __attribute__((unused)))  { P_MOV(editor_state.right); }
static int CMD_MinusRight(char* args __attribute__((unused))) { M_MOV(editor_state.right); }

static int CMD_ToggleConsole(char* args __attribute__((unused))) {
    editor_state.console = !editor_state.console;
    return SUCCESS;
}

static int CMD_ToggleConsole(char* args) {
    if (strcmp(args, "cursor") == 0) {
        
    }

    return SUCCESS;
}

static void ME_InitCommands(void) {
    CMD_AddCommand("+forward", &CMD_PlusForward);
    CMD_AddCommand("-forward", &CMD_MinusForward);
    CMD_AddCommand("+back",    &CMD_PlusBack);
    CMD_AddCommand("-back",    &CMD_MinusBack);
    CMD_AddCommand("+left",    &CMD_PlusLeft);
    CMD_AddCommand("-left",    &CMD_MinusLeft);
    CMD_AddCommand("+right",   &CMD_PlusRight);
    CMD_AddCommand("-right",   &CMD_MinusRight);

    CMD_AddCommand("toggle_console", &CMD_ToggleConsole);
}

static void ME_Init(int argc, char** argv) {
    SYS_Init(argc, argv);
    M_Init();

    KEY_Init();

    CMD_Init();
    CON_Init();
    ME_InitCommands();

    V_Init();
    R_Init();

    editor_state.grid_res = 30;
    editor_state.walls.arr[editor_state.walls.n++] = (wall_t) {
        (v2i) { 0, 0 },
        (v2i) { 40, 50 },
        0
    };

    CMD_ExecuteText("exec editor.cfg");
}

// process SDL events
static void CL_CheckWindowEvents(void) {
    event_count = 0; // sets event count no zero 
    SDL_Event ev;    // variable for event

    // event check loop
    while (SDL_PollEvent(&ev)) {
        events[event_count++] = ev; // write event to event array

        // check type
        switch (ev.type) {
            case SDL_QUIT: // if window close event, quit
                quit = true;
                break;
            
            default: // else other, continue
                break;
        }
    }
}

#define MOVE_SPEED 0.8f

static void ME_MoveCamera(void) {
    if (editor_state.forward) editor_state.pos.y -= MOVE_SPEED;
    if (editor_state.back)    editor_state.pos.y += MOVE_SPEED;
    if (editor_state.left)    editor_state.pos.x += MOVE_SPEED;
    if (editor_state.right)   editor_state.pos.x -= MOVE_SPEED;

    editor_state.pix_pos.x = (int) editor_state.pos.x;
    editor_state.pix_pos.y = (int) editor_state.pos.y;
}

static void ME_MainLoop(void) {
    // init variable for calculate delta time
    u64 now = SDL_GetPerformanceCounter(),
        last = 0;

    // main loop
    while (!quit) {
        CL_CheckWindowEvents(); // process events

        // if quit in true, break
        if (quit) {
            break;
        }

        // calculate delta time
        last = now;
        now = SDL_GetPerformanceCounter();

        KEY_Update(); // process keys

        V_Update(); // update video (clear screen buffer)

        if (editor_state.console) {
            CON_Update();
            CON_Draw();
        }

        ME_Draw();
        ME_MoveCamera();

        V_Present();
    }
}

static void ME_Free(void) {
    CON_Free();
    R_Free();
    V_Free();
    
    M_Free();
}

int main(int argc, char* argv[]) {
    ME_Init(argc, argv);
    ME_MainLoop();
    ME_Free();

    return 0;
}