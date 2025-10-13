/**
 * CompileOS VGA Graphics Implementation
 * Handles BMP loading and VGA 320x200 256-color mode
 */

#include "vga_graphics.h"
#include <string.h>

// VGA I/O functions
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline void outw(uint16_t port, uint16_t val) {
    __asm__ volatile("outw %0, %1" : : "a"(val), "Nd"(port));
}

/**
 * Set VGA to 320x200 256-color graphics mode
 */
void vga_set_graphics_mode(void) {
    // Set VGA to mode 13h (320x200 256 colors)

    // Miscellaneous Output Register
    outb(VGA_MISC_WRITE, 0x63);

    // Sequencer Registers
    outb(VGA_SEQ_INDEX, 0x00);
    outb(VGA_SEQ_DATA, 0x03);

    outb(VGA_SEQ_INDEX, 0x01);
    outb(VGA_SEQ_DATA, 0x01);

    outb(VGA_SEQ_INDEX, 0x02);
    outb(VGA_SEQ_DATA, 0x0F);

    outb(VGA_SEQ_INDEX, 0x03);
    outb(VGA_SEQ_DATA, 0x00);

    outb(VGA_SEQ_INDEX, 0x04);
    outb(VGA_SEQ_DATA, 0x0E);

    // CRTC Registers
    outb(VGA_CRTC_INDEX, 0x00);
    outb(VGA_CRTC_DATA, 0x5F);

    outb(VGA_CRTC_INDEX, 0x01);
    outb(VGA_CRTC_DATA, 0x4F);

    outb(VGA_CRTC_INDEX, 0x02);
    outb(VGA_CRTC_DATA, 0x50);

    outb(VGA_CRTC_INDEX, 0x03);
    outb(VGA_CRTC_DATA, 0x82);

    outb(VGA_CRTC_INDEX, 0x04);
    outb(VGA_CRTC_DATA, 0x54);

    outb(VGA_CRTC_INDEX, 0x05);
    outb(VGA_CRTC_DATA, 0x80);

    outb(VGA_CRTC_INDEX, 0x06);
    outb(VGA_CRTC_DATA, 0xBF);

    outb(VGA_CRTC_INDEX, 0x07);
    outb(VGA_CRTC_DATA, 0x1F);

    outb(VGA_CRTC_INDEX, 0x08);
    outb(VGA_CRTC_DATA, 0x00);

    outb(VGA_CRTC_INDEX, 0x09);
    outb(VGA_CRTC_DATA, 0x41);

    outb(VGA_CRTC_INDEX, 0x10);
    outb(VGA_CRTC_DATA, 0x9C);

    outb(VGA_CRTC_INDEX, 0x11);
    outb(VGA_CRTC_DATA, 0x8E);

    outb(VGA_CRTC_INDEX, 0x12);
    outb(VGA_CRTC_DATA, 0x8F);

    outb(VGA_CRTC_INDEX, 0x13);
    outb(VGA_CRTC_DATA, 0x28);

    outb(VGA_CRTC_INDEX, 0x14);
    outb(VGA_CRTC_DATA, 0x00);

    outb(VGA_CRTC_INDEX, 0x15);
    outb(VGA_CRTC_DATA, 0x96);

    outb(VGA_CRTC_INDEX, 0x16);
    outb(VGA_CRTC_DATA, 0xB9);

    outb(VGA_CRTC_INDEX, 0x17);
    outb(VGA_CRTC_DATA, 0xE3);

    // Graphics Controller Registers
    outb(VGA_GC_INDEX, 0x00);
    outb(VGA_GC_DATA, 0x00);

    outb(VGA_GC_INDEX, 0x01);
    outb(VGA_GC_DATA, 0x00);

    outb(VGA_GC_INDEX, 0x02);
    outb(VGA_GC_DATA, 0x00);

    outb(VGA_GC_INDEX, 0x03);
    outb(VGA_GC_DATA, 0x00);

    outb(VGA_GC_INDEX, 0x04);
    outb(VGA_GC_DATA, 0x00);

    outb(VGA_GC_INDEX, 0x05);
    outb(VGA_GC_DATA, 0x40);

    outb(VGA_GC_INDEX, 0x06);
    outb(VGA_GC_DATA, 0x05);

    outb(VGA_GC_INDEX, 0x07);
    outb(VGA_GC_DATA, 0x0F);

    outb(VGA_GC_INDEX, 0x08);
    outb(VGA_GC_DATA, 0xFF);

    // Attribute Controller Registers
    inb(VGA_INSTAT_READ);  // Reset attribute controller

    outb(VGA_AC_INDEX, 0x00);
    outb(VGA_AC_WRITE, 0x00);

    outb(VGA_AC_INDEX, 0x01);
    outb(VGA_AC_WRITE, 0x01);

    outb(VGA_AC_INDEX, 0x02);
    outb(VGA_AC_WRITE, 0x02);

    outb(VGA_AC_INDEX, 0x03);
    outb(VGA_AC_WRITE, 0x03);

    outb(VGA_AC_INDEX, 0x04);
    outb(VGA_AC_WRITE, 0x04);

    outb(VGA_AC_INDEX, 0x05);
    outb(VGA_AC_WRITE, 0x05);

    outb(VGA_AC_INDEX, 0x06);
    outb(VGA_AC_WRITE, 0x06);

    outb(VGA_AC_INDEX, 0x07);
    outb(VGA_AC_WRITE, 0x07);

    outb(VGA_AC_INDEX, 0x08);
    outb(VGA_AC_WRITE, 0x08);

    outb(VGA_AC_INDEX, 0x09);
    outb(VGA_AC_WRITE, 0x09);

    outb(VGA_AC_INDEX, 0x0A);
    outb(VGA_AC_WRITE, 0x0A);

    outb(VGA_AC_INDEX, 0x0B);
    outb(VGA_AC_WRITE, 0x0B);

    outb(VGA_AC_INDEX, 0x0C);
    outb(VGA_AC_WRITE, 0x0C);

    outb(VGA_AC_INDEX, 0x0D);
    outb(VGA_AC_WRITE, 0x0D);

    outb(VGA_AC_INDEX, 0x0E);
    outb(VGA_AC_WRITE, 0x0E);

    outb(VGA_AC_INDEX, 0x0F);
    outb(VGA_AC_WRITE, 0x0F);

    outb(VGA_AC_INDEX, 0x10);
    outb(VGA_AC_WRITE, 0x41);

    outb(VGA_AC_INDEX, 0x11);
    outb(VGA_AC_WRITE, 0x00);

    outb(VGA_AC_INDEX, 0x12);
    outb(VGA_AC_WRITE, 0x0F);

    outb(VGA_AC_INDEX, 0x13);
    outb(VGA_AC_WRITE, 0x00);

    outb(VGA_AC_INDEX, 0x14);
    outb(VGA_AC_WRITE, 0x00);
}

