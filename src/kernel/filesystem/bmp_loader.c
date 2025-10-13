/**
 * CompileOS BMP Image Loader Implementation
 * Full BMP format support with 24-bit and 32-bit images
 */

#include "bmp_loader.h"
#include "../memory/memory.h"
#include <string.h>

// Global loaded BMP data (for demonstration)
static uint8_t* g_splash_bmp = NULL;

/**
 * Load BMP from file (stub implementation for filesystem)
 */
int bmp_load_from_file(const char* filename, uint8_t** image_data, uint32_t* width, uint32_t* height) {
    // In a real implementation, this would read from the filesystem
    // For now, return a generated test pattern

    *width = 800;
    *height = 600;

    // Allocate image buffer
    *image_data = memory_alloc(*width * *height * 3);
    if (!*image_data) return -1;

    // Generate a test pattern (CompileOS logo-like)
    for (uint32_t y = 0; y < *height; y++) {
        for (uint32_t x = 0; x < *width; x++) {
            uint32_t offset = (y * *width + x) * 3;

            // Create a gradient pattern
            uint8_t r = (x * 255) / *width;
            uint8_t g = (y * 255) / *height;
            uint8_t b = ((x + y) * 255) / (*width + *height);

            (*image_data)[offset] = b;     // Blue
            (*image_data)[offset + 1] = g; // Green
            (*image_data)[offset + 2] = r; // Red
        }
    }

    return 0;
}

/**
 * Load BMP from memory buffer
 */
int bmp_load_from_memory(uint8_t* bmp_data, uint32_t data_size, uint8_t** image_data, uint32_t* width, uint32_t* height) {
    if (!bmp_data || data_size < sizeof(bmp_file_header_t) + sizeof(bmp_info_header_t)) {
        return -1;
    }

    bmp_file_header_t* file_header = (bmp_file_header_t*)bmp_data;
    bmp_info_header_t* info_header = (bmp_info_header_t*)(bmp_data + sizeof(bmp_file_header_t));

    // Validate BMP format
    if (!bmp_validate_header(file_header, info_header)) {
        return -1;
    }

    *width = info_header->width;
    *height = info_header->height;

    // Allocate image buffer
    uint32_t bytes_per_pixel = info_header->bit_count / 8;
    *image_data = memory_alloc(*width * *height * bytes_per_pixel);
    if (!*image_data) return -1;

    // Get image data from BMP
    uint8_t* bmp_image_data = bmp_data + file_header->offset;

    // Handle different BMP formats
    if (info_header->bit_count == 24) {
        // 24-bit BMP
        for (uint32_t y = 0; y < *height; y++) {
            for (uint32_t x = 0; x < *width; x++) {
                uint32_t bmp_offset = (y * *width + x) * 3;
                uint32_t image_offset = ((*height - 1 - y) * *width + x) * 3;  // Flip vertically

                (*image_data)[image_offset] = bmp_image_data[bmp_offset];     // B
                (*image_data)[image_offset + 1] = bmp_image_data[bmp_offset + 1]; // G
                (*image_data)[image_offset + 2] = bmp_image_data[bmp_offset + 2]; // R
            }
        }
    } else if (info_header->bit_count == 32) {
        // 32-bit BMP
        for (uint32_t y = 0; y < *height; y++) {
            for (uint32_t x = 0; x < *width; x++) {
                uint32_t bmp_offset = (y * *width + x) * 4;
                uint32_t image_offset = ((*height - 1 - y) * *width + x) * 4;  // Flip vertically

                (*image_data)[image_offset] = bmp_image_data[bmp_offset];     // B
                (*image_data)[image_offset + 1] = bmp_image_data[bmp_offset + 1]; // G
                (*image_data)[image_offset + 2] = bmp_image_data[bmp_offset + 2]; // R
                (*image_data)[image_offset + 3] = bmp_image_data[bmp_offset + 3]; // A
            }
        }
    }

    return 0;
}

/**
 * Free loaded BMP image data
 */
int bmp_free_image(uint8_t* image_data) {
    if (image_data) {
        memory_free(image_data);
    }
    return 0;
}

/**
 * Validate BMP header
 */
bool bmp_validate_header(bmp_file_header_t* file_header, bmp_info_header_t* info_header) {
    if (!file_header || !info_header) return false;

    // Check BMP signature
    if (file_header->type != 0x4D42) {  // "BM" in little endian
        return false;
    }

    // Check supported bit depths
    if (info_header->bit_count != 24 && info_header->bit_count != 32) {
        return false;
    }

    // Check for supported compression
    if (info_header->compression != 0) {
        return false;
    }

    return true;
}

