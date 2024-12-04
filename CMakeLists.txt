cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(TinyGL)

# Add SDL2Config.cmake to the package search path
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_LIST_DIR}/SDL2/lib/cmake)

add_subdirectory(src)
add_subdirectory(examples)

