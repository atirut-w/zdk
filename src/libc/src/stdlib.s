    ; int atexit(void (*func)(void))
    .global atexit
atexit:
    push ix
    ld ix, 0
    add ix, sp

    ld hl, (atexit_table_ptr)
    ld bc, atexit_table_end
    xor a
    push hl
    sbc hl, bc
    pop hl
    jr nz, 0f

    ld hl, -1 ; Failure
    jr 1f

0:
    ld a, (ix+4)
    ld (hl), a
    inc hl
    ld a, (ix+5)
    ld (hl), a
    inc hl
    ld (atexit_table_ptr), hl
    ld hl, 0 ; Success

1:
    pop ix
    ret

    ; void exit(int status)
    .global exit
exit:
    ld ix, 0
    add ix, sp

    ; Call atexit functions
0:
    ld hl, (atexit_table_ptr)
    ld bc, atexit_table
    xor a
    push hl
    sbc hl, bc
    pop hl
    jp z, 1f

    dec hl
    ld b, (hl)
    dec hl
    ld c, (hl)
    ld (atexit_table_ptr), hl
    push bc
    call __call_ptr
    pop bc
    jr 0b

1:
    ld l, (ix+2)
    ld h, (ix+3)
    push hl
    call __exit

    ; NOTE: Override this with your OS exit routine!
    ; void __exit(int status)
    .weak __exit
__exit:
    halt
    jr exit

    .section .data
atexit_table:
    .space 32 * 2
    .global atexit_table_end
atexit_table_end:
atexit_table_ptr:
    .word atexit_table
