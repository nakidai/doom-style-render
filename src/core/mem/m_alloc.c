// Copyright (C) by gimura 2024
// --- PREFIX ---
//
// M_* - memory functions
//
// --- DESCRIPTION ---
//
// working with memory
//
// -------------------

#include "../../cl_def.h"

// memory block header
typedef struct mem_block_s {
    int sig;    // if valid block MEM_BLOCK_SIG (0x4CE0CA70)
                // if the block was released by M_Collect, OLD_BLOCK_SIG (0xDEADB104)
    int tag;    // 0 - free block, 1 - occupied block
    usize size; // block size in bytes
    struct mem_block_s *next, *prev;
} mem_block_t;

#define BLOCK_SIZE sizeof(mem_block_t)

#define FREE_BLOCK_TAG 0
#define OCCUPIED_BLOCK_TAG 1

#ifdef DEV_BUILD
#define M_ERROR(...) ({ M_Debug(); ERROR(__VA_ARGS__); })
#else
#define M_ERROR(...) ERROR(__VA_ARGS__)
#endif

static void  M_InitBlockMem(void* buf, usize size);
static usize M_ReadMemSize(const char* size_str);

static void* glob_mem;
static void* temp_mem;

static usize glob_size;
static usize temp_size;

// memory initialization
void M_Init(void) {
#   ifndef STD_ALLOC
    const char* glob_size_str = SYS_GetArg("-globHeap");
    const char* temp_size_str = SYS_GetArg("-tempHeap");

    if (glob_size_str == NULL) glob_size = GLOB_HEAP;
    else                       glob_size = M_ReadMemSize(glob_size_str);

    if (temp_size_str == NULL) temp_size = TEMP_HEAP;
    else                       temp_size = M_ReadMemSize(temp_size_str);

    // memory allocation
    glob_mem = malloc(glob_size);
    temp_mem = malloc(temp_size);

    // clearing memory buffers
    memset(glob_mem, 0, glob_size);
    memset(temp_mem, 0, temp_size);

    // create first block of memory
    M_InitBlockMem(glob_mem, glob_size);
    M_InitBlockMem(temp_mem, temp_size);
#   endif
}

// freeing up memory
void M_Free(void) {
#   ifndef STD_ALLOC
    free(glob_mem);
    free(temp_mem);
#   endif
}

static void M_DebugBuffer(void* buf, usize size);

// output memory blocks to console
void M_Debug(void) {
#   ifndef STD_ALLOC
    printf("M_Debug: memory stats:\n");
    printf("--- glob_mem ---\n");
    M_DebugBuffer(glob_mem, glob_size);
    printf("--- temp_mem ---\n");
    M_DebugBuffer(temp_mem, temp_size);
    printf("--- end ---\n");
#   endif
}

static void  M_Check(void* buf);
static void* M_BufAlloc(void* buf, usize size, usize buf_size);

// allocate memory to global buffer
void* M_GlobAlloc(usize size) {
#   ifndef SDL_ALLOC
    void* addr = M_BufAlloc(glob_mem, size, glob_size); // allocate memory
#   ifdef PARANOID
    M_Check(glob_mem); // check blocks signature
#   endif

    return addr;
#   else
    return malloc(size);
#   endif
}

// allocate memory to temp buffer
void* M_TempAlloc(usize size) {
#   ifndef STD_ALLOC
    void* addr = M_BufAlloc(temp_mem, size, temp_size); // allocate memory
#   ifdef PARANOID
    M_Check(temp_mem); // check blocks signature
#   endif

    return addr;
#   else
    return malloc(size);
#   endif
}

static void M_BufFree(void* ptr);
static void M_Collect(void* buf);

static usize glob_free = 0;
static usize temp_free = 0;

// freeing memory in the global buffer
void M_GlobFree(void* ptr) {
#   ifndef STD_ALLOC
    if (++glob_free >= GLOB_COLLECT_RATE) { M_GlobCollect(); glob_free = 0; } // clearing blocks
    M_BufFree(ptr); // freeing memory

#   ifdef PARANOID
    M_Check(glob_mem); // check blocks signature
#   endif
#   else
    free(ptr);
#   endif
}

// freeing memory in the temp buffer
void M_TempFree(void* ptr) {
#   ifndef STD_ALLOC
    if (++temp_free >= TEMP_COLLECT_RATE) { M_TempCollect(); temp_free = 0; } // clearing blocks
    M_BufFree(ptr); // freeing memory

#   ifdef PARANOID
    M_Check(temp_mem); // check blocks signature
#   endif
#   else
    free(ptr);
#   endif
}

// garbage collection in the global buffer
void M_GlobCollect(void) {
#   ifndef STD_ALLOC
    M_Collect(glob_mem);

#   ifdef PARANOID
    M_Check(glob_mem);
#   endif
#   endif
}

// garbage collection in the temp buffer
void M_TempCollect(void) {
#   ifndef STD_ALLOC
    M_Collect(temp_mem);

#   ifdef PARANOID
    M_Check(temp_mem);
#   endif    
#   endif
}

