cmake_minimum_required(VERSION 2.8)
project(HaltyraEngine)

add_compile_options(-g)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

enable_language(C ASM)

set(SOURCE ./src/render/r_render.c
           ./src/render/utils/r_texture.c
           ./src/render/draw/asm/r_draw.s
           ./src/render/draw/r_draw.c
           ./src/game/g_player.c
           ./src/game/g_map.c
           ./src/core/v_vid.c
           ./src/core/file/f_wad.c
           ./src/core/file/f_conf.c
           ./src/lib/cbmp/cbmp.c
           ./src/core/mem/m_alloc.c
           ./src/core/sys.c)

set(SYSTEM_LIBS winmm.lib
                imm32.lib
                ole32.lib
                Setupapi.lib
                gdi32.lib
                version.lib)

set(SOURCE_MAIN ./src/cl_main.c ${SOURCE})

add_executable(cl_main ${SOURCE_MAIN})

target_link_libraries(cl_main
                      ${SDL2_LIBRARIES}
                      ${SYSTEM_LIBS})