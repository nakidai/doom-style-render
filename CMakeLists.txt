cmake_minimum_required(VERSION 2.8)
project(HaltyraEngine)

add_compile_options(-g)
add_compile_options(-Wall)
add_compile_options(-Wextra)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# cl_main executable
add_executable(cl_main ./src/cl_main.c)

target_link_libraries(cl_main ${SDL2_LIBRARIES})

target_sources(cl_main PRIVATE
    ./src/render/r_render.c
    ./src/render/utils/r_texture.c
    ./src/game/g_player.c
    ./src/game/g_map.c
    ./src/core/v_vid.c
    ./src/core/file/f_wad.c
    ./src/core/file/f_conf.c
    ./src/lib/cbmp/cbmp.c
    ./src/core/mem/m_alloc.c
    ./src/core/sys.c
)

if(WIN32)
    target_link_libraries(cl_main
        winmm.lib
        imm32.lib
        ole32.lib
        Setupapi.lib
        gdi32.lib
        version.lib
    )
endif()

if(UNIX)
    target_link_libraries(cl_main m)
endif()

# Assembly code only for x86-64
if (CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)")
    target_sources(cl_main PRIVATE ./src/render/draw/asm/x86_r_draw.s)
else()
    target_sources(cl_main PRIVATE ./src/render/draw/r_draw.c)
endif()

# tests
add_executable(t_mem ./src/core/mem/t_mem.c)

target_sources(t_mem PRIVATE
    ./src/core/mem/m_alloc.c
    ./src/core/sys.c
)

add_test(NAME MemoryManagerTest COMMAND t_mem)cmake_minimum_required(VERSION 2.8)
project(HaltyraEngine)

add_compile_options(-g)
add_compile_options(-Wall)
add_compile_options(-Wextra)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

enable_language(ASM)

# cl_main executable
add_executable(cl_main ./src/cl_main.c)

target_link_libraries(cl_main ${SDL2_LIBRARIES})

target_sources(cl_main PRIVATE
    ./src/render/r_render.c
    ./src/render/utils/r_texture.c
    ./src/game/g_player.c
    ./src/game/g_map.c
    ./src/core/v_vid.c
    ./src/core/file/f_wad.c
    ./src/core/file/f_conf.c
    ./src/lib/cbmp/cbmp.c
    ./src/core/mem/m_alloc.c
    ./src/core/sys.c
)

if(WIN32)
    target_link_libraries(cl_main
        winmm.lib
        imm32.lib
        ole32.lib
        Setupapi.lib
        gdi32.lib
        version.lib
    )
endif()

if(UNIX)
    target_link_libraries(cl_main m)
endif()

# Assembly code only for x86-64
if (CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)")
    target_sources(cl_main PRIVATE ./src/render/draw/asm/x86_r_draw.s)
else()
    target_sources(cl_main PRIVATE ./src/render/draw/r_draw.c)
endif()

# tests
add_executable(t_mem ./src/core/mem/t_mem.c)

target_sources(t_mem PRIVATE
    ./src/core/mem/m_alloc.c
    ./src/core/sys.c
)

add_test(NAME MemoryManagerTest COMMAND t_mem)