// reading size from string
// returns the size in bytes
static usize M_ReadMemSize(const char* size_str) {
    char  type;
    usize size;

    usize return_size = -1;

    if (sscanf(
        size_str,
        "%u%c",
        size, type
    ) != 2) goto fail;

    switch (type) {
        case 'B': // bytes
            return_size = size;
            goto done;

        case 'K': // kilobytes
            return_size = KB(size);
            goto done;

        case 'M': // megabytes
            return_size = MB(size);
            goto done;

        case 'G': // gigabytes
            return_size = GB(size);
            goto done;

        default:
#           ifdef PARANOID
            printf("M_ReadMemSize: w-why d-didn't you s-specify the f-format o_0 (perceived as bytes)\n");
#           endif

            goto done;
    }

    done:
        if (return_size == -1) goto fail;
        return return_size;

    fail: ERROR("M_ReadMemSize: invalid memory format");
}

static void M_BufFree(void* ptr) {
    mem_block_t* block = (mem_block_t*) ((usize) ptr - BLOCK_SIZE); // reading memory block

    if (block == NULL)               M_ERROR("M_BufFree: can't free NULL");
    if (block->sig != MEM_BLOCK_SIG) M_ERROR("M_BufFree: invalid block signature %x", block->sig);

    block->tag = FREE_BLOCK_TAG; // mark the block as empty
}

static void M_DebugBuffer(void* buf, usize size) {
    mem_block_t blocks[1024];
    void* offset = buf;
    usize id = 0;

    while (offset - (usize) buf < size) {
        mem_block_t* block = (mem_block_t*) offset;
        blocks[id++] = *block;
        if (block->next == NULL) break;
        offset = block->next;
    }

    printf(" block count: %i\n", id);

    printf(" -- blocks --\n");
    for (int i = 0; i < id; i++) {
        const mem_block_t block = blocks[i];

        printf("  - block %i -\n", i);
        printf("   tag: %i\n", block.tag);
        printf("   size: %x\n", block.size);
        printf("   sig: %x\n", block.sig);
        printf("   prev: %x\n", block.prev);
        printf("   next: %x\n", block.next);
    }
}

// creating a memory block header at the beginning of the buffer
static void M_InitBlockMem(void* buf, usize size) {
    mem_block_t* block = (mem_block_t*) buf;

    block->sig = MEM_BLOCK_SIG;
    block->tag = FREE_BLOCK_TAG;
    block->size = size;
    block->next = NULL;
    block->prev = NULL;
}

// memory allocation in buffer
static void* M_BufAlloc(void* buf, usize size, usize buf_size) {
    void* addr = NULL; // return address
    void* offset = buf;

    while (offset - (usize) buf < buf_size) {
        mem_block_t* block = (mem_block_t*) offset; // getting block by address

        if (block->tag != FREE_BLOCK_TAG) goto next; // if the block is not empty we move on to the next one

        if (block->size < size) goto next; // if the block size is less than the allocated memory, we move on to the next one
        if (block->size == size) {
            block->tag = OCCUPIED_BLOCK_TAG; // set block tag to busy

            addr = (void*) offset + BLOCK_SIZE; // getting address after block
            goto done;
        } else {
            mem_block_t* next = (mem_block_t*) offset + BLOCK_SIZE + size; // create a block after the current one, which will contain empty memory of the current one

            // decreasing the memory of the current block and initializing a new one
            next->size = block->size - size;
            next->next = block->next;
            next->prev = block;

            block->size = size;
            block->tag = OCCUPIED_BLOCK_TAG;
            block->next = next;

            next->sig = MEM_BLOCK_SIG;
            next->tag = FREE_BLOCK_TAG;

            addr = offset + BLOCK_SIZE; // getting address after block
            goto done;
        }

        next: // move to next block
            if (block->next == NULL) goto done;
            offset = block->next;
    }

    done:
        if (addr == NULL) M_ERROR("M_BufAlloc: can't alloc"); // if addr is NULL then an error occurred
        return addr;
}

// collecting garbage blocks into one
static void M_Collect(void* buf) {
    mem_block_t* block = (mem_block_t*) buf; // getting the first block

    while (block->next != NULL) {
        if (block->next == NULL) continue;
        if (block->tag == FREE_BLOCK_TAG && block->next->tag == FREE_BLOCK_TAG) { // if current and next are empty
            block->next->sig = OLD_BLOCK_SIG; // setting the signature of the next block on OLD_BLOCK_SIG (0xDEADB104)
            block->size += block->next->size;
            block->next = block->next->next;
            if (block->next != NULL) block->next->prev = block;

            continue; // leave the current block to collect the entire sequence of blocks
        }

        block = block->next;
    }
}

// block signature verification
static void M_Check(void* buf) {
    for (mem_block_t* block = (mem_block_t*) buf; block->next != NULL; block = block->next) {
        if (block->sig != MEM_BLOCK_SIG) M_ERROR("M_Check: invalid memory block signature %X", block->sig);
    }
}