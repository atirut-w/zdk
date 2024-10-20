    .equ BDOS, 5

    .global getchar
    .type getchar, @function
getchar:
    push ix

    ld c, 1
    call BDOS
    ld l, a
    ld h, 0

    pop ix
    ret

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
