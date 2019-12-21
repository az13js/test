#!/bin/bash

REPO_DIR=`pwd`
LIB_PNG="$REPO_DIR/libpng"
CC=gcc
CFLAGS=-O3
CPP=g++

# Remove the compiled file, ready to be recompressed and compiled
rm -fr "$REPO_DIR/libpng"
rm -fr "$REPO_DIR/libpng-1.6.37"

# Unzip the source code file for "libpng"
tar -axf libpng-1.6.37.tar.gz

cd "$REPO_DIR/libpng-1.6.37"

# Configuration and compilation
./configure \
--prefix="$LIB_PNG" \
--enable-hardware-optimizations=yes \
--enable-intel-sse=yes \
--with-zlib-prefix=/usr/local

make
# "make check" wastes too much time
# make check
make install