OUTPUT_FORMAT("binary")
ENTRY(_start)

MEMORY
{
    TPA (rw) : ORIGIN = 0x0100, LENGTH = 0xf000
}

SECTIONS
{
    .text : {
        _start = .;
        *(.text)
        *(.rodata)
    } > TPA

    .data : {
        *(.data)
    } > TPA

    .bss : {
        __bss_start = .;
        *(.bss)
        __bss_end = .;
    } > TPA
}
