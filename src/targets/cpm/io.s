    .equ BDOS, 5
    
    .global _puts
    .type _puts, @function
_puts:
    push ix
    ld ix, 0
    add ix, sp
    ld l, (ix+4)
    ld h, (ix+5)
0:
    ld a, (hl)
    or a
    jr z, 1f
    ld c, 2
    ld e, a

    push hl
    call BDOS
    pop hl

    inc hl
    jr 0b
1:
    pop ix
    ret
