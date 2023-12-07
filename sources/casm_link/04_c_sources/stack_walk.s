	.text
	.syntax unified
	.eabi_attribute	67, "2.09"	@ Tag_conformance
	.eabi_attribute	6, 1	@ Tag_CPU_arch
	.eabi_attribute	8, 1	@ Tag_ARM_ISA_use
	.eabi_attribute	34, 1	@ Tag_CPU_unaligned_access
	.eabi_attribute	17, 1	@ Tag_ABI_PCS_GOT_use
	.eabi_attribute	20, 1	@ Tag_ABI_FP_denormal
	.eabi_attribute	21, 0	@ Tag_ABI_FP_exceptions
	.eabi_attribute	23, 3	@ Tag_ABI_FP_number_model
	.eabi_attribute	24, 1	@ Tag_ABI_align_needed
	.eabi_attribute	25, 1	@ Tag_ABI_align_preserved
	.eabi_attribute	38, 1	@ Tag_ABI_FP_16bit_format
	.eabi_attribute	18, 4	@ Tag_ABI_PCS_wchar_t
	.eabi_attribute	14, 0	@ Tag_ABI_PCS_R9_use
	.file	"stack_walk.c"
	.globl	print_address                   @ -- Begin function print_address
	.p2align	2
	.type	print_address,%function
	.code	32                              @ @print_address
print_address:
	.fnstart
@ %bb.0:
	push	{r11, lr}
	mov	r11, sp
	sub	sp, sp, #8
	str	r0, [sp, #4]
	ldr	r1, [sp, #4]
	ldr	r0, .LCPI0_0
	bl	printf
	mov	sp, r11
	pop	{r11, lr}
	mov	pc, lr
	.p2align	2
@ %bb.1:
.LCPI0_0:
	.long	.L.str
.Lfunc_end0:
	.size	print_address, .Lfunc_end0-print_address
	.cantunwind
	.fnend
                                        @ -- End function
	.globl	print_one                       @ -- Begin function print_one
	.p2align	2
	.type	print_one,%function
	.code	32                              @ @print_one
print_one:
	.fnstart
@ %bb.0:
	push	{r11, lr}
	mov	r11, sp
	sub	sp, sp, #8
	str	r0, [sp, #4]
	ldr	r1, [sp, #4]
	ldr	r0, .LCPI1_0
	bl	printf
	mov	sp, r11
	pop	{r11, lr}
	mov	pc, lr
	.p2align	2
@ %bb.1:
.LCPI1_0:
	.long	.L.str.1
.Lfunc_end1:
	.size	print_one, .Lfunc_end1-print_one
	.cantunwind
	.fnend
                                        @ -- End function
	.globl	func3                           @ -- Begin function func3
	.p2align	2
	.type	func3,%function
	.code	32                              @ @func3
func3:
	.fnstart
@ %bb.0:
	push	{r11, lr}
	mov	r11, sp
	sub	sp, sp, #8
	str	r0, [sp, #4]
	ldr	r0, [sp, #8]
	ldr	r0, [r0, #-8]
	bl	print_one
	ldr	r0, [sp, #4]
	add	r0, r0, r0, lsl #1
	mov	sp, r11
	pop	{r11, lr}
	mov	pc, lr
.Lfunc_end2:
	.size	func3, .Lfunc_end2-func3
	.cantunwind
	.fnend
                                        @ -- End function
	.globl	func2                           @ -- Begin function func2
	.p2align	2
	.type	func2,%function
	.code	32                              @ @func2
func2:
	.fnstart
@ %bb.0:
	push	{r11, lr}
	mov	r11, sp
	sub	sp, sp, #16
	str	r0, [r11, #-4]
	mov	r0, #0
	str	r0, [sp, #8]
	str	r0, [sp, #4]
	b	.LBB3_1
.LBB3_1:                                @ =>This Inner Loop Header: Depth=1
	ldr	r0, [sp, #4]
	cmp	r0, #9
	bgt	.LBB3_4
	b	.LBB3_2
.LBB3_2:                                @   in Loop: Header=BB3_1 Depth=1
	ldr	r0, [sp, #4]
	ldr	r1, [sp, #8]
	add	r0, r1, r0
	str	r0, [sp, #8]
	b	.LBB3_3
.LBB3_3:                                @   in Loop: Header=BB3_1 Depth=1
	ldr	r0, [sp, #4]
	add	r0, r0, #1
	str	r0, [sp, #4]
	b	.LBB3_1
.LBB3_4:
	ldr	r0, [r11, #-4]
	ldr	r1, [sp, #8]
	add	r0, r0, r1
	bl	func3
	add	r0, r0, #2
	mov	sp, r11
	pop	{r11, lr}
	mov	pc, lr
.Lfunc_end3:
	.size	func2, .Lfunc_end3-func2
	.cantunwind
	.fnend
                                        @ -- End function
	.globl	func1                           @ -- Begin function func1
	.p2align	2
	.type	func1,%function
	.code	32                              @ @func1
func1:
	.fnstart
@ %bb.0:
	push	{r11, lr}
	mov	r11, sp
	sub	sp, sp, #8
	str	r0, [sp, #4]
	ldr	r0, [sp, #4]
	bl	func2
	add	r0, r0, #1
	mov	sp, r11
	pop	{r11, lr}
	mov	pc, lr
.Lfunc_end4:
	.size	func1, .Lfunc_end4-func1
	.cantunwind
	.fnend
                                        @ -- End function
	.globl	main                            @ -- Begin function main
	.p2align	2
	.type	main,%function
	.code	32                              @ @main
main:
	.fnstart
@ %bb.0:
	push	{r11, lr}
	mov	r11, sp
	sub	sp, sp, #8
	mov	r0, #0
	str	r0, [sp, #4]
	mov	r0, #123
	bl	func1
	str	r0, [sp]
	ldr	r1, [sp]
	ldr	r0, .LCPI5_0
	bl	printf
	mov	r0, #0
	mov	sp, r11
	pop	{r11, lr}
	mov	pc, lr
	.p2align	2
@ %bb.1:
.LCPI5_0:
	.long	.L.str.2
.Lfunc_end5:
	.size	main, .Lfunc_end5-main
	.cantunwind
	.fnend
                                        @ -- End function
	.type	.L.str,%object                  @ @.str
	.section	.rodata.str1.1,"aMS",%progbits,1
.L.str:
	.asciz	"address: 0x%x\n"
	.size	.L.str, 15

	.type	.L.str.1,%object                @ @.str.1
.L.str.1:
	.asciz	"value: %d\n"
	.size	.L.str.1, 11

	.type	.L.str.2,%object                @ @.str.2
.L.str.2:
	.asciz	"result is %d\n"
	.size	.L.str.2, 14

	.ident	"Debian clang version 11.0.1-2"
	.section	".note.GNU-stack","",%progbits