/**
 * Set VGA back to 80x25 text mode
 */
void vga_set_text_mode(void) {
    // Set VGA to mode 3 (80x25 text mode)
    outb(VGA_MISC_WRITE, 0x67);

    outb(VGA_SEQ_INDEX, 0x00);
    outb(VGA_SEQ_DATA, 0x03);

    outb(VGA_SEQ_INDEX, 0x01);
    outb(VGA_SEQ_DATA, 0x00);

    outb(VGA_SEQ_INDEX, 0x02);
    outb(VGA_SEQ_DATA, 0x03);

    outb(VGA_SEQ_INDEX, 0x03);
    outb(VGA_SEQ_DATA, 0x00);

    outb(VGA_SEQ_INDEX, 0x04);
    outb(VGA_SEQ_DATA, 0x02);

    outb(VGA_CRTC_INDEX, 0x00);
    outb(VGA_CRTC_DATA, 0x5F);

    outb(VGA_CRTC_INDEX, 0x01);
    outb(VGA_CRTC_DATA, 0x4F);

    outb(VGA_CRTC_INDEX, 0x02);
    outb(VGA_CRTC_DATA, 0x50);

    outb(VGA_CRTC_INDEX, 0x03);
    outb(VGA_CRTC_DATA, 0x82);

    outb(VGA_CRTC_INDEX, 0x04);
    outb(VGA_CRTC_DATA, 0x55);

    outb(VGA_CRTC_INDEX, 0x05);
    outb(VGA_CRTC_DATA, 0x81);

    outb(VGA_CRTC_INDEX, 0x06);
    outb(VGA_CRTC_DATA, 0xBF);

    outb(VGA_CRTC_INDEX, 0x07);
    outb(VGA_CRTC_DATA, 0x1F);

    outb(VGA_CRTC_INDEX, 0x08);
    outb(VGA_CRTC_DATA, 0x00);

    outb(VGA_CRTC_INDEX, 0x09);
    outb(VGA_CRTC_DATA, 0x4F);

    outb(VGA_CRTC_INDEX, 0x10);
    outb(VGA_CRTC_DATA, 0x9C);

    outb(VGA_CRTC_INDEX, 0x11);
    outb(VGA_CRTC_DATA, 0x8E);

    outb(VGA_CRTC_INDEX, 0x12);
    outb(VGA_CRTC_DATA, 0x8F);

    outb(VGA_CRTC_INDEX, 0x13);
    outb(VGA_CRTC_DATA, 0x28);

    outb(VGA_CRTC_INDEX, 0x14);
    outb(VGA_CRTC_DATA, 0x1F);

    outb(VGA_CRTC_INDEX, 0x15);
    outb(VGA_CRTC_DATA, 0x96);

    outb(VGA_CRTC_INDEX, 0x16);
    outb(VGA_CRTC_DATA, 0xB9);

    outb(VGA_CRTC_INDEX, 0x17);
    outb(VGA_CRTC_DATA, 0xA3);

    outb(VGA_GC_INDEX, 0x00);
    outb(VGA_GC_DATA, 0x00);

    outb(VGA_GC_INDEX, 0x01);
    outb(VGA_GC_DATA, 0x00);

    outb(VGA_GC_INDEX, 0x02);
    outb(VGA_GC_DATA, 0x00);

    outb(VGA_GC_INDEX, 0x03);
    outb(VGA_GC_DATA, 0x00);

    outb(VGA_GC_INDEX, 0x04);
    outb(VGA_GC_DATA, 0x00);

    outb(VGA_GC_INDEX, 0x05);
    outb(VGA_GC_DATA, 0x10);

    outb(VGA_GC_INDEX, 0x06);
    outb(VGA_GC_DATA, 0x0E);

    outb(VGA_GC_INDEX, 0x07);
    outb(VGA_GC_DATA, 0x00);

    outb(VGA_GC_INDEX, 0x08);
    outb(VGA_GC_DATA, 0xFF);
}

