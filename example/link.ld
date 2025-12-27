MEMORY {
    TPA (rwx): ORIGIN = 0x0100, LENGTH = 0x4000
}

SECTIONS {
    .text : {
        *(.text.header)
        *(.text)
    } > TPA
}
