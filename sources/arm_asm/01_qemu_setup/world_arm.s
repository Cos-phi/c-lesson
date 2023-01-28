/*
;
; https://stackoverflow.com/questions/15802748/arm-assembly-using-qemu
;
; arm-none-eabi-as hello_arm.s -o hello_arm.o
; arm-none-eabi-ld hello_arm.o -Ttext 0x00010000 -o hello_arm.elf
; arm-none-eabi-objcopy hello_arm.elf -O binary hello_arm.bin
; qemu-system-arm -M versatilepb -m 128M -nographic -kernel hello_arm.bin -serial mon:stdio
;
*/
.globl _start
_start:
    ldr r0,=0x101f1000
    mov r1,#0x77
    str r1,[r0]
    mov r1,#0x6F
    str r1,[r0]
    mov r1,#0x72
    str r1,[r0]
    mov r1,#0x6c
    str r1,[r0]
    mov r1,#0x64
    str r1,[r0]
    mov r2,#0x0D
    str r2,[r0]
    mov r2,#0x0A
    str r2,[r0]
loop:
    b loop
