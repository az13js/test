#!/bin/bash

REPO_DIR=`pwd`
LIB_PNG="$REPO_DIR/libpng"
CC=gcc

cd $REPO_DIR
rm -f bin/*.bin

for c_file in src/t*.c
do
    c_file_name=`echo $c_file | grep -Eo "t[0-9]+_\w+"`
    bin_file="bin/$c_file_name.bin"
    COMMAND="$CC -pie -O3 -o $bin_file -I$LIB_PNG/include -L$LIB_PNG/lib $c_file -lpng"
    echo $COMMAND
    $COMMAND
    echo "----------------------------------------------------------------------"
    if [ -f $bin_file ]; then
        echo "Try to execute $bin_file"
        $bin_file
        echo "----------------------------------------------------------------------"
    fi
done
