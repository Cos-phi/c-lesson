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
	.file	"hello_jit.c"
	.globl	return5_inline                  @ -- Begin function return5_inline
	.p2align	2
	.type	return5_inline,%function
	.code	32                              @ @return5_inline
return5_inline:
	.fnstart
@ %bb.0:
	sub	sp, sp, #4
	@APP
	mov	r0, #5
	@NO_APP
	str	r0, [sp]
	ldr	r0, [sp]
	add	sp, sp, #4
	mov	pc, lr
.Lfunc_end0:
	.size	return5_inline, .Lfunc_end0-return5_inline
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
.Lfunc_end1:
	.size	allocate_executable_buf, .Lfunc_end1-allocate_executable_buf
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
	bne	.LBB2_2
	b	.LBB2_1
.LBB2_1:
	ldr	r0, .LCPI2_0
	bl	printf
	b	.LBB2_2
.LBB2_2:
	mov	sp, r11
	pop	{r11, lr}
	mov	pc, lr
	.p2align	2
@ %bb.3:
.LCPI2_0:
	.long	.L.str
.Lfunc_end2:
	.size	assert_true, .Lfunc_end2-assert_true
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
	sub	sp, sp, #16
	mov	r0, #0
	str	r0, [r11, #-4]
	bl	return5_inline
	str	r0, [sp, #8]
	ldr	r0, [sp, #8]
	sub	r0, r0, #5
	rsbs	r1, r0, #0
	adc	r0, r0, r1
	bl	assert_true
	mov	r0, #1024
	bl	allocate_executable_buf
	ldr	r1, .LCPI3_0
	str	r0, [r1]
	ldr	r0, [r1]
	ldr	r2, .LCPI3_1
	str	r2, [r0]
	ldr	r0, [r1]
	ldr	r2, .LCPI3_2
	str	r2, [r0, #4]
	ldr	r0, [r1]
	str	r0, [sp, #4]
	ldr	r0, [sp, #4]
	mov	lr, pc
	mov	pc, r0
	str	r0, [sp, #8]
	ldr	r1, [sp, #8]
	ldr	r0, .LCPI3_3
	bl	printf
	ldr	r0, [sp, #8]
	sub	r0, r0, #5
	rsbs	r1, r0, #0
	adc	r0, r0, r1
	bl	assert_true
	mov	r0, #0
	mov	sp, r11
	pop	{r11, lr}
	mov	pc, lr
	.p2align	2
@ %bb.1:
.LCPI3_0:
	.long	binary_buf
.LCPI3_1:
	.long	3818913797                      @ 0xe3a00005
.LCPI3_2:
	.long	3785420814                      @ 0xe1a0f00e
.LCPI3_3:
	.long	.L.str.1
.Lfunc_end3:
	.size	main, .Lfunc_end3-main
	.cantunwind
	.fnend
                                        @ -- End function
	.type	.L.str,%object                  @ @.str
	.section	.rodata.str1.1,"aMS",%progbits,1
.L.str:
	.asciz	"assert fail\n"
	.size	.L.str, 13

	.type	binary_buf,%object              @ @binary_buf
	.bss
	.globl	binary_buf
	.p2align	2
binary_buf:
	.long	0
	.size	binary_buf, 4

	.type	.L.str.1,%object                @ @.str.1
	.section	.rodata.str1.1,"aMS",%progbits,1
.L.str.1:
	.asciz	"%d\n"
	.size	.L.str.1, 4

	.ident	"Debian clang version 11.0.1-2"
	.section	".note.GNU-stack","",%progbits
