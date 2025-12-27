    .globl bdos
bdos:
    push ix
    ld ix, 0
    add ix, sp

    ld c, (ix+4)
    ld e, (ix+6)
    ld d, (ix+7)
    call 5

    ; This is only for compat with CP/M 1.x BDOS
    ld l, a
    ld h, b

    pop ix
    ret

    .section .text.header
    .globl start
header:
    jp start
