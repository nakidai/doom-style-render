project(src)

# cl_main executable
add_executable(cl_main ./cl_main.c)

target_link_libraries(cl_main ${SDL2_LIBRARIES})

target_sources(cl_main PRIVATE
    ./render/r_render.c
    ./render/utils/r_texture.c
    ./game/g_player.c
    ./game/g_map.c
    ./core/v_vid.c
    ./core/file/f_wad.c
    ./core/file/f_conf.c
    ./lib/cbmp/cbmp.c
    ./core/mem/m_alloc.c
    ./core/mem/m_utils.c
    ./core/sys.c
    ./utils/math/u_math.c
    ./game/g_phys.c
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
if ((CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)") AND WIN32)
    target_sources(cl_main PRIVATE ./render/draw/asm/x86/win/win_x86_64.s)
    # target_sources(cl_main PRIVATE ./utils/math/asm/x86/win/win_x86_64.s)
    target_sources(cl_main PRIVATE ./utils/math/asm/c_impl.c) # TODO: fix asebmler implementation
else()
    target_sources(cl_main PRIVATE ./render/draw/r_draw.c)
    target_sources(cl_main PRIVATE ./utils/math/asm/c_impl.c)
endif()