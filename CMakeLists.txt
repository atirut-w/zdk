cmake_minimum_required(VERSION 3.22)
project(ZDK)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-pedantic -pedantic-errors -Wall -Wextra)
set(CMAKE_INSTALL_PREFIX /)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
