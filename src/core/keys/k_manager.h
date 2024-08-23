// Copyright (C) by gimura 2024
// --- PREFIX ---
//
// KEY_* - key manager functions
//
// --- DESCRIPTION ---
//
// key manager
//
// -------------------

#ifndef k_manager_h
#define k_manager_h

#include "utils/u_def.h"

typedef enum {
    KEY_DOWN, // only key down
    KEY_UP,   // only key up
    KEY_ANY   // any event
} key_event_t; // key press enum
typedef void (*key_handler_t)(int, key_event_t); // key handler function type

void KEY_Init(void); // init key manager
void KEY_Update(void); // update key manager

void KEY_AddKeyHandler(key_handler_t fn); // add key handler

#endif