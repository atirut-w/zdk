ENTRY(_start)

MEMORY
{
    TPA (rw) : ORIGIN = 0x0100, LENGTH = 0xf000
}

SECTIONS
{
    .text : {
        *(.startup)
        *(.text)
        *(_CODE)
    } > TPA
}
