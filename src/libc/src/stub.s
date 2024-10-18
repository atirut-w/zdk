.macro STUB name
    .global \name
    .type \name, @function
\name:
    ld hl, -1
    ret
.endm

    STUB _putchar
