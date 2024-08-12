project(tests)

include_directories(./../src)

# tests
add_executable(t_mem ./t_mem.c)
add_executable(t_sys ./t_sys.c)

target_sources(t_mem PRIVATE
    ./../src/core/mem/m_alloc.c
    ./../src/core/mem/m_utils.c
    ./../src/core/sys.c
)

target_sources(t_sys PRIVATE
    ./../src/core/sys.c
)

add_test(NAME MemoryManagerTest COMMAND t_mem)
add_test(NAME SysTest           COMMAND t_sys)