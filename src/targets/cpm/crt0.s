    .equ BDOS, 5
    .global _main

    .global _start
    .type _start, @function
_start:
    call _main
    ld c, 0
    call BDOS
