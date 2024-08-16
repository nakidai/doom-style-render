#ifndef k_manager_h
#define k_manager_h

#include "utils/u_def.h"

typedef enum {
    KEY_DOWN,
    KEY_UP,
    KEY_ANY
} key_event_t;
typedef void (*key_handler_t)(int, key_event_t);

void KEY_Init(void);
void KEY_Update(void);

void KEY_AddKeyHandler(key_handler_t fn);

#endif