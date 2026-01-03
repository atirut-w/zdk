cmake_minimum_required(VERSION 3.22)
project(ZDK)
include(CTest)
add_compile_options(-std=c90 -pedantic -pedantic-errors -Wall -Wextra)
set(CMAKE_INSTALL_PREFIX /)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(as)
add_subdirectory(libzobj)
