    ; Stub LibC functions, to be overridden by platform implementations

.macro STUB name
    .weak \name
    .type \name, @function
\name:
    ld hl, -1
    ret
.endm

    STUB _putchar
    STUB _puts
