    ; void exit(int status)
    .global exit
exit:
    ld ix, 0
    add ix, sp

    ; TODO: Perform cleanup

    ld l, (ix+2)
    ld h, (ix+3)
    push hl
    call __exit

    ; void __exit(int status)
    .weak __exit
__exit:
    halt
    jr exit
