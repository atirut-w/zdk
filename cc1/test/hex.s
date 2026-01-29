    ; Simple numeric output helpers for Zeal tests
    ;
    ; C ABI (current):
    ;   - args are 16-bit stack slots at (ix+4..)
    ;   - unsigned char returns in A, int/pointer in HL
    ;
    ; Exports:
    ;   void puthex8(unsigned char v);   ; prints 2 hex digits
    ;   void puthex16(unsigned short v); ; prints 4 hex digits
    ;   void putnl(void);                ; prints '\n'

    .globl _puthex8
_puthex8:
    push ix
    ld ix, 0
    add ix, sp

    ld a, (ix+4)
    ld hl, _hexbuf8
    call _hex_byte_to_buf

    ld hl, 2
    push hl
    ld hl, _hexbuf8
    push hl
    ld hl, 0
    push hl
    call _write
    pop bc
    pop bc
    pop bc

    ld sp, ix
    pop ix
    ret

    .globl _puthex16
_puthex16:
    push ix
    ld ix, 0
    add ix, sp

    ; value in HL from arg slot
    ld l, (ix+4)
    ld h, (ix+5)

    ; write high byte then low byte
    ld a, h
    ld de, _hexbuf16
    push hl
    ld h, d
    ld l, e
    call _hex_byte_to_buf
    pop hl

    ld a, l
    ld hl, _hexbuf16+2
    call _hex_byte_to_buf

    ld hl, 4
    push hl
    ld hl, _hexbuf16
    push hl
    ld hl, 0
    push hl
    call _write
    pop bc
    pop bc
    pop bc

    ld sp, ix
    pop ix
    ret

    .globl _putnl
_putnl:
    push ix
    ld ix, 0
    add ix, sp

    ld hl, 1
    push hl
    ld hl, _nl
    push hl
    ld hl, 0
    push hl
    call _write
    pop bc
    pop bc
    pop bc

    ld sp, ix
    pop ix
    ret

    ; --- helpers ---
    ; A = byte, HL = dest buffer (2 bytes)
_hex_byte_to_buf:
    push bc
    push de

    ld b, a
    ; high nibble
    ld a, b
    rrca
    rrca
    rrca
    rrca
    and 0x0f
    call _hex_nibble_to_char
    ld (hl), a
    inc hl

    ; low nibble
    ld a, b
    and 0x0f
    call _hex_nibble_to_char
    ld (hl), a

    pop de
    pop bc
    ret

    ; A = 0..15, returns ASCII in A
_hex_nibble_to_char:
    push hl
    ld hl, _hexdigits
    ld e, a
    ld d, 0
    add hl, de
    ld a, (hl)
    pop hl
    ret

    .data
_hexdigits:
    .ascii "0123456789ABCDEF"

    .bss
_hexbuf8:
    .space 2
_hexbuf16:
    .space 4

    .data
_nl:
    .byte 0x0a
