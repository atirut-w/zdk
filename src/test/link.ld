SEARCH_DIR(/usr/z80-elf/lib)

MEMORY
{
    TPA (rwx) : ORIGIN = 0x0100, LENGTH = 0x8000
}

SECTIONS
{
    .text : {
        *(.startup)
        *(.text)
    } > TPA
}
