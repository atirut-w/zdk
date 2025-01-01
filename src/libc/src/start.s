    .global main
    .global exit

    ; void __libc_start_main(int argc, char *argv[])
    .global __libc_start_main
__libc_start_main:
    ld ix, 0
    add ix, sp

    ld l, (ix+4)
    ld h, (ix+5)
    push hl
    ld l, (ix+2)
    ld h, (ix+3)
    push hl
    call main
    pop bc
    ex (sp), hl
    call exit
