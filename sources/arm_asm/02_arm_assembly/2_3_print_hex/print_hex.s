/*
 arm-none-eabi-as print_hex.s -o print_hex.o
 arm-none-eabi-ld print_hex.o -Ttext 0x00010000 -o print_hex.elf
 arm-none-eabi-objcopy print_hex.elf -O binary print_hex.bin
 qemu-system-arm -M versatilepb -m 128M -nographic -kernel print_hex.bin -serial mon:stdio
*/
.globl _start
_start:
    ldr r1,=0xdead1234
    b print_hex
print_hex:
    // TODO: implement here
    ldr r0,=0x101f1000
    mov r3,#28
loop:
    lsr r2,r1,r3
    and r2,r2,#0x0F
    cmp r2,#10
    blt arabicnum
    add r2,r2,#55
    b print
arabicnum:
    add r2,r2,#48
print:
    str r2,[r0]
    sub r3,#4
    cmp r3,#0
    bge loop
    mov r4,#0x0D
    str r4,[r0]
    mov r4,#0x0A
    str r4,[r0]
end:
    b end
