/*
    Simple test
 */
#include <stdio.h>
#include <png.h>

int main(int argc, char *argv[])
{
    printf("Hello world!\n");
    printf("%s\n", PNG_LIBPNG_VER_STRING);
    printf("%s\n", PNG_HEADER_VERSION_STRING);
    return 0;
}
