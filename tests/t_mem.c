#include "cl_def.h"

static void T_BadGlobAllocSize(void) {
    void* alloc = M_TryGlobAlloc(GLOB_HEAP + 1);
    if (alloc != NULL) ERROR("T_BadGlobAllocSize: M_TryGlobAlloc error");
}

static void T_BadTempAllocSize(void) {
    void* alloc = M_TryTempAlloc(GLOB_HEAP + 1);
    if (alloc != NULL) ERROR("T_BadGlobAllocSize: M_TryTempAlloc error");
}

static void T_BadAllocSize(void) {
    T_BadGlobAllocSize();
    T_BadTempAllocSize();
}

static void T_MemUtils(void) {
    // M_ReadMemSize
#   define MEM_SIZE_ERR(x, y) if (M_ReadMemSize(x) != (usize) y) goto read_mem_error;
    
    MEM_SIZE_ERR("10M",   MB(10))
    MEM_SIZE_ERR("13M",   MB(13))
    MEM_SIZE_ERR("1231M", MB(1231))

    MEM_SIZE_ERR("5K",    KB(5))
    MEM_SIZE_ERR("2000K", KB(2000))
    MEM_SIZE_ERR("10K",   KB(10))

    MEM_SIZE_ERR("12B", 12)
    MEM_SIZE_ERR("32B", 32)
    MEM_SIZE_ERR("53B", 53)
    MEM_SIZE_ERR("12",  12)
    MEM_SIZE_ERR("32",  32)
    MEM_SIZE_ERR("53",  53)

    MEM_SIZE_ERR("28G", GB(28))
    MEM_SIZE_ERR("14G", GB(14))
    MEM_SIZE_ERR("63G", GB(63))

#   undef MEM_SIZE_ERR

    read_mem_error: ERROR("T_MemUtils: read mem size error");
}

int main() {
    M_Init();

    T_BadAllocSize();
    T_MemUtils();
    
    M_Free();

    return 0;
} 