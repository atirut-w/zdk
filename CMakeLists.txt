cmake_minimum_required(VERSION 3.22)
project(ZDK)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-pedantic -pedantic-errors -Wall -Wextra)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(ZDK_TARGET_TRIPLET "z80-unknown-none-elf" CACHE STRING "ZDK target triplet")
set(ZDK_TOOLCHAIN_NAME "cc" CACHE STRING "ZDK toolchain name (matches driver exec name by default)")

add_subdirectory(cc1)
add_subdirectory(cc)

install(CODE [[
  file(MAKE_DIRECTORY "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/${ZDK_TARGET_TRIPLET}/${ZDK_TOOLCHAIN_NAME}/sysroot/usr/include")
  file(MAKE_DIRECTORY "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/${ZDK_TARGET_TRIPLET}/${ZDK_TOOLCHAIN_NAME}/sysroot/usr/lib")
]])
