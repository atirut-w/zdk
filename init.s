    .org 0
    .type init, @function
init:
    ld hl, 0
    ld sp, hl ; Anything put on the stack should appear all the way at 0xffff

    .extern main
    call main
    halt ; Put breakpoint here to see if the function returns correctly
