cmake_minimum_required(VERSION 3.0)
project(libc ASM)

add_library(c STATIC
    src/stubs.s
    src/start.s
    src/stdlib.s
)
install(TARGETS c DESTINATION /usr/z80-elf/lib)
