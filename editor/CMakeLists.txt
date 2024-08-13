project(editor)

# me_main executable
add_executable(me_main ./me_main.c)

target_link_libraries(me_main ${SDL2_LIBRARIES})

target_sources(me_main PRIVATE
    
)

if(WIN32)
    target_link_libraries(me_main
        winmm.lib
        imm32.lib
        ole32.lib
        Setupapi.lib
        gdi32.lib
        version.lib
    )
endif()

if(UNIX)
    target_link_libraries(me_main m)
endif()