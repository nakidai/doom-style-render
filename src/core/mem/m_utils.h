// Copyright (C) by gimura 2024
// --- PREFIX ---
//
// M_* - memory functions
//
// --- DESCRIPTION ---
//
// memory utils
//
// -------------------

#ifndef _m_utils_h
#define _m_utils_h

#define KB(x) x * 1024     // convert kilobytes to bytes
#define MB(x) KB(x) * 1024 // convert megabytes to bytes
#define GB(x) MB(x) * 1024 // convert gigabytes to bytes

usize M_ReadMemSize(const char* size_str); // read size from string

#endif