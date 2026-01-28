    ; Put this file first in link order!
    .globl _start
    jp _start
    
    .globl _read
_read:
    push ix
    ld ix, 0
    add ix, sp

    ld h, (ix+4) ; dev
    ld e, (ix+6) ; buf
    ld d, (ix+7)
    ld c, (ix+8) ; size
    ld b, (ix+9)
    ld l, 0 ; read
    rst 0x08
    ld a, (_errno)
    ld l, c
    ld h, b

    pop ix
    ret

    .globl _write
_write:
    push ix
    ld ix, 0
    add ix, sp

    ld h, (ix+4) ; dev
    ld e, (ix+6) ; buf
    ld d, (ix+7)
    ld c, (ix+8) ; size
    ld b, (ix+9)
    ld l, 1 ; write
    rst 0x08
    ld a, (_errno)
    ld l, c
    ld h, b

    pop ix
    ret

    .globl _exit
_exit:
    push ix
    ld ix, 0
    add ix, sp

    ld h, (ix+4) ; code
    ld l, 15 ; exit
    rst 0x08 ; Does not return

    .data
    .globl _errno
_errno:
    .byte 0
