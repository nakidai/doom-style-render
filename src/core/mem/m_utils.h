#ifndef _m_utils_h
#define _m_utils_h

#define KB(x) x * 1024
#define MB(x) KB(x) * 1024
#define GB(x) MB(x) * 1024

usize M_ReadMemSize(const char* size_str);

#endif