cmake_minimum_required(VERSION 3.10)
project(sdl_viewer C)

set(CMAKE_C_STANDARD 99)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(sdl_viewer sdl_viewer.c)
target_link_libraries(sdl_viewer ${SDL2_LIBRARIES})
