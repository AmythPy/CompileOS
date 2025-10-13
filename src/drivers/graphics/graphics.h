/**
 * CompileOS Graphics Driver - Enhanced
 * High-resolution VESA graphics with true color support
 */

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Graphics modes
typedef enum {
    GRAPHICS_MODE_TEXT_80x25 = 0,
    GRAPHICS_MODE_VESA_640x480_16,
    GRAPHICS_MODE_VESA_800x600_24,
    GRAPHICS_MODE_VESA_1024x768_32,
    GRAPHICS_MODE_VESA_1280x1024_32
} graphics_mode_t;

// Color formats
typedef struct {
    uint8_t r, g, b, a;
} __attribute__((packed)) color_rgba_t;

// Graphics context
typedef struct {
    graphics_mode_t mode;
    uint16_t width, height;
    uint8_t bpp;
    uint32_t pitch;
    uintptr_t framebuffer;
    uintptr_t backbuffer;
    color_rgba_t* buffer;
    size_t size;
    bool double_buffering;
} graphics_context_t;

// VESA mode info structure
typedef struct {
    uint16_t attributes;
    uint8_t window_a, window_b;
    uint16_t granularity;
    uint16_t window_size;
    uint16_t segment_a, segment_b;
    uint32_t window_function_ptr;
    uint16_t pitch;
    uint16_t resolution_x, resolution_y;
    uint8_t w_char, y_char, planes, bpp, banks;
    uint8_t memory_model, bank_size, image_pages;
    uint8_t reserved0;
    uint8_t red_mask, red_position;
    uint8_t green_mask, green_position;
    uint8_t blue_mask, blue_position;
    uint8_t direct_color_attributes;
    uint32_t framebuffer;
    uint32_t off_screen_mem_off;
    uint16_t off_screen_mem_size;
    uint8_t reserved1[206];
} __attribute__((packed)) vesa_mode_info_t;

// VESA BIOS Extensions
int vesa_get_mode_info(uint16_t mode, vesa_mode_info_t* info);
int vesa_set_mode(uint16_t mode);
int vesa_set_bios_mode(uint16_t mode);
int vesa_get_controller_info(void);

// Hardware detection
bool graphics_detect_vesa_support(void);
uint16_t* graphics_get_vesa_modes_list(void);
int graphics_get_resolution(uint16_t* width, uint16_t* height);
int graphics_get_bpp(uint8_t* bpp);
int graphics_set_vga_mode(void);

// Pixel operations
void graphics_put_pixel(uint16_t x, uint16_t y, color_rgba_t color);
void graphics_fill_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, color_rgba_t color);
void graphics_draw_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, color_rgba_t color);

// Advanced graphics
void graphics_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, color_rgba_t color);
void graphics_draw_circle(uint16_t x, uint16_t y, uint16_t radius, color_rgba_t color);
void graphics_fill_circle(uint16_t x, uint16_t y, uint16_t radius, color_rgba_t color);
void graphics_draw_bitmap(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t* bitmap);

// Text rendering in graphics mode
int graphics_draw_char(uint16_t x, uint16_t y, char c, color_rgba_t fg, color_rgba_t bg);
int graphics_draw_string(uint16_t x, uint16_t y, const char* str, color_rgba_t fg, color_rgba_t bg);
int graphics_draw_text(uint16_t x, uint16_t y, const char* text, color_rgba_t fg, color_rgba_t bg);

// Double buffering
int graphics_flip(void);
void graphics_enable_double_buffering(bool enable);
bool graphics_is_double_buffering(void);

// Framebuffer operations
int graphics_clear(color_rgba_t color);
uint8_t* graphics_get_framebuffer(void);
int graphics_save_region(uint16_t x, uint16_t y, uint16_t width, uint16_t height, color_rgba_t* buffer);
int graphics_restore_region(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const color_rgba_t* buffer);

// Color utilities
color_rgba_t graphics_rgb(uint8_t r, uint8_t g, uint8_t b);
color_rgba_t graphics_RGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
/* Implementation provides lowercase graphics_rgba; expose it here for consumers */
color_rgba_t graphics_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
color_rgba_t graphics_color_from_hex(uint32_t hex);

#endif // GRAPHICS_H
