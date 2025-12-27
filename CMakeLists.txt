cmake_minimum_required(VERSION 3.22)
project(ZDK)
add_compile_options(-pedantic -pedantic-errors -Wall -Wextra)
include(CTest)

# Option to select target backend
set(ZDK_TARGET "amd64" CACHE STRING "Target architecture (amd64 or z80)")
set_property(CACHE ZDK_TARGET PROPERTY STRINGS amd64 z80)

# Put all executables in a single bin directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(cc)
add_subdirectory(cc1)
add_subdirectory(tests)
