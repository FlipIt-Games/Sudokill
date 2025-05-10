cmake_minimum_required(VERSION 3.15)
project(sudokill VERSION 0.1 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/build)

set(SDL3_include ${CMAKE_SOURCE_DIR}/deps/SDL/include)
set(SDL3_lib ${CMAKE_SOURCE_DIR}/build/SDL3.lib)
set(SDL3_shadercross_include ${CMAKE_SOURCE_DIR}/deps/SDL_shadercross/include)
set(SDL3_shadercross_lib ${CMAKE_SOURCE_DIR}/build/SDL3_shadercross.lib)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/deps/SDL_shadercross/include
)

set(SDLSHADERCROSS_VENDORED ON)

add_subdirectory(game_lib)
add_subdirectory(term_app)
add_subdirectory(desktop_app)
