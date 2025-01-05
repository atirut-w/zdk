    ; int puts(const char *s)
    .global puts
puts:
    push ix
    ld ix, 0
    add ix, sp

    ld l, (ix+4)
    ld h, (ix+5)
0:
    ld a, (hl)
    or a
    jr z, 1f
    push hl
    ld l, a
    ld h, 0
    push hl
    call putchar
    pop hl
    pop hl
    inc hl
    jr 0b
1:
    pop ix
    ret
