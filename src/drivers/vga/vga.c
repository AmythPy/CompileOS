/**
 * CompileOS VGA Driver - Implementation
 * 
 * VGA text mode and basic graphics support
 */

#include "vga.h"
#include "../../hal/arch/x86_64/io.h"
#include <string.h>

// VGA state
static struct {
    bool initialized;
    vga_mode_t current_mode;
    uint16_t width;
    uint16_t height;
    uint16_t* buffer;
    vga_cursor_t cursor;
    vga_attributes_t current_attr;
    bool cursor_visible;
} g_vga_state = {0};

/**
 * Initialize VGA
 */
int vga_init(void) {
    if (g_vga_state.initialized) {
        return 0;
    }
    
    // Set up 80x25 text mode
    g_vga_state.current_mode = VGA_MODE_TEXT_80x25;
    g_vga_state.width = 80;
    g_vga_state.height = 25;
    g_vga_state.buffer = (uint16_t*)0xB8000; // VGA text buffer
    g_vga_state.cursor.x = 0;
    g_vga_state.cursor.y = 0;
    g_vga_state.cursor_visible = true;
    g_vga_state.current_attr.foreground = VGA_COLOR_WHITE;
    g_vga_state.current_attr.background = VGA_COLOR_BLACK;
    g_vga_state.current_attr.blink = false;
    g_vga_state.current_attr.bold = false;
    
    // Clear screen
    vga_clear_screen();
    
    g_vga_state.initialized = true;
    return 0;
}

/**
 * Shutdown VGA
 */
void vga_shutdown(void) {
    if (!g_vga_state.initialized) {
        return;
    }
    
    g_vga_state.initialized = false;
}

/**
 * Set VGA mode
 */
int vga_set_mode(vga_mode_t mode) {
    if (!g_vga_state.initialized) {
        return -1;
    }
    
    switch (mode) {
        case VGA_MODE_TEXT_80x25:
            g_vga_state.current_mode = mode;
            g_vga_state.width = 80;
            g_vga_state.height = 25;
            g_vga_state.buffer = (uint16_t*)0xB8000;
            break;
        case VGA_MODE_TEXT_80x50:
            g_vga_state.current_mode = mode;
            g_vga_state.width = 80;
            g_vga_state.height = 50;
            g_vga_state.buffer = (uint16_t*)0xB8000;
            break;
        default:
            return -1;
    }
    
    vga_clear_screen();
    return 0;
}

/**
 * Get current VGA mode
 */
vga_mode_t vga_get_mode(void) {
    return g_vga_state.current_mode;
}

/**
 * Clear screen
 */
int vga_clear_screen(void) {
    if (!g_vga_state.initialized) {
        return -1;
    }
    
    uint16_t blank = ' ' | (g_vga_state.current_attr.background << 4) | g_vga_state.current_attr.foreground;
    
    for (uint16_t i = 0; i < g_vga_state.width * g_vga_state.height; i++) {
        g_vga_state.buffer[i] = blank;
    }
    
    g_vga_state.cursor.x = 0;
    g_vga_state.cursor.y = 0;
    
    return 0;
}

/**
 * Clear screen with color
 */
int vga_clear_screen_color(vga_color_t background) {
    vga_attributes_t old_attr = g_vga_state.current_attr;
    g_vga_state.current_attr.background = background;
    int result = vga_clear_screen();
    g_vga_state.current_attr = old_attr;
    return result;
}

/**
 * Set cursor position
 */
int vga_set_cursor(uint16_t x, uint16_t y) {
    if (!g_vga_state.initialized || x >= g_vga_state.width || y >= g_vga_state.height) {
        return -1;
    }
    
    g_vga_state.cursor.x = x;
    g_vga_state.cursor.y = y;
    
    // Update hardware cursor
    uint16_t pos = y * g_vga_state.width + x;
    io_outb(VGA_CRTC_INDEX, 0x0F);
    io_outb(VGA_CRTC_DATA, pos & 0xFF);
    io_outb(VGA_CRTC_INDEX, 0x0E);
    io_outb(VGA_CRTC_DATA, (pos >> 8) & 0xFF);
    
    return 0;
}

/**
 * Get cursor position
 */
vga_cursor_t vga_get_cursor(void) {
    return g_vga_state.cursor;
}

/**
 * Set cursor visibility
 */
int vga_set_cursor_visible(bool visible) {
    if (!g_vga_state.initialized) {
        return -1;
    }
    
    g_vga_state.cursor_visible = visible;
    
    // Update hardware cursor
    if (visible) {
        vga_set_cursor(g_vga_state.cursor.x, g_vga_state.cursor.y);
    } else {
        io_outb(VGA_CRTC_INDEX, 0x0A);
        io_outb(VGA_CRTC_DATA, 0x20);
    }
    
    return 0;
}

/**
 * Check if cursor is visible
 */
