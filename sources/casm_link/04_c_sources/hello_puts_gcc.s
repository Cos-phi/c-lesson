	.cpu arm7tdmi
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 1
	.eabi_attribute 30, 6
	.eabi_attribute 34, 0
	.eabi_attribute 18, 4
	.file	"hello_puts.c"
	.text
	.align	2
	.global	print_something
	.arch armv4t
	.syntax unified
	.arm
	.fpu softvfp
	.type	print_something, %function
print_something:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	str	lr, [sp, #-4]!
	sub	sp, sp, #12
	str	r0, [sp, #4]
	ldr	r0, [sp, #4]
	bl	puts
	nop
	add	sp, sp, #12
	@ sp needed
	ldr	lr, [sp], #4
	bx	lr
	.size	print_something, .-print_something
	.section	.rodata
	.align	2
.LC0:
	.ascii	"Hello World\000"
	.text
	.align	2
	.global	main
	.syntax unified
	.arm
	.fpu softvfp
	.type	main, %function
main:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, lr}
	ldr	r0, .L4
	bl	print_something
	mov	r3, #0
	mov	r0, r3
	pop	{r4, lr}
	bx	lr
.L5:
	.align	2
.L4:
	.word	.LC0
	.size	main, .-main
	.ident	"GCC: (15:8-2019-q3-1+b1) 8.3.1 20190703 (release) [gcc-8-branch revision 273027]"