/**
 * Load BMP file into memory (simplified for CompileOS)
 */
bool bmp_load(const char* filename, uint8_t** palette, uint8_t** image_data, int* width, int* height) {
    // For now, return a simple test pattern
    // In a full implementation, this would read the BMP file

    *width = 320;
    *height = 200;

    // Allocate palette (256 colors * 3 bytes each)
    *palette = (uint8_t*)0x10000;  // Use some memory for palette

    // Default VGA palette
    for (int i = 0; i < 256; i++) {
        (*palette)[i * 3] = i;      // Red
        (*palette)[i * 3 + 1] = i;  // Green
        (*palette)[i * 3 + 2] = i;  // Blue
    }

    // Allocate image data
    *image_data = (uint8_t*)0x20000;  // Use some memory for image

    // Create a simple test pattern (CompileOS logo)
    for (int y = 0; y < 200; y++) {
        for (int x = 0; x < 320; x++) {
            if (x > 100 && x < 220 && y > 50 && y < 150) {
                // Logo area
                if ((x - 160) * (x - 160) + (y - 100) * (y - 100) < 2500) {
                    (*image_data)[y * 320 + x] = 15;  // White circle
                } else {
                    (*image_data)[y * 320 + x] = 1;   // Blue background
                }
            } else {
                (*image_data)[y * 320 + x] = 0;   // Black background
            }
        }
    }

    return true;
}

