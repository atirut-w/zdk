    .global exit
    .global __call_ptr

    ; void __libc_start_main(int (*main)(int, char**, char**), int argc, char **ubp_av, void (*init)(void), void (*fini)(void), void (*rtld_fini)(void), void *stack_end)
    ; Argument locations:
    ;   IX+4: main
    ;   IX+6: argc
    ;   IX+8: ubp_av
    ;   IX+10: init
    ;   IX+12: fini
    ;   IX+14: rtld_fini
    ;   IX+16: stack_end
    ; Variable locations:
    ;   IX-2: return value from main
    .global __libc_start_main
__libc_start_main:
    push ix
    ld ix, 0
    add ix, sp

    ; TODO: atexit(fini) & atexit(rtld_fini)

    ; Call init
    ld l, (ix+10)
    ld h, (ix+11)
    push hl
    ld a, l
    or h
    call nz, __call_ptr

    ; Call main and save return value
    ld hl, 0
    ex (sp), hl ; No support for environment variables (yet)
    ld l, (ix+8)
    ld h, (ix+9)
    push hl
    ld l, (ix+6)
    ld h, (ix+7)
    push hl
    ld l, (ix+4)
    ld h, (ix+5)
    push hl
    call __call_ptr
    ld (ix-2), l
    ld (ix-1), h
    pop bc
    pop bc

    ; TODO: Again, atexit(fini)
    ld l, (ix+12)
    ld h, (ix+13)
    ld a, l
    or h
    ex (sp), hl
    call nz, __call_ptr

    ; TODO: atexit(rtld_fini)
    ld l, (ix+14)
    ld h, (ix+15)
    ld a, l
    or h
    ex (sp), hl
    call nz, __call_ptr

    ; Call exit with return value from main
    ld l, (ix-2)
    ld h, (ix-1)
    ex (sp), hl
    call exit
