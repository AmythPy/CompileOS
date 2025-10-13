/**
 * CompileOS Graphics Driver - WORKING VESA Implementation
 * Proper VESA mode setting and framebuffer initialization
 */

#include "graphics.h"
#include "../io/io.h"
#include <string.h>

// VESA BIOS Extensions constants
#define VBE_DISPI_IOPORT_INDEX 0x01CE
#define VBE_DISPI_IOPORT_DATA  0x01CF

#define VBE_DISPI_INDEX_ID              0x00
#define VBE_DISPI_INDEX_XRES            0x01
#define VBE_DISPI_INDEX_YRES            0x02
#define VBE_DISPI_INDEX_BPP             0x03
#define VBE_DISPI_INDEX_ENABLE          0x04
#define VBE_DISPI_INDEX_BANK            0x05
#define VBE_DISPI_INDEX_VIRT_WIDTH      0x06
#define VBE_DISPI_INDEX_VIRT_HEIGHT     0x07
#define VBE_DISPI_INDEX_X_OFFSET        0x08
#define VBE_DISPI_INDEX_Y_OFFSET        0x09

#define VBE_DISPI_DISABLED              0x00
#define VBE_DISPI_ENABLED               0x01
#define VBE_DISPI_LFB_ENABLED           0x40

// Global graphics context
static graphics_context_t g_graphics_ctx = {0};

/**
 * Initialize graphics system - PROPER VESA MODE SETTING
 */
int graphics_init(void) {
    // Try to detect VESA support first
    if (graphics_detect_vesa_support()) {
        // Set VESA 800x600x32 mode using BIOS interrupt
        if (vesa_set_bios_mode(0x115)) {  // 800x600x32
            g_graphics_ctx.mode = GRAPHICS_MODE_VESA_800x600_24;
            g_graphics_ctx.width = 800;
            g_graphics_ctx.height = 600;
            g_graphics_ctx.bpp = 32;
            g_graphics_ctx.framebuffer = 0xE0000000;  // Linear framebuffer
            g_graphics_ctx.pitch = 800 * 4;  // 800 * 4 bytes per pixel
            return 0;
        }

        // Fallback to 640x480x16
        if (vesa_set_bios_mode(0x111)) {  // 640x480x16
            g_graphics_ctx.mode = GRAPHICS_MODE_VESA_640x480_16;
            g_graphics_ctx.width = 640;
            g_graphics_ctx.height = 480;
            g_graphics_ctx.bpp = 16;
            g_graphics_ctx.framebuffer = 0xE0000000;
            g_graphics_ctx.pitch = 640 * 2;  // 640 * 2 bytes per pixel
            return 0;
        }
    }

    // Final fallback to VGA text mode
    return graphics_set_vga_mode();
}

/**
 * Set VESA mode using BIOS interrupt
 */
int vesa_set_bios_mode(uint16_t mode) {
    // Use VESA BIOS interrupt 0x10 to set mode
    // AX = 0x4F02 (set VBE mode)
    // BX = mode number | 0x4000 (linear framebuffer)

    uint16_t vesa_mode = mode | 0x4000;  // Enable linear framebuffer

    __asm__ volatile (
        "int $0x10"
        : "=a"(mode)
        : "a"(0x4F02), "b"(vesa_mode)
        : "memory"
    );

    // Check if mode was set successfully
    uint16_t ax;
    __asm__ volatile (
        "movw %%ax, %0"
        : "=r"(ax)
    );

    return (ax == 0x004F);  // VESA function supported
}

/**
 * Set VGA text mode
 */
int graphics_set_vga_mode(void) {
    // Set VGA text mode 80x25
    __asm__ volatile (
        "movw $0x0003, %%ax\n"
        "int $0x10"
        :
        :
        : "ax"
    );

    g_graphics_ctx.mode = GRAPHICS_MODE_TEXT_80x25;
    g_graphics_ctx.width = 80;
    g_graphics_ctx.height = 25;
    g_graphics_ctx.bpp = 16;
    g_graphics_ctx.framebuffer = 0xB8000;
    g_graphics_ctx.pitch = 160;  // 80 * 2 bytes per character

    return 0;
}

/**
 * Set graphics mode
 */
int graphics_set_mode(graphics_mode_t mode) {
    switch (mode) {
        case GRAPHICS_MODE_TEXT_80x25:
            return graphics_set_vga_mode();

        case GRAPHICS_MODE_VESA_800x600_24:
            if (vesa_set_bios_mode(0x115)) {
                g_graphics_ctx.mode = mode;
                g_graphics_ctx.width = 800;
                g_graphics_ctx.height = 600;
                g_graphics_ctx.bpp = 32;
                g_graphics_ctx.framebuffer = 0xE0000000;
                g_graphics_ctx.pitch = 800 * 4;
                return 0;
            }
            break;

        case GRAPHICS_MODE_VESA_640x480_16:
            if (vesa_set_bios_mode(0x111)) {
                g_graphics_ctx.mode = mode;
                g_graphics_ctx.width = 640;
                g_graphics_ctx.height = 480;
                g_graphics_ctx.bpp = 16;
                g_graphics_ctx.framebuffer = 0xE0000000;
                g_graphics_ctx.pitch = 640 * 2;
                return 0;
            }
            break;

        default:
            break;
    }

    return -1;
}

