/**
 * CompileOS BMP Image Loader
 * Load and display BMP images with full color support
 */

#ifndef BMP_LOADER_H
#define BMP_LOADER_H

#include <stdint.h>
#include <stdbool.h>

// BMP file structures
typedef struct {
    uint16_t type;          // File type ("BM")
    uint32_t size;          // File size
    uint16_t reserved1;     // Reserved
    uint16_t reserved2;     // Reserved
    uint32_t offset;        // Offset to image data
} __attribute__((packed)) bmp_file_header_t;

typedef struct {
    uint32_t size;          // Header size
    int32_t width;          // Image width
    int32_t height;         // Image height
    uint16_t planes;        // Color planes
    uint16_t bit_count;     // Bits per pixel
    uint32_t compression;   // Compression type
    uint32_t size_image;    // Image size
    int32_t x_pels_per_meter; // X pixels per meter
    int32_t y_pels_per_meter; // Y pixels per meter
    uint32_t clr_used;      // Colors used
    uint32_t clr_important; // Important colors
} __attribute__((packed)) bmp_info_header_t;

// BMP loader functions
int bmp_load_from_file(const char* filename, uint8_t** image_data, uint32_t* width, uint32_t* height);
int bmp_load_from_memory(uint8_t* bmp_data, uint32_t data_size, uint8_t** image_data, uint32_t* width, uint32_t* height);
int bmp_free_image(uint8_t* image_data);
bool bmp_validate_header(bmp_file_header_t* file_header, bmp_info_header_t* info_header);

// BMP display functions
int bmp_display(uint8_t* image_data, uint16_t x, uint16_t y);
int bmp_display_scaled(uint8_t* image_data, uint16_t x, uint16_t y, uint16_t width, uint16_t height);
int bmp_display_centered(uint8_t* image_data);

// BMP creation functions
int bmp_create(uint8_t** bmp_data, uint32_t width, uint32_t height, uint16_t bpp);
int bmp_set_pixel(uint8_t* bmp_data, uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b);
int bmp_save_to_file(uint8_t* bmp_data, const char* filename);

#endif // BMP_LOADER_H
