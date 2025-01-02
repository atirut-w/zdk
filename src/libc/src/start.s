    .global exit
    .global __call_ptr

    ; void __libc_start_main(int (*main)(int, char**, char**), int argc, char **ubp_av, void (*init)(void), void (*fini)(void), void (*rtld_fini)(void), void *stack_end)
    ; Argument locations:
    ;   IX+2: main
    ;   IX+4: argc
    ;   IX+6: ubp_av
    ;   IX+8: init
    ;   IX+10: fini
    ;   IX+12: rtld_fini
    ;   IX+14: stack_end
    .global __libc_start_main
__libc_start_main:
    ld ix, 0
    add ix, sp

    ; TODO: atexit(fini)

    ; Call init
    ld l, (ix+8)
    ld h, (ix+9)
    push hl
    call __call_ptr

    ; Call main
    ld hl, 0
    ex (sp), hl ; No support for environment variables (yet)
    ld l, (ix+6)
    ld h, (ix+7)
    push hl
    ld l, (ix+4)
    ld h, (ix+5)
    push hl
    ld l, (ix+2)
    ld h, (ix+3)
    push hl
    call __call_ptr
    pop bc
    pop bc

    ; TODO: Again, atexit(fini)
    push hl
    ld l, (ix+10)
    ld h, (ix+11)
    push hl
    call __call_ptr
    pop hl
    pop hl

    ; Call exit with return value from main
    ex (sp), hl
    call exit
