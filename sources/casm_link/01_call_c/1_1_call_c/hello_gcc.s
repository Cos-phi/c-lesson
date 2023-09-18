	.cpu arm7tdmi
	.arch armv4t
	.fpu softvfp
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 1
	.eabi_attribute 30, 6
	.eabi_attribute 34, 0
	.eabi_attribute 18, 4
	.file	"hello.c"
	.text
	.align	2
	.global	hello_c
	.syntax unified
	.arm
	.type	hello_c, %function
hello_c:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r3, .L3
	mov	r2, #72
	strb	r2, [r3]
	ldr	r3, .L3
	mov	r2, #101
	strb	r2, [r3]
	ldr	r3, .L3
	mov	r2, #108
	strb	r2, [r3]
	ldr	r3, .L3
	mov	r2, #108
	strb	r2, [r3]
	ldr	r3, .L3
	mov	r2, #111
	strb	r2, [r3]
	ldr	r3, .L3
	mov	r2, #32
	strb	r2, [r3]
	ldr	r3, .L3
	mov	r2, #87
	strb	r2, [r3]
	ldr	r3, .L3
	mov	r2, #111
	strb	r2, [r3]
	ldr	r3, .L3
	mov	r2, #114
	strb	r2, [r3]
	ldr	r3, .L3
	mov	r2, #108
	strb	r2, [r3]
	ldr	r3, .L3
	mov	r2, #100
	strb	r2, [r3]
	ldr	r3, .L3
	mov	r2, #33
	strb	r2, [r3]
	ldr	r3, .L3
	mov	r2, #10
	strb	r2, [r3]
	mov	r3, #1
	mov	r0, r3
	bx	lr
.L4:
	.align	2
.L3:
	.word	270471168
	.size	hello_c, .-hello_c
	.ident	"GCC: (Arm GNU Toolchain 12.2.Rel1 (Build arm-12.24)) 12.2.1 20221205"
