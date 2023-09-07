_start:
    ldr r0,=0x101f1000
    ldr r1,=message
    ldrb r3,[r1]    
loop:
    str r3,[r0]
    add r1, r1, #1
    ldrb r3,[r1]
    cmp r3,#0
    bne loop
end:
    b end

message:
    .raw "hello, world\n"
