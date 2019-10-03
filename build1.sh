#!/bin/sh

rm -f test1.bin
gcc test1.c -O3 -o test1.bin
./test1.bin