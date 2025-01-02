    .global __call_ptr
__call_ptr:
    push ix
    ld ix, 0
    add ix, sp

    ld l, (ix+4)
    ld h, (ix+5)

    pop ix
    jp (hl)
