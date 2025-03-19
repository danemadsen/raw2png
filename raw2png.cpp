/*
* raw2png.cpp
*
* This program reads a raw file in ARGB8888 format and converts it to a PNG image.
* Usage: raw2png input.raw width height output.png
*
* Note: The raw file is expected to have width*height pixels, each pixel being 4 bytes:
*       A, R, G, B.
*
*       The program converts this data into RGBA order for PNG output.
*
*       Download stb_image_write.h from:
*       https://github.com/nothings/stb/blob/master/stb_image_write.h
*       and place it in the same directory as this file.
*/

#include <algorithm>  // Needed for std::min in C++ mode

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_STATIC
#include "stb_image_write.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s input.raw width height output.png\n", argv[0]);
        return 1;
    }
    
    const char *input_file = argv[1];
    int width = atoi(argv[2]);
    int height = atoi(argv[3]);
    const char *output_file = argv[4];

    size_t num_pixels = (size_t)width * height;
    size_t expected_size = num_pixels * 2;  // 2 bytes per pixel (ARGB4444)

    // Open the input file in binary mode.
    FILE *fp = fopen(input_file, "rb");
    if (!fp) {
        perror("Error opening input file");
        return 1;
    }

    // Allocate memory to hold the raw data.
    uint8_t *raw_data = (uint8_t *)malloc(expected_size);
    if (!raw_data) {
        fprintf(stderr, "Memory allocation error\n");
        fclose(fp);
        return 1;
    }
    
    // Read the raw file into memory.
    size_t read_size = fread(raw_data, 1, expected_size, fp);
    fclose(fp);
    if (read_size != expected_size) {
        fprintf(stderr, "Error: Expected %zu bytes but read %zu bytes\n", expected_size, read_size);
        free(raw_data);
        return 1;
    }

    // Allocate memory for the converted RGBA data (8 bits per channel).
    uint8_t *rgba_data = (uint8_t *)malloc(num_pixels * 4);
    if (!rgba_data) {
        fprintf(stderr, "Memory allocation error\n");
        free(raw_data);
        return 1;
    }

    // Convert each pixel from ARGB4444 to RGBA8888.
    // For each pixel, two bytes are read:
    //   First byte: high nibble = A, low nibble = R.
    //   Second byte: high nibble = G, low nibble = B.
    // Each 4-bit value is expanded to 8 bits by: (value << 4) | value.
    for (size_t i = 0; i < num_pixels; i++) {
        uint8_t byte0 = raw_data[i * 2 + 0];
        uint8_t byte1 = raw_data[i * 2 + 1];

        uint8_t A = byte0 >> 4;
        uint8_t R = byte0 & 0x0F;
        uint8_t G = byte1 >> 4;
        uint8_t B = byte1 & 0x0F;

        uint8_t A8 = (A << 4) | A;
        uint8_t R8 = (R << 4) | R;
        uint8_t G8 = (G << 4) | G;
        uint8_t B8 = (B << 4) | B;

        // Write the pixel in RGBA order.
        rgba_data[i * 4 + 0] = R8;
        rgba_data[i * 4 + 1] = G8;
        rgba_data[i * 4 + 2] = B8;
        rgba_data[i * 4 + 3] = A8;
    }

    // Write the RGBA data as a PNG file.
    // The last parameter (width * 4) is the stride (bytes per row).
    int write_result = stbi_write_png(output_file, width, height, 4, rgba_data, width * 4);
    if (!write_result) {
        fprintf(stderr, "Failed to write PNG file.\n");
        free(raw_data);
        free(rgba_data);
        return 1;
    }
    printf("PNG image written to %s\n", output_file);

    free(raw_data);
    free(rgba_data);
    return 0;
}
 