bool vga_is_cursor_visible(void) {
    return g_vga_state.cursor_visible;
}

/**
 * Put character at current cursor position
 */
int vga_putchar(char c) {
    if (!g_vga_state.initialized) {
        return -1;
    }
    
    if (c == '\n') {
        g_vga_state.cursor.x = 0;
        g_vga_state.cursor.y++;
        if (g_vga_state.cursor.y >= g_vga_state.height) {
            vga_scroll_up();
            g_vga_state.cursor.y = g_vga_state.height - 1;
        }
    } else if (c == '\r') {
        g_vga_state.cursor.x = 0;
    } else if (c == '\t') {
        g_vga_state.cursor.x = (g_vga_state.cursor.x + 8) & ~7;
        if (g_vga_state.cursor.x >= g_vga_state.width) {
            g_vga_state.cursor.x = 0;
            g_vga_state.cursor.y++;
        }
    } else if (c == '\b') {
        if (g_vga_state.cursor.x > 0) {
            g_vga_state.cursor.x--;
        } else if (g_vga_state.cursor.y > 0) {
            g_vga_state.cursor.x = g_vga_state.width - 1;
            g_vga_state.cursor.y--;
        }
    } else {
        // Put character
        uint16_t pos = g_vga_state.cursor.y * g_vga_state.width + g_vga_state.cursor.x;
        uint16_t attr = (g_vga_state.current_attr.background << 4) | g_vga_state.current_attr.foreground;
        g_vga_state.buffer[pos] = c | (attr << 8);
        
        g_vga_state.cursor.x++;
        if (g_vga_state.cursor.x >= g_vga_state.width) {
            g_vga_state.cursor.x = 0;
            g_vga_state.cursor.y++;
            if (g_vga_state.cursor.y >= g_vga_state.height) {
                vga_scroll_up();
                g_vga_state.cursor.y = g_vga_state.height - 1;
            }
        }
    }
    
    vga_set_cursor(g_vga_state.cursor.x, g_vga_state.cursor.y);
    return 0;
}

/**
 * Put character at specific position
 */
int vga_putchar_at(uint16_t x, uint16_t y, char c) {
    if (!g_vga_state.initialized || x >= g_vga_state.width || y >= g_vga_state.height) {
        return -1;
    }
    
    uint16_t pos = y * g_vga_state.width + x;
    uint16_t attr = (g_vga_state.current_attr.background << 4) | g_vga_state.current_attr.foreground;
    g_vga_state.buffer[pos] = c | (attr << 8);
    
    return 0;
}

/**
 * Put character with color
 */
int vga_putchar_color(char c, vga_color_t foreground, vga_color_t background) {
    vga_attributes_t old_attr = g_vga_state.current_attr;
    g_vga_state.current_attr.foreground = foreground;
    g_vga_state.current_attr.background = background;
    int result = vga_putchar(c);
    g_vga_state.current_attr = old_attr;
    return result;
}

/**
 * Put character with attributes
 */
int vga_putchar_attr(char c, vga_attributes_t attr) {
    vga_attributes_t old_attr = g_vga_state.current_attr;
    g_vga_state.current_attr = attr;
    int result = vga_putchar(c);
    g_vga_state.current_attr = old_attr;
    return result;
}

/**
 * Put string
 */
int vga_puts(const char* str) {
    if (!g_vga_state.initialized || !str) {
        return -1;
    }
    
    while (*str) {
        vga_putchar(*str++);
    }
    
    return 0;
}

/**
 * Put string at specific position
 */
int vga_puts_at(uint16_t x, uint16_t y, const char* str) {
    if (!g_vga_state.initialized || !str) {
        return -1;
    }
    
    vga_cursor_t old_cursor = g_vga_state.cursor;
    g_vga_state.cursor.x = x;
    g_vga_state.cursor.y = y;
    
    int result = vga_puts(str);
    
    g_vga_state.cursor = old_cursor;
    return result;
}

/**
 * Put string with color
 */
int vga_puts_color(const char* str, vga_color_t foreground, vga_color_t background) {
    vga_attributes_t old_attr = g_vga_state.current_attr;
    g_vga_state.current_attr.foreground = foreground;
    g_vga_state.current_attr.background = background;
    int result = vga_puts(str);
    g_vga_state.current_attr = old_attr;
    return result;
}

/**
 * Put string with attributes
 */
int vga_puts_attr(const char* str, vga_attributes_t attr) {
    vga_attributes_t old_attr = g_vga_state.current_attr;
    g_vga_state.current_attr = attr;
    int result = vga_puts(str);
    g_vga_state.current_attr = old_attr;
    return result;
}

/**
 * Formatted output
 */
int vga_printf(const char* format, ...) {
    // Simple implementation - just print the format string
    // TODO: Implement proper printf functionality
    return vga_puts(format);
}

/**
 * Scroll screen up
 */
