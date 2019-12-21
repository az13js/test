/**
 * write a png image, resources/t02_image.png
 */
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <png.h>

/**
 * Set a RGB image Pixels color
 *
 * @param void* buffer
 * @param unsigned int color
 * @param png_uint_32 x
 * @param png_uint_32 y
 * @param png_uint_32 width
 * @param png_uint_32 height
 * @return void
 */
void set_pixel_value(void* buffer, unsigned int color, png_uint_32 x, png_uint_32 y, int width, int height)
{
    unsigned char* byte_buffer = (unsigned char*)buffer;
    int offset = (x + y * width) * 3;
    byte_buffer[offset] = (color & 0xFF0000) >> 16;
    byte_buffer[offset + 1] = (color & 0x00FF00) >> 8;
    byte_buffer[offset + 2] = color & 0x00FF;
}

/**
 * Main function
 *
 * @param int argc Number of command parameters
 * @param char** argv The contents of the argument
 * @return int Return 0 if execution is successful
 */
int main(int argc, char *argv[])
{
    png_image image;
    png_uint_16p buffer;
    png_uint_32 x, y;

    // Declare a 'png_image' structure on the stack and memset()
    // it to all zero.
    memset(&image, 0, sizeof image);

    image.version = PNG_IMAGE_VERSION;
    image.format = PNG_FORMAT_RGB;
    image.width = 1600;
    image.height = 900;

    // Allocate a buffer for the image
    printf("IMAGE_SIZE %d\n", PNG_IMAGE_SIZE(image));
    if (NULL == (buffer = malloc(PNG_IMAGE_SIZE(image)))) {
        printf("malloc fail.\n");
        return -1;
    }

    // PIXEL_SIZE=3, if image.format = PNG_FORMAT_RGB
    printf(
        "PIXEL_SIZE %d, PIXEL_CHANNELS %d\n",
        PNG_IMAGE_PIXEL_SIZE(image.format),
        PNG_IMAGE_PIXEL_CHANNELS(image.format)
    );

    for (y = 0; y < image.height; y++) {
        for (x = 0; x < image.width; x++) {
            set_pixel_value(
                buffer,
                // color, 0x000000 ~ 0xFFFFFF, 0xRRGGBB
                255 * y / 900 << 16 | (255 * x / 1600 << 8) | (255 - (255 * x / 1600)),
                x,
                y,
                image.width,
                image.height
            );
        }
    }
    
    if (png_image_write_to_file(&image, "resources/t02_image.png", 0, buffer, 0, NULL)) {
        printf("Write image success.\n");
    } else {
        printf("Write image fail.\n");
    }
    free(buffer);
    return 0;
}
