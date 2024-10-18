    .equ BDOS, 5

    .section .startup
    .global _start
_start:
    call _main
    ld c, 0
    call BDOS
