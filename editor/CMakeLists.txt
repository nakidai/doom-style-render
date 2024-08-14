project(editor)

# me_main executable
add_executable(me_main ./me_main.c)

target_link_libraries(me_main ${SDL2_LIBRARIES} ${SDL_TTF_LIBRARIES})

target_sources(me_main PRIVATE
    ./../src/core/mem/m_alloc.c
    ./../src/core/mem/m_utils.c
    ./../src/core/sys.c
    ./../src/core/file/f_wad.c
    ./../src/core/file/f_conf.c
    ./../src/game/g_map.c
    ./../src/cmd/cmd_exec.c
)

if(WIN32)
    target_link_libraries(me_main
        winmm.lib
        imm32.lib
        ole32.lib
        Setupapi.lib
        gdi32.lib
        version.lib
        Rpcrt4.lib
    )
endif()

if(UNIX)
    target_link_libraries(me_main m SDL2_ttf)
endif()
