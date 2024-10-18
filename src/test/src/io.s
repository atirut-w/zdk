    .equ BDOS, 5

    .global _putchar
    .type _putchar, @function
_putchar:
    push ix
    ld ix, 0
    add ix, sp

    ld c, 2
    ld e, (ix+4)
    call BDOS

    pop ix
    ret
