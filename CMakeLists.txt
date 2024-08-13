cmake_minimum_required(VERSION 3.5)
project(HaltyraEngine C ASM)

add_compile_options(-g)
add_compile_options(-fPIE)
add_compile_options(-Wall)
add_compile_options(-Wextra)

enable_testing()

if (WIN32)
    set(SDL2TTF_DIR ./SDL2_ttf/)
endif()

find_package(SDL2 REQUIRED)
find_package(SDL2_TTF REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${SDL_TTF_INCLUDE_DIRS})
include_directories(./src)

add_subdirectory(src    ./src)
add_subdirectory(tests  ./tests)
add_subdirectory(editor ./editor)
