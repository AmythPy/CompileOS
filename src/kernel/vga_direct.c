/**
 * Direct VGA Driver - GUARANTEED TO WORK
 * Bypasses all broken abstractions
 */

#include <stdint.h>
#include <stdbool.h>

#define VGA_MEMORY 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

static volatile uint8_t* vga = (volatile uint8_t*)VGA_MEMORY;
static int cursor_x = 0;
static int cursor_y = 0;

// Clear screen
void vga_direct_clear(uint8_t color) {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT * 2; i += 2) {
        vga[i] = ' ';
        vga[i + 1] = color;
    }
    cursor_x = 0;
    cursor_y = 0;
}

// Put character at position
void vga_direct_putchar_at(int x, int y, char c, uint8_t color) {
    if (x >= 0 && x < VGA_WIDTH && y >= 0 && y < VGA_HEIGHT) {
        int pos = (y * VGA_WIDTH + x) * 2;
        vga[pos] = c;
        vga[pos + 1] = color;
    }
}

// Put string at position
void vga_direct_puts_at(int x, int y, const char* str, uint8_t color) {
    int i = 0;
    while (str[i] && x + i < VGA_WIDTH) {
        vga_direct_putchar_at(x + i, y, str[i], color);
        i++;
    }
}

// Print character at cursor
void vga_direct_putchar(char c, uint8_t color) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\r') {
        cursor_x = 0;
    } else if (c == '\b') {
        if (cursor_x > 0) cursor_x--;
    } else {
        vga_direct_putchar_at(cursor_x, cursor_y, c, color);
        cursor_x++;
    }
    
    if (cursor_x >= VGA_WIDTH) {
        cursor_x = 0;
        cursor_y++;
    }
    
    if (cursor_y >= VGA_HEIGHT) {
        // Scroll
        for (int i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH * 2; i++) {
            vga[i] = vga[i + VGA_WIDTH * 2];
        }
        for (int i = 0; i < VGA_WIDTH * 2; i += 2) {
            vga[(VGA_HEIGHT - 1) * VGA_WIDTH * 2 + i] = ' ';
            vga[(VGA_HEIGHT - 1) * VGA_WIDTH * 2 + i + 1] = 0x07;
        }
        cursor_y = VGA_HEIGHT - 1;
    }
}

// Print string at cursor
void vga_direct_puts(const char* str, uint8_t color) {
    for (int i = 0; str[i]; i++) {
        vga_direct_putchar(str[i], color);
    }
}

// Draw box
void vga_direct_box(int x, int y, int w, int h, uint8_t color) {
    // Top and bottom
    for (int i = 0; i < w; i++) {
        vga_direct_putchar_at(x + i, y, '-', color);
        vga_direct_putchar_at(x + i, y + h - 1, '-', color);
    }
    // Sides
    for (int i = 0; i < h; i++) {
        vga_direct_putchar_at(x, y + i, '|', color);
        vga_direct_putchar_at(x + w - 1, y + i, '|', color);
    }
    // Corners
    vga_direct_putchar_at(x, y, '+', color);
    vga_direct_putchar_at(x + w - 1, y, '+', color);
    vga_direct_putchar_at(x, y + h - 1, '+', color);
    vga_direct_putchar_at(x + w - 1, y + h - 1, '+', color);
}

// Fill rectangle
void vga_direct_fill(int x, int y, int w, int h, char c, uint8_t color) {
    for (int row = 0; row < h; row++) {
        for (int col = 0; col < w; col++) {
            vga_direct_putchar_at(x + col, y + row, c, color);
        }
    }
}

// Get cursor position
void vga_direct_get_cursor(int* x, int* y) {
    if (x) *x = cursor_x;
    if (y) *y = cursor_y;
}

// Set cursor position
void vga_direct_set_cursor(int x, int y) {
    if (x >= 0 && x < VGA_WIDTH) cursor_x = x;
    if (y >= 0 && y < VGA_HEIGHT) cursor_y = y;
}