int vga_scroll_up(void) {
    if (!g_vga_state.initialized) {
        return -1;
    }
    
    // Move all lines up by one
    for (uint16_t y = 0; y < g_vga_state.height - 1; y++) {
        for (uint16_t x = 0; x < g_vga_state.width; x++) {
            uint16_t src_pos = (y + 1) * g_vga_state.width + x;
            uint16_t dst_pos = y * g_vga_state.width + x;
            g_vga_state.buffer[dst_pos] = g_vga_state.buffer[src_pos];
        }
    }
    
    // Clear bottom line
    uint16_t blank = ' ' | (g_vga_state.current_attr.background << 4) | g_vga_state.current_attr.foreground;
    for (uint16_t x = 0; x < g_vga_state.width; x++) {
        uint16_t pos = (g_vga_state.height - 1) * g_vga_state.width + x;
        g_vga_state.buffer[pos] = blank;
    }
    
    return 0;
}

/**
 * Set foreground color
 */
int vga_set_foreground_color(vga_color_t color) {
    if (!g_vga_state.initialized) {
        return -1;
    }
    
    g_vga_state.current_attr.foreground = color;
    return 0;
}

/**
 * Set background color
 */
int vga_set_background_color(vga_color_t color) {
    if (!g_vga_state.initialized) {
        return -1;
    }
    
    g_vga_state.current_attr.background = color;
    return 0;
}

/**
 * Set attributes
 */
int vga_set_attributes(vga_attributes_t attr) {
    if (!g_vga_state.initialized) {
        return -1;
    }
    
    g_vga_state.current_attr = attr;
    return 0;
}

/**
 * Get attributes
 */
vga_attributes_t vga_get_attributes(void) {
    return g_vga_state.current_attr;
}

/**
 * Get foreground color
 */
vga_color_t vga_get_foreground_color(void) {
    return g_vga_state.current_attr.foreground;
}

/**
 * Get background color
 */
vga_color_t vga_get_background_color(void) {
    return g_vga_state.current_attr.background;
}

/**
 * Get buffer
 */
uint16_t* vga_get_buffer(void) {
    return g_vga_state.buffer;
}

/**
 * Get buffer width
 */
uint16_t vga_get_buffer_width(void) {
    return g_vga_state.width;
}

/**
 * Get buffer height
 */
uint16_t vga_get_buffer_height(void) {
    return g_vga_state.height;
}

/**
 * Get character at position
 */
int vga_get_char_at(uint16_t x, uint16_t y, char* c, vga_attributes_t* attr) {
    if (!g_vga_state.initialized || x >= g_vga_state.width || y >= g_vga_state.height || !c || !attr) {
        return -1;
    }
    
    uint16_t pos = y * g_vga_state.width + x;
    uint16_t data = g_vga_state.buffer[pos];
    
    *c = data & 0xFF;
    attr->foreground = data & 0x0F;
    attr->background = (data >> 4) & 0x0F;
    attr->blink = false;
    attr->bold = false;
    
    return 0;
}

/**
 * Set character at position
 */
int vga_set_char_at(uint16_t x, uint16_t y, char c, vga_attributes_t attr) {
    if (!g_vga_state.initialized || x >= g_vga_state.width || y >= g_vga_state.height) {
        return -1;
    }
    
    uint16_t pos = y * g_vga_state.width + x;
    uint16_t data = c | (attr.background << 4) | attr.foreground;
    g_vga_state.buffer[pos] = data;
    
    return 0;
}

/**
 * Check if in text mode
 */
bool vga_is_text_mode(void) {
    return g_vga_state.current_mode == VGA_MODE_TEXT_80x25 || g_vga_state.current_mode == VGA_MODE_TEXT_80x50;
}

/**
 * Check if in graphics mode
 */
bool vga_is_graphics_mode(void) {
    return !vga_is_text_mode();
}

/**
 * Get screen info
 */
int vga_get_screen_info(uint16_t* width, uint16_t* height, vga_mode_t* mode) {
    if (!g_vga_state.initialized || !width || !height || !mode) {
        return -1;
    }
    
    *width = g_vga_state.width;
    *height = g_vga_state.height;
    *mode = g_vga_state.current_mode;
    
    return 0;
}

/**
 * Move cursor to home position
 */
int vga_cursor_home(void) {
    return vga_set_cursor(0, 0);
}

/**
 * Move cursor to next line
 */
int vga_cursor_next_line(void) {
    if (!g_vga_state.initialized) {
        return -1;
    }
    
    g_vga_state.cursor.x = 0;
    g_vga_state.cursor.y++;
    
    if (g_vga_state.cursor.y >= g_vga_state.height) {
        vga_scroll_up();
        g_vga_state.cursor.y = g_vga_state.height - 1;
    }
    
    return vga_set_cursor(g_vga_state.cursor.x, g_vga_state.cursor.y);
}










