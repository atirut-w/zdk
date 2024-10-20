    .equ BDOS, 5

    .global putchar
    .type putchar, @function
putchar:
    push ix
    ld ix, 0
    add ix, sp

    ld c, 2
    ld e, (ix+4)
    call BDOS

    pop ix
    ret
