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
	.file	"hello_inline.c"
	.globl	addmul3                         @ -- Begin function addmul3
	.p2align	2
	.type	addmul3,%function
	.code	32                              @ @addmul3
addmul3:
	.fnstart
@ %bb.0:
	sub	sp, sp, #8
	str	r0, [sp, #4]
	str	r1, [sp]
	ldr	r0, [sp, #4]
	ldr	r1, [sp]
	add	r0, r0, r1
	add	r0, r0, r0, lsl #1
	add	sp, sp, #8
	mov	pc, lr
.Lfunc_end0:
	.size	addmul3, .Lfunc_end0-addmul3
	.cantunwind
	.fnend
                                        @ -- End function
	.globl	addmul3_inline_notwork          @ -- Begin function addmul3_inline_notwork
	.p2align	2
	.type	addmul3_inline_notwork,%function
	.code	32                              @ @addmul3_inline_notwork
addmul3_inline_notwork:
	.fnstart
@ %bb.0:
	sub	sp, sp, #12
	str	r0, [sp, #4]
	str	r1, [sp]
	@APP
	mov	r2, r0
	@NO_APP
	@APP
	add	r2, r2, r1
	@NO_APP
	@APP
	mov	r3, #3
	@NO_APP
	@APP
	mul	r0, r2, r3
	@NO_APP
	ldr	r0, [sp, #8]
	add	sp, sp, #12
	mov	pc, lr
.Lfunc_end1:
	.size	addmul3_inline_notwork, .Lfunc_end1-addmul3_inline_notwork
	.cantunwind
	.fnend
                                        @ -- End function
	.globl	addmul3_inline                  @ -- Begin function addmul3_inline
	.p2align	2
	.type	addmul3_inline,%function
	.code	32                              @ @addmul3_inline
addmul3_inline:
	.fnstart
@ %bb.0:
	sub	sp, sp, #12
	str	r0, [sp, #8]
	str	r1, [sp, #4]
	@APP
	mov	r2, r0
	@NO_APP
	@APP
	add	r2, r2, r1
	@NO_APP
	@APP
	mov	r3, #3
	@NO_APP
	@APP
	mul	r0, r2, r3
	@NO_APP
	str	r0, [sp]
	ldr	r0, [sp]
	add	sp, sp, #12
	mov	pc, lr
.Lfunc_end2:
	.size	addmul3_inline, .Lfunc_end2-addmul3_inline
	.cantunwind
	.fnend
                                        @ -- End function
	.globl	assert_true                     @ -- Begin function assert_true
	.p2align	2
	.type	assert_true,%function
	.code	32                              @ @assert_true
assert_true:
	.fnstart
@ %bb.0:
	push	{r11, lr}
	mov	r11, sp
	sub	sp, sp, #8
	str	r0, [sp, #4]
	ldr	r0, [sp, #4]
	cmp	r0, #0
	bne	.LBB3_2
	b	.LBB3_1
.LBB3_1:
	ldr	r0, .LCPI3_0
	bl	printf
	b	.LBB3_2
.LBB3_2:
	mov	sp, r11
	pop	{r11, lr}
	mov	pc, lr
	.p2align	2
@ %bb.3:
.LCPI3_0:
	.long	.L.str
.Lfunc_end3:
	.size	assert_true, .Lfunc_end3-assert_true
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
	mov	r0, #3
	mov	r1, #4
	bl	addmul3
	str	r0, [sp]
	ldr	r0, [sp]
	sub	r0, r0, #21
	rsbs	r1, r0, #0
	adc	r0, r0, r1
	bl	assert_true
	mov	r0, #3
	mov	r1, #4
	bl	addmul3_inline
	str	r0, [sp]
	ldr	r0, [sp]
	sub	r0, r0, #21
	rsbs	r1, r0, #0
	adc	r0, r0, r1
	bl	assert_true
	mov	r0, #0
	mov	sp, r11
	pop	{r11, lr}
	mov	pc, lr
.Lfunc_end4:
	.size	main, .Lfunc_end4-main
	.cantunwind
	.fnend
                                        @ -- End function
	.type	.L.str,%object                  @ @.str
	.section	.rodata.str1.1,"aMS",%progbits,1
.L.str:
	.asciz	"assert fail\n"
	.size	.L.str, 13

	.ident	"Debian clang version 11.0.1-2"
	.section	".note.GNU-stack","",%progbits
