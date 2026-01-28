	.section .text,"ax",@progbits
	.globl _start
	.type _start, @function
_start:
	push ix
	ld ix, 0
	add ix, sp
	ld hl, 14
	push hl
	ld hl, .LC1
	push hl
	ld a, 0x00
	ld l, a
	push hl
	call _write
	pop bc
	pop bc
	pop bc
	ld hl, 0
	push hl
	call _exit
	pop bc
.L1:
	ld sp, ix
	pop ix
	ret
	.size _start, .- _start

	.section .rodata,"a",@progbits
.LC1:
	.byte 0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x2c, 0x20, 0x57, 0x6f, 0x72, 0x6c, 0x64, 0x21, 0x0a, 0x00

	.section .data,"aw",@progbits

	.section .bss,"aw",@nobits
