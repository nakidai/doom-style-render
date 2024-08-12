project(tests)

include_directories(./../src)

# tests
add_executable(t_mem ./t_mem.c)

target_sources(t_mem PRIVATE
    ./../src/core/mem/m_alloc.c
    ./../src/core/sys.c
)

add_test(NAME MemoryManagerTest COMMAND t_mem)