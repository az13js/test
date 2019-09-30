#!/bin/sh

rm -f test.bin
gcc test.c -lm -O3 -o test.bin
./test.bin