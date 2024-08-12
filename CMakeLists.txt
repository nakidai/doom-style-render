cmake_minimum_required(VERSION 2.8)
project(HaltyraEngine C ASM)

add_compile_options(-g)
add_compile_options(-fPIE)
add_compile_options(-Wall)
add_compile_options(-Wextra)

enable_testing()

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_subdirectory(src   ./src)
add_subdirectory(tests ./tests)