/**
 * Display BMP image on VGA graphics screen
 */
void bmp_display(uint8_t* image_data, uint8_t* palette, int width, int height) {
    // Set palette first
    vga_set_palette(palette);

    // Copy image data to VGA memory
    uint8_t* vga_mem = (uint8_t*)0xA0000;

    for (int y = 0; y < height && y < 200; y++) {
        for (int x = 0; x < width && x < 320; x++) {
            vga_mem[y * 320 + x] = image_data[y * width + x];
        }
    }
}

/**
 * Wait for specified milliseconds
 */
void delay_ms(int milliseconds) {
    for (volatile int i = 0; i < milliseconds * 1000; i++) {
        // Busy wait
    }
}

/**
 * Draw pixel at coordinates
 */
void vga_plot_pixel(int x, int y, uint8_t color) {
    if (x >= 0 && x < 320 && y >= 0 && y < 200) {
        uint8_t* vga_mem = (uint8_t*)0xA0000;
        vga_mem[y * 320 + x] = color;
    }
}

/**
 * Draw rectangle
 */
void vga_draw_rect(int x1, int y1, int x2, int y2, uint8_t color) {
    for (int y = y1; y <= y2; y++) {
        for (int x = x1; x <= x2; x++) {
            vga_plot_pixel(x, y, color);
        }
    }
}

/**
 * Draw text in graphics mode (simple font)
 */
void vga_draw_text(const char* text, int x, int y, uint8_t color) {
    // Simple bitmap font (8x8 pixels per character)
    for (int i = 0; text[i]; i++) {
        char c = text[i];
        for (int cy = 0; cy < 8; cy++) {
            for (int cx = 0; cx < 8; cx++) {
                if (c & (1 << (7 - cx))) {
                    vga_plot_pixel(x + (i * 8) + cx, y + cy, color);
                }
            }
        }
    }
}

/**
 * Set VGA palette from BMP palette data
 */
void vga_set_palette(uint8_t* palette) {
    outb(VGA_DAC_WRITE_INDEX, 0x00);

    for (int i = 0; i < 256; i++) {
        outb(VGA_DAC_DATA, palette[i * 3] >> 2);     // Red
        outb(VGA_DAC_DATA, palette[i * 3 + 1] >> 2); // Green
        outb(VGA_DAC_DATA, palette[i * 3 + 2] >> 2); // Blue
    }
}

/**
 * Load CompileOS splash screen
 */
bool load_splash_screen(void) {
    uint8_t* palette;
    uint8_t* image_data;
    int width, height;

    return bmp_load("Splash.bmp", &palette, &image_data, &width, &height);
}

/**
 * Display splash screen for specified duration
 */
void show_splash_screen(int duration_seconds) {
    // Set graphics mode
    vga_set_graphics_mode();

    // Load and display splash image
    uint8_t* palette;
    uint8_t* image_data;
    int width, height;

    if (bmp_load("Splash.bmp", &palette, &image_data, &width, &height)) {
        bmp_display(image_data, palette, width, height);
    } else {
        // Fallback: draw CompileOS logo
        draw_compileos_logo();
    }

    // Wait for duration
    delay_ms(duration_seconds * 1000);

    // Back to text mode
    vga_set_text_mode();
}

/**
 * Draw CompileOS logo/title in graphics mode
 */
void draw_compileos_logo(void) {
    // Clear screen (blue background)
    vga_draw_rect(0, 0, 319, 199, 1);

    // Draw CompileOS logo
    vga_draw_text("CompileOS", 120, 80, 15);
    vga_draw_text("v0.1.0", 140, 100, 15);

    // Draw loading message
    vga_draw_text("Loading...", 130, 160, 14);
}
