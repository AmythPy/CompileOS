/**
 * CompileOS Bitmap Display System for VGA Graphics Mode
 * Loads and displays BMP images in 320x200 VGA 256-color mode
 */

#ifndef VGA_GRAPHICS_H
#define VGA_GRAPHICS_H

#include <stdint.h>
#include <stdbool.h>

// VGA Graphics Mode Constants
#define VGA_MODE_13H 0x13  // 320x200 256-color mode
#define VGA_WIDTH 320
#define VGA_HEIGHT 200

// VGA Register Ports
#define VGA_AC_INDEX 0x3C0
#define VGA_AC_WRITE 0x3C0
#define VGA_AC_READ 0x3C1
#define VGA_MISC_WRITE 0x3C2
#define VGA_SEQ_INDEX 0x3C4
#define VGA_SEQ_DATA 0x3C5
#define VGA_DAC_READ_INDEX 0x3C7
#define VGA_DAC_WRITE_INDEX 0x3C8
#define VGA_DAC_DATA 0x3C9
#define VGA_MISC_READ 0x3CC
#define VGA_GC_INDEX 0x3CE
#define VGA_GC_DATA 0x3CF
#define VGA_CRTC_INDEX 0x3D4
#define VGA_CRTC_DATA 0x3D5
#define VGA_INSTAT_READ 0x3DA

// VGA Plane Masks
#define VGA_PLANE_0 0x01
#define VGA_PLANE_1 0x02
#define VGA_PLANE_2 0x04
#define VGA_PLANE_3 0x08

// Bitmap File Header (BMP)
typedef struct {
    uint16_t bfType;      // File type ("BM")
    uint32_t bfSize;      // File size
    uint16_t bfReserved1; // Reserved
    uint16_t bfReserved2; // Reserved
    uint32_t bfOffBits;   // Offset to bitmap data
} __attribute__((packed)) bmp_file_header_t;

// Bitmap Info Header
typedef struct {
    uint32_t biSize;          // Header size
    int32_t biWidth;         // Image width
    int32_t biHeight;        // Image height
    uint16_t biPlanes;       // Planes (must be 1)
    uint16_t biBitCount;     // Bits per pixel
    uint32_t biCompression;  // Compression type
    uint32_t biSizeImage;    // Image size
    int32_t biXPelsPerMeter; // X pixels per meter
    int32_t biYPelsPerMeter; // Y pixels per meter
    uint32_t biClrUsed;      // Colors used
    uint32_t biClrImportant; // Important colors
} __attribute__((packed)) bmp_info_header_t;

// VGA Graphics Functions

/**
 * Set VGA to 320x200 256-color graphics mode
 */
void vga_set_graphics_mode(void);

/**
 * Set VGA back to 80x25 text mode
 */
void vga_set_text_mode(void);

/**
 * Load BMP file into memory
 */
bool bmp_load(const char* filename, uint8_t** palette, uint8_t** image_data, int* width, int* height);

/**
 * Display BMP image on VGA graphics screen
 */
void bmp_display(uint8_t* image_data, uint8_t* palette, int width, int height);

/**
 * Wait for specified milliseconds
 */
void delay_ms(int milliseconds);

/**
 * Draw pixel at coordinates
 */
void vga_plot_pixel(int x, int y, uint8_t color);

/**
 * Draw rectangle
 */
void vga_draw_rect(int x1, int y1, int x2, int y2, uint8_t color);

/**
 * Draw text in graphics mode (simple font)
 */
void vga_draw_text(const char* text, int x, int y, uint8_t color);

/**
 * Set VGA palette from BMP palette data
 */
void vga_set_palette(uint8_t* palette);

/**
 * Load CompileOS splash screen
 */
bool load_splash_screen(void);

/**
 * Display splash screen for specified duration
 */
void show_splash_screen(int duration_seconds);

/**
 * Draw CompileOS logo/title in graphics mode
 */
void draw_compileos_logo(void);

#endif /* VGA_GRAPHICS_H */