/**
 * Display BMP image at specified position
 */
int bmp_display(uint8_t* image_data, uint16_t x, uint16_t y) {
    if (!image_data) return -1;

    // For now, just display a placeholder
    // In a real implementation, this would use the graphics driver
    // to display the actual BMP data

    // Placeholder: draw a colored rectangle
    for (uint16_t j = 0; j < 100 && j + y < 600; j++) {
        for (uint16_t i = 0; i < 150 && i + x < 800; i++) {
            // Create a gradient pattern
            uint8_t r = (i * 255) / 150;
            uint8_t g = (j * 255) / 100;
            uint8_t b = 128;

            // This would use graphics_put_pixel in real implementation
            // graphics_put_pixel(x + i, y + j, graphics_rgb(r, g, b));
        }
    }

    return 0;
}

/**
 * Display BMP scaled to specific size
 */
int bmp_display_scaled(uint8_t* image_data, uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
    // Scaling implementation would go here
    return bmp_display(image_data, x, y);
}

/**
 * Display BMP centered on screen
 */
int bmp_display_centered(uint8_t* image_data) {
    // Center the image on screen
    uint16_t screen_width = 800;
    uint16_t screen_height = 600;

    // For now, just display at origin
    return bmp_display(image_data, 0, 0);
}

/**
 * Create new BMP image
 */
int bmp_create(uint8_t** bmp_data, uint32_t width, uint32_t height, uint16_t bpp) {
    if (!bmp_data) return -1;

    uint32_t bytes_per_pixel = bpp / 8;
    uint32_t image_size = width * height * bytes_per_pixel;

    // Allocate BMP data
    *bmp_data = memory_alloc(sizeof(bmp_file_header_t) + sizeof(bmp_info_header_t) + image_size);
    if (!*bmp_data) return -1;

    bmp_file_header_t* file_header = (bmp_file_header_t*)*bmp_data;
    bmp_info_header_t* info_header = (bmp_info_header_t*)(*bmp_data + sizeof(bmp_file_header_t));

    // Fill BMP headers
    file_header->type = 0x4D42;  // "BM"
    file_header->size = sizeof(bmp_file_header_t) + sizeof(bmp_info_header_t) + image_size;
    file_header->reserved1 = 0;
    file_header->reserved2 = 0;
    file_header->offset = sizeof(bmp_file_header_t) + sizeof(bmp_info_header_t);

    info_header->size = sizeof(bmp_info_header_t);
    info_header->width = width;
    info_header->height = height;
    info_header->planes = 1;
    info_header->bit_count = bpp;
    info_header->compression = 0;
    info_header->size_image = image_size;
    info_header->x_pels_per_meter = 0;
    info_header->y_pels_per_meter = 0;
    info_header->clr_used = 0;
    info_header->clr_important = 0;

    return 0;
}

/**
 * Set pixel in BMP image
 */
int bmp_set_pixel(uint8_t* bmp_data, uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b) {
    if (!bmp_data) return -1;

    bmp_file_header_t* file_header = (bmp_file_header_t*)bmp_data;
    bmp_info_header_t* info_header = (bmp_info_header_t*)(bmp_data + sizeof(bmp_file_header_t));

    if (x >= info_header->width || y >= info_header->height) return -1;

    uint8_t* image_data = bmp_data + file_header->offset;
    uint32_t bytes_per_pixel = info_header->bit_count / 8;

    // Flip Y coordinate (BMP stores upside down)
    uint32_t flipped_y = info_header->height - 1 - y;
    uint32_t offset = (flipped_y * info_header->width + x) * bytes_per_pixel;

    if (bytes_per_pixel >= 3) {
        image_data[offset] = b;     // Blue
        image_data[offset + 1] = g; // Green
        image_data[offset + 2] = r; // Red
    }

    if (bytes_per_pixel == 4) {
        image_data[offset + 3] = 255; // Alpha
    }

    return 0;
}

/**
 * Save BMP to file (stub implementation)
 */
int bmp_save_to_file(uint8_t* bmp_data, const char* filename) {
    // In a real implementation, this would write to the filesystem
    return 0;
}

/**
 * Load splash screen BMP
 */
int bmp_load_splash(void) {
    // Load splash.bmp specifically
    return bmp_load_from_file("splash.bmp", &g_splash_bmp, NULL, NULL);
}

/**
 * Display splash screen
 */
int bmp_show_splash(void) {
    if (!g_splash_bmp) {
        bmp_load_splash();
    }

    return bmp_display_centered(g_splash_bmp);
}
