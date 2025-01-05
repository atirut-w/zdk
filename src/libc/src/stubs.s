.macro STUB name
    .weak \name
    .type \name, @function
\name:
    ld hl, -1
    ret
.endm

STUB putchar
