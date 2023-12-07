	.arch armv5te
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 2
	.eabi_attribute 30, 6
	.eabi_attribute 34, 0
	.eabi_attribute 18, 4
	.file	"many_symbols.c"
	.text
	.global	g_in_hello_uninit
	.bss
	.align	2
	.type	g_in_hello_uninit, %object
	.size	g_in_hello_uninit, 4
g_in_hello_uninit:
	.space	4
	.global	g_in_hello
	.data
	.align	2
	.type	g_in_hello, %object
	.size	g_in_hello, 4
g_in_hello:
	.word	1
	.align	2
	.type	g_static_in_hello, %object
	.size	g_static_in_hello, 4
g_static_in_hello:
	.word	1
	.local	g_static_uninit
	.comm	g_static_uninit,4,4
	.global	g_text
	.section	.rodata
	.align	2
.LC0:
	.ascii	"abc\000"
	.section	.data.rel.local,"aw"
	.align	2
	.type	g_text, %object
	.size	g_text, 4
g_text:
	.word	.LC0
	.global	g_text_uninit
	.bss
	.align	2
	.type	g_text_uninit, %object
	.size	g_text_uninit, 4
g_text_uninit:
	.space	4
	.global	g_text_arr
	.data
	.align	2
	.type	g_text_arr, %object
	.size	g_text_arr, 4
g_text_arr:
	.ascii	"def\000"
	.global	g_large_buf
	.bss
	.align	2
	.type	g_large_buf, %object
	.size	g_large_buf, 4194304
g_large_buf:
	.space	4194304
	.text
	.align	2
	.global	print_something
	.syntax unified
	.arm
	.fpu softvfp
	.type	print_something, %function
print_something:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	push	{r4, fp, lr}
	add	fp, sp, #8
	sub	sp, sp, #12
	str	r0, [fp, #-16]
	ldr	r4, .L3
.LPIC1:
	add	r4, pc, r4
	ldr	r0, [fp, #-16]
	bl	func_in_main(PLT)
	ldr	r3, .L3+4
.LPIC0:
	add	r3, pc, r3
	ldr	r3, [r3]
	mov	r0, r3
	bl	func_in_main(PLT)
	ldr	r3, .L3+8
	ldr	r3, [r4, r3]
	ldr	r3, [r3]
	mov	r0, r3
	sub	sp, fp, #8
	@ sp needed
	pop	{r4, fp, pc}
.L4:
	.align	2
.L3:
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC1+8)
	.word	g_text_uninit-(.LPIC0+8)
	.word	g_in_main(GOT)
	.size	print_something, .-print_something
	.ident	"GCC: (Debian 10.2.1-6) 10.2.1 20210110"
	.section	.note.GNU-stack,"",%progbits
