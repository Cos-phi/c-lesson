#!/bin/sh
./mycomp.sh stack_walk2.c
arm-linux-gnueabi-gcc stack_walk2.s
qemu-arm -L /usr/arm-linux-gnueabi ./a.out
rm a.out