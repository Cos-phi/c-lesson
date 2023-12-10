    mov r1, #0
    mov r2, #0
loop:
    cmp r0, r1
    beq end
    add r2, r2, r1
    add r1, r1, #1
    b loop
end:
    mov r0, r2
    mov r15, r14