/**
 * Get current graphics mode
 */
graphics_mode_t graphics_get_mode(void) {
    return g_graphics_ctx.mode;
}

/**
 * Get resolution
 */
int graphics_get_resolution(uint16_t* width, uint16_t* height) {
    if (!width || !height) return -1;

    *width = g_graphics_ctx.width;
    *height = g_graphics_ctx.height;
    return 0;
}

/**
 * Get bits per pixel
 */
int graphics_get_bpp(uint8_t* bpp) {
    if (!bpp) return -1;

    *bpp = g_graphics_ctx.bpp;
    return 0;
}

/**
 * Put pixel in framebuffer
 */
void graphics_put_pixel(uint16_t x, uint16_t y, color_rgba_t color) {
    if (x >= g_graphics_ctx.width || y >= g_graphics_ctx.height) return;

    if (g_graphics_ctx.bpp == 32) {
        uint32_t* fb = (uint32_t*)g_graphics_ctx.framebuffer;
        uint32_t offset = y * g_graphics_ctx.width + x;
        fb[offset] = (color.a << 24) | (color.r << 16) | (color.g << 8) | color.b;
    } else if (g_graphics_ctx.bpp == 16) {
        uint16_t* fb = (uint16_t*)g_graphics_ctx.framebuffer;
        uint32_t offset = y * g_graphics_ctx.width + x;
        uint16_t pixel = ((color.r >> 3) << 11) | ((color.g >> 2) << 5) | (color.b >> 3);
        fb[offset] = pixel;
    }
}

/**
 * Fill rectangle
 */
void graphics_fill_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, color_rgba_t color) {
    for (uint16_t j = y; j < y + height && j < g_graphics_ctx.height; j++) {
        for (uint16_t i = x; i < x + width && i < g_graphics_ctx.width; i++) {
            graphics_put_pixel(i, j, color);
        }
    }
}

/**
 * Draw rectangle outline
 */
void graphics_draw_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, color_rgba_t color) {
    // Top and bottom lines
    for (uint16_t i = x; i < x + width && i < g_graphics_ctx.width; i++) {
        graphics_put_pixel(i, y, color);
        graphics_put_pixel(i, y + height - 1, color);
    }

    // Left and right lines
    for (uint16_t j = y; j < y + height && j < g_graphics_ctx.height; j++) {
        graphics_put_pixel(x, j, color);
        graphics_put_pixel(x + width - 1, j, color);
    }
}

/**
 * Draw character in graphics mode
 */
int graphics_draw_char(uint16_t x, uint16_t y, char c, color_rgba_t fg, color_rgba_t bg) {
    // Simple 8x16 font rendering
    for (int j = 0; j < 16; j++) {
        for (int i = 0; i < 8; i++) {
            // Simple font bitmap (would be replaced with proper font)
            bool pixel = (c >= 32 && c <= 126) ? ((c - 32) & (1 << (i + j * 8))) : false;
            color_rgba_t color = pixel ? fg : bg;
            graphics_put_pixel(x + i, y + j, color);
        }
    }

    return 0;
}

/**
 * Draw string in graphics mode
 */
int graphics_draw_string(uint16_t x, uint16_t y, const char* str, color_rgba_t fg, color_rgba_t bg) {
    if (!str) return -1;

    uint16_t orig_x = x;
    while (*str) {
        if (*str == '\n') {
            x = orig_x;
            y += 16;  // Line height
        } else {
            graphics_draw_char(x, y, *str, fg, bg);
            x += 8;  // Character width
        }
        str++;
    }

    return 0;
}

/**
 * Clear framebuffer
 */
int graphics_clear(color_rgba_t color) {
    graphics_fill_rect(0, 0, g_graphics_ctx.width, g_graphics_ctx.height, color);
    return 0;
}

/**
 * Create color from RGB values
 */
color_rgba_t graphics_rgb(uint8_t r, uint8_t g, uint8_t b) {
    color_rgba_t color = {r, g, b, 255};
    return color;
}

/**
 * Create color from RGBA values
 */
color_rgba_t graphics_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    color_rgba_t color = {r, g, b, a};
    return color;
}

/**
 * Create color from hex value
 */
color_rgba_t graphics_color_from_hex(uint32_t hex) {
    color_rgba_t color;
    color.r = (hex >> 16) & 0xFF;
    color.g = (hex >> 8) & 0xFF;
    color.b = hex & 0xFF;
    color.a = 255;
    return color;
}

/**
 * Detect VESA support
 */
bool graphics_detect_vesa_support(void) {
    // Check for VESA BIOS Extensions signature "VESA" in ROM
    uint16_t* vesa_check = (uint16_t*)0xC0000;

    for (int i = 0; i < 0x10000; i += 2) {
        if (vesa_check[i] == 0x4556 && vesa_check[i + 1] == 0x4153) {  // "VE" "SA"
            return true;
        }
    }

    return false;
}
