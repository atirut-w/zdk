	; Generated by ZDK C Compiler

	.global main
	.type main, @function
main:
	push iy
	push ix
	ld ix, 0
	add ix, sp
	ld iy, -1
	add iy, sp
	ld sp, iy
	ld sp, ix
	pop ix
	pop iy
	ret