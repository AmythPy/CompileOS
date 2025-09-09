/**
 * CompileOS VGA Driver - Header
 * 
 * VGA text mode and graphics mode support
 */

#ifndef VGA_H
#define VGA_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// VGA display modes
typedef enum {
    VGA_MODE_TEXT_80x25 = 0,
    VGA_MODE_TEXT_80x50,
    VGA_MODE_GRAPHICS_320x200,
    VGA_MODE_GRAPHICS_640x480,
    VGA_MODE_GRAPHICS_800x600,
    VGA_MODE_GRAPHICS_1024x768
} vga_mode_t;

// VGA colors
typedef enum {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE,
    VGA_COLOR_GREEN,
    VGA_COLOR_CYAN,
    VGA_COLOR_RED,
    VGA_COLOR_MAGENTA,
    VGA_COLOR_BROWN,
    VGA_COLOR_LIGHT_GRAY,
    VGA_COLOR_DARK_GRAY,
    VGA_COLOR_LIGHT_BLUE,
    VGA_COLOR_LIGHT_GREEN,
    VGA_COLOR_LIGHT_CYAN,
    VGA_COLOR_LIGHT_RED,
    VGA_COLOR_LIGHT_MAGENTA,
    VGA_COLOR_YELLOW,
    VGA_COLOR_WHITE
} vga_color_t;

// VGA character attributes
typedef struct {
    uint8_t foreground : 4;
    uint8_t background : 4;
    bool blink : 1;
    bool bold : 1;
} vga_attributes_t;

// VGA cursor position
typedef struct {
    uint16_t x;
    uint16_t y;
} vga_cursor_t;

// VGA screen buffer
typedef struct {
    uint16_t width;
    uint16_t height;
    uint16_t* buffer;
    vga_cursor_t cursor;
    vga_attributes_t current_attr;
    bool cursor_visible;
} vga_screen_t;

// VGA initialization and control
int vga_init(void);
void vga_shutdown(void);
int vga_set_mode(vga_mode_t mode);
vga_mode_t vga_get_mode(void);

// Text mode functions
int vga_clear_screen(void);
int vga_clear_screen_color(vga_color_t background);
int vga_set_cursor(uint16_t x, uint16_t y);
vga_cursor_t vga_get_cursor(void);
int vga_set_cursor_visible(bool visible);
bool vga_is_cursor_visible(void);

// Character output
int vga_putchar(char c);
int vga_putchar_at(uint16_t x, uint16_t y, char c);
int vga_putchar_color(char c, vga_color_t foreground, vga_color_t background);
int vga_putchar_attr(char c, vga_attributes_t attr);

// String output
int vga_puts(const char* str);
int vga_puts_at(uint16_t x, uint16_t y, const char* str);
int vga_puts_color(const char* str, vga_color_t foreground, vga_color_t background);
int vga_puts_attr(const char* str, vga_attributes_t attr);

// Formatted output
int vga_printf(const char* format, ...);
int vga_printf_at(uint16_t x, uint16_t y, const char* format, ...);
int vga_printf_color(vga_color_t foreground, vga_color_t background, const char* format, ...);

// Color and attribute functions
int vga_set_foreground_color(vga_color_t color);
int vga_set_background_color(vga_color_t color);
int vga_set_attributes(vga_attributes_t attr);
vga_attributes_t vga_get_attributes(void);
vga_color_t vga_get_foreground_color(void);
vga_color_t vga_get_background_color(void);

// Screen manipulation
int vga_scroll_up(void);
int vga_scroll_down(void);
int vga_scroll_up_lines(uint16_t lines);
int vga_scroll_down_lines(uint16_t lines);
int vga_clear_line(uint16_t line);
int vga_clear_line_from_cursor(void);
int vga_clear_screen_from_cursor(void);

// Buffer access
uint16_t* vga_get_buffer(void);
uint16_t vga_get_buffer_width(void);
uint16_t vga_get_buffer_height(void);
int vga_get_char_at(uint16_t x, uint16_t y, char* c, vga_attributes_t* attr);
int vga_set_char_at(uint16_t x, uint16_t y, char c, vga_attributes_t attr);

// Graphics mode functions (basic)
int vga_graphics_init(void);
int vga_graphics_clear(uint8_t color);
int vga_graphics_put_pixel(uint16_t x, uint16_t y, uint8_t color);
uint8_t vga_graphics_get_pixel(uint16_t x, uint16_t y);
int vga_graphics_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t color);
int vga_graphics_draw_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t color);
int vga_graphics_fill_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t color);
int vga_graphics_draw_circle(uint16_t x, uint16_t y, uint16_t radius, uint8_t color);
int vga_graphics_fill_circle(uint16_t x, uint16_t y, uint16_t radius, uint8_t color);

// Font rendering
int vga_draw_char_graphics(uint16_t x, uint16_t y, char c, uint8_t color);
int vga_draw_string_graphics(uint16_t x, uint16_t y, const char* str, uint8_t color);
int vga_draw_char_large(uint16_t x, uint16_t y, char c, uint8_t color);

// Screen information
int vga_get_screen_info(uint16_t* width, uint16_t* height, vga_mode_t* mode);
bool vga_is_text_mode(void);
bool vga_is_graphics_mode(void);

// VGA port I/O
int vga_write_register(uint16_t port, uint8_t value);
int vga_read_register(uint16_t port, uint8_t* value);
int vga_write_register_pair(uint16_t port, uint8_t index, uint8_t value);
int vga_read_register_pair(uint16_t port, uint8_t index, uint8_t* value);

// VGA palette
int vga_set_palette_color(uint8_t index, uint8_t r, uint8_t g, uint8_t b);
int vga_get_palette_color(uint8_t index, uint8_t* r, uint8_t* g, uint8_t* b);
int vga_load_palette(const uint8_t* palette, size_t count);

// VGA cursor control
int vga_cursor_home(void);
int vga_cursor_move(int dx, int dy);
int vga_cursor_next_line(void);
int vga_cursor_prev_line(void);
int vga_cursor_tab(uint16_t tab_size);

// VGA screen save/restore
int vga_save_screen(vga_screen_t* screen);
int vga_restore_screen(const vga_screen_t* screen);
int vga_clear_screen_buffer(void);

// VGA splash screen
int vga_display_splash(const char* title, const char* subtitle);
int vga_display_boot_message(const char* message);
int vga_display_progress_bar(uint16_t x, uint16_t y, uint16_t width, uint8_t progress);

#endif // VGA_H
