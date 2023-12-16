#!/bin/sh
export ASAN_OPTIONS=detect_leaks=0
arm-linux-gnueabi-gcc -g -fsanitize=address *.c
qemu-arm -L /usr/arm-linux-gnueabi ./a.out