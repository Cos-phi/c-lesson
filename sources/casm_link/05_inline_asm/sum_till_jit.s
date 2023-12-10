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
	.file	"sum_till_jit.c"
	.globl	sum_till                        @ -- Begin function sum_till
	.p2align	2
	.type	sum_till,%function
	.code	32                              @ @sum_till
sum_till:
	.fnstart
@ %bb.0:
	sub	sp, sp, #12
	str	r0, [sp, #8]
	mov	r0, #0
	str	r0, [sp, #4]
	str	r0, [sp]
	b	.LBB0_1
.LBB0_1:                                @ =>This Inner Loop Header: Depth=1
	ldr	r0, [sp]
	ldr	r1, [sp, #8]
	cmp	r0, r1
	bge	.LBB0_4
	b	.LBB0_2
.LBB0_2:                                @   in Loop: Header=BB0_1 Depth=1
	ldr	r0, [sp]
	ldr	r1, [sp, #4]
	add	r0, r1, r0
	str	r0, [sp, #4]
	b	.LBB0_3
.LBB0_3:                                @   in Loop: Header=BB0_1 Depth=1
	ldr	r0, [sp]
	add	r0, r0, #1
	str	r0, [sp]
	b	.LBB0_1
.LBB0_4:
	ldr	r0, [sp, #4]
	add	sp, sp, #12
	mov	pc, lr
.Lfunc_end0:
	.size	sum_till, .Lfunc_end0-sum_till
	.cantunwind
	.fnend
                                        @ -- End function
	.globl	sum_till_inline                 @ -- Begin function sum_till_inline
	.p2align	2
	.type	sum_till_inline,%function
	.code	32                              @ @sum_till_inline
sum_till_inline:
	.fnstart
@ %bb.0:
	sub	sp, sp, #8
	str	r0, [sp, #4]
	@APP
	mov	r1, #0
	@NO_APP
	@APP
	mov	r2, #0
	@NO_APP
	@APP
loop:
	@NO_APP
	@APP
	cmp	r0, r1
	@NO_APP
	@APP
	beq	end
	@NO_APP
	@APP
	add	r2, r2, r1
	@NO_APP
	@APP
	add	r1, r1, #1
	@NO_APP
	@APP
	b	loop
	@NO_APP
	@APP
end:
	@NO_APP
	@APP
	mov	r0, r2
	@NO_APP
	str	r0, [sp]
	ldr	r0, [sp]
	add	sp, sp, #8
	mov	pc, lr
.Lfunc_end1:
	.size	sum_till_inline, .Lfunc_end1-sum_till_inline
	.cantunwind
	.fnend
                                        @ -- End function
	.globl	allocate_executable_buf         @ -- Begin function allocate_executable_buf
	.p2align	2
	.type	allocate_executable_buf,%function
	.code	32                              @ @allocate_executable_buf
allocate_executable_buf:
	.fnstart
@ %bb.0:
	push	{r11, lr}
	mov	r11, sp
	sub	sp, sp, #16
	str	r0, [r11, #-4]
	ldr	r1, [r11, #-4]
	mov	r0, #0
	str	r0, [sp, #4]
	mvn	r0, #0
	str	r0, [sp]
	mov	r0, #0
	mov	r2, #7
	mov	r3, #34
	bl	mmap
	mov	sp, r11
	pop	{r11, lr}
	mov	pc, lr
.Lfunc_end2:
	.size	allocate_executable_buf, .Lfunc_end2-allocate_executable_buf
	.cantunwind
	.fnend
                                        @ -- End function
	.globl	jit_sum_till                    @ -- Begin function jit_sum_till
	.p2align	2
	.type	jit_sum_till,%function
	.code	32                              @ @jit_sum_till
jit_sum_till:
	.fnstart
@ %bb.0:
	ldr	r0, .LCPI3_0
	ldr	r0, [r0]
	ldr	r1, .LCPI3_1
	str	r1, [r0]
	mov	pc, lr
	.p2align	2
@ %bb.1:
.LCPI3_0:
	.long	binary_buf
.LCPI3_1:
	.long	3785420814                      @ 0xe1a0f00e
.Lfunc_end3:
	.size	jit_sum_till, .Lfunc_end3-jit_sum_till
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
	bne	.LBB4_2
	b	.LBB4_1
.LBB4_1:
	ldr	r0, .LCPI4_0
	bl	printf
	b	.LBB4_2
.LBB4_2:
	mov	sp, r11
	pop	{r11, lr}
	mov	pc, lr
	.p2align	2
@ %bb.3:
.LCPI4_0:
	.long	.L.str
.Lfunc_end4:
	.size	assert_true, .Lfunc_end4-assert_true
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
	push	{r4, r10, r11, lr}
	add	r11, sp, #8
	sub	sp, sp, #16
	mov	r0, #0
	str	r0, [sp, #12]
	mov	r0, #10
	bl	sum_till
	str	r0, [sp, #8]
	ldr	r0, [sp, #8]
	sub	r0, r0, #45
	rsbs	r1, r0, #0
	adc	r0, r0, r1
	bl	assert_true
	mov	r0, #10
	bl	sum_till_inline
	str	r0, [sp, #8]
	ldr	r0, [sp, #8]
	sub	r0, r0, #45
	rsbs	r1, r0, #0
	adc	r0, r0, r1
	bl	assert_true
	mov	r0, #1024
	bl	allocate_executable_buf
	ldr	r4, .LCPI5_0
	str	r0, [r4]
	bl	jit_sum_till
	ldr	r0, [r4]
	str	r0, [sp, #4]
	ldr	r1, [sp, #4]
	mov	r0, #10
	mov	lr, pc
	mov	pc, r1
	str	r0, [sp, #8]
	ldr	r0, [sp, #8]
	sub	r0, r0, #45
	rsbs	r1, r0, #0
	adc	r0, r0, r1
	bl	assert_true
	ldr	r1, [sp, #4]
	mov	r0, #11
	mov	lr, pc
	mov	pc, r1
	str	r0, [sp, #8]
	ldr	r0, [sp, #8]
	sub	r0, r0, #55
	rsbs	r1, r0, #0
	adc	r0, r0, r1
	bl	assert_true
	mov	r0, #0
	sub	sp, r11, #8
	pop	{r4, r10, r11, lr}
	mov	pc, lr
	.p2align	2
@ %bb.1:
.LCPI5_0:
	.long	binary_buf
.Lfunc_end5:
	.size	main, .Lfunc_end5-main
	.cantunwind
	.fnend
                                        @ -- End function
	.type	binary_buf,%object              @ @binary_buf
	.bss
	.globl	binary_buf
	.p2align	2
binary_buf:
	.long	0
	.size	binary_buf, 4

	.type	.L.str,%object                  @ @.str
	.section	.rodata.str1.1,"aMS",%progbits,1
.L.str:
	.asciz	"assert fail\n"
	.size	.L.str, 13

	.ident	"Debian clang version 11.0.1-2"
	.section	".note.GNU-stack","",%progbits
