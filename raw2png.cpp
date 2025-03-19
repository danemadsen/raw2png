#include <algorithm>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_STATIC
#include "stb_image_write.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s input.raw output.png\n", argv[0]);
        return 1;
    }
    
    const char *input_file = argv[1];
    const char *output_file = argv[2];

    // Open the input file in binary mode.
    FILE *fp = fopen(input_file, "rb");
    if (!fp) {
        perror("Error opening input file");
        return 1;
    }

    // Determine the size of the file
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    rewind(fp);

    if (file_size % 2 != 0) {
        fprintf(stderr, "Invalid file size. Must be divisible by 2.\n");
        fclose(fp);
        return 1;
    }

    size_t num_pixels = file_size / 2;
    int dimension = (int)sqrt(num_pixels);

    printf("Number of pixels: %zu\n", num_pixels);
    printf("Dimension: %d x %d\n", dimension, dimension);

    int threshold = 10;
    if (abs((dimension * dimension) - (int)num_pixels) > threshold) {
        fprintf(stderr, "Input file does not represent a square image.\n");
        fclose(fp);
        return 1;
    }

    int width = dimension;
    int height = dimension;

    size_t expected_size = num_pixels * 2;

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

    // Convert each pixel from BGRA4444 to RGBA8888 with accurate scaling.
    for (size_t i = 0; i < num_pixels; i++) {
        uint8_t byte0 = raw_data[i * 2 + 0];
        uint8_t byte1 = raw_data[i * 2 + 1];

        uint8_t R4 = byte1 >> 4;
        uint8_t G4 = byte1 & 0x0F;
        uint8_t B4 = byte0 >> 4;
        uint8_t A4 = byte0 & 0x0F;

        uint8_t R8 = (R4 * 255 + 7) / 15;
        uint8_t G8 = (G4 * 255 + 7) / 15;
        uint8_t B8 = (B4 * 255 + 7) / 15;
        uint8_t A8 = (A4 * 255 + 7) / 15;

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
 