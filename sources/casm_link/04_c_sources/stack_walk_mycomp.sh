#!/bin/sh
./mycomp.sh stack_walk.c
arm-linux-gnueabi-gcc stack_walk.s
qemu-arm -L /usr/arm-linux-gnueabi ./a.out
rm a.out