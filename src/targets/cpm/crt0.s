    .equ BDOS, 5

    .global _start
    .type _start, @function
_start:
    ld c, 0
    call BDOS
