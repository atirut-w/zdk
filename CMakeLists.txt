cmake_minimum_required(VERSION 3.22)
project(ZDK)
add_compile_options(-pedantic -pedantic-errors -Wall -Wextra)
include(CTest)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(cc)
add_subdirectory(cc1)
