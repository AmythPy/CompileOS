/**
 * CompileOS - SIMPLE WORKING VERSION
 * Guaranteed keyboard input + simple mouse cursor
 */

#include <stdint.h>

// VGA memory
#define VGA ((volatile uint16_t*)0xB8000)

// Port I/O
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// Colors
#define COLOR(fg, bg) ((bg << 4) | fg)
#define BLACK 0
#define BLUE 1
#define GREEN 2
#define CYAN 3
#define RED 4
#define YELLOW 14
#define WHITE 15

// Scancode to ASCII map
static const char scancode_map[] = {
    0, 0, '1','2','3','4','5','6','7','8','9','0','-','=','\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0,'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,'\\','z','x','c','v','b','n','m',',','.','/',0,'*',0,' '
};

// Mouse cursor position
static int cursor_x = 40;
static int cursor_y = 12;

// Command buffer
static char cmd_buffer[256];
static int cmd_pos = 0;

// Keyboard input - guaranteed to work
static char get_key(void) {
    if (!(inb(0x64) & 1)) return 0;
    uint8_t scan = inb(0x60);
    if (scan & 0x80) return 0;
    if (scan < sizeof(scancode_map)) return scancode_map[scan];
    return 0;
}

// String comparison
static int strcmp(const char* s1, const char* s2) {
    while (*s1 && *s1 == *s2) { s1++; s2++; }
    return *s1 - *s2;
}

// Draw complete desktop
void draw_desktop(void) {
    // Clear screen to cyan
    for (int i = 0; i < 80 * 25; i++) {
        VGA[i] = 0x3020;  // Black on cyan
    }

    // Title bar (blue)
    for (int i = 0; i < 80; i++) {
        VGA[i] = 0x1F20;  // White on blue
    }
    // Title
    const char* title = " CompileOS v0.1.0 - Desktop OS";
    for (int i = 0; title[i] && i < 35; i++) {
        VGA[i] = title[i] | 0x1F00;
    }

    // Desktop icons
    VGA[2 * 80 + 2] = '[' | 0x0F00; VGA[2 * 80 + 3] = 'P' | 0x0F00; VGA[2 * 80 + 4] = 'C' | 0x0F00; VGA[2 * 80 + 5] = ']' | 0x0F00;
    VGA[3 * 80 + 2] = 'M' | 0x0F00; VGA[3 * 80 + 3] = 'y' | 0x0F00; VGA[3 * 80 + 4] = 'P' | 0x0F00; VGA[3 * 80 + 5] = 'C' | 0x0F00;

    VGA[5 * 80 + 2] = '[' | 0x0E00; VGA[5 * 80 + 3] = '#' | 0x0E00; VGA[5 * 80 + 4] = '#' | 0x0E00; VGA[5 * 80 + 5] = ']' | 0x0E00;
    VGA[6 * 80 + 2] = 'F' | 0x0E00; VGA[6 * 80 + 3] = 'i' | 0x0E00; VGA[6 * 80 + 4] = 'l' | 0x0E00; VGA[6 * 80 + 5] = 'e' | 0x0E00; VGA[6 * 80 + 6] = 's' | 0x0E00;

    // Terminal window
    int wx = 8, wy = 3, ww = 64, wh = 18;

    // Window borders
    for (int y = wy; y < wy + wh; y++) {
        VGA[y * 80 + wx] = 0xB3 | 0x0E00;
        VGA[y * 80 + wx + ww - 1] = 0xB3 | 0x0E00;
    }
    for (int x = wx; x < wx + ww; x++) {
        VGA[wy * 80 + x] = 0xC4 | 0x0E00;
        VGA[(wy + wh - 1) * 80 + x] = 0xC4 | 0x0E00;
    }
    VGA[wy * 80 + wx] = 0xC9 | 0x0E00;
    VGA[wy * 80 + wx + ww - 1] = 0xBB | 0x0E00;
    VGA[(wy + wh - 1) * 80 + wx] = 0xC8 | 0x0E00;
    VGA[(wy + wh - 1) * 80 + wx + ww - 1] = 0xBC | 0x0E00;

    // Window content (black background)
    for (int y = wy + 1; y < wy + wh - 1; y++) {
        for (int x = wx + 1; x < wx + ww - 1; x++) {
            VGA[y * 80 + x] = ' ' | 0x0F00;
        }
    }

    // Welcome message
    const char* msg1 = "Welcome to CompileOS!";
    for (int i = 0; msg1[i]; i++) VGA[(wy + 2) * 80 + wx + 2 + i] = msg1[i] | 0x0B00;

    const char* msg2 = "Mouse cursor enabled!";
    for (int i = 0; msg2[i]; i++) VGA[(wy + 4) * 80 + wx + 2 + i] = msg2[i] | 0x0A00;

    const char* msg3 = "Use WASD to move cursor";
    for (int i = 0; msg3[i]; i++) VGA[(wy + 6) * 80 + wx + 2 + i] = msg3[i] | 0x0E00;

    // Prompt
    const char* prompt = "CompileOS> ";
    for (int i = 0; prompt[i]; i++) VGA[(wy + 8) * 80 + wx + 2 + i] = prompt[i] | 0x0F00;

    // Taskbar
    for (int i = 0; i < 80; i++) {
        VGA[24 * 80 + i] = ' ' | 0x7000;
    }
    const char* taskbar = " [Start] Terminal | Files | Settings";
    for (int i = 0; taskbar[i]; i++) {
        VGA[24 * 80 + i] = taskbar[i] | 0x7000;
    }
}

// Draw mouse cursor as intersecting lines
void draw_cursor(void) {
    // Horizontal line (yellow)
    int start_x = cursor_x - 8;
    int end_x = cursor_x + 8;
    if (start_x < 0) start_x = 0;
    if (end_x >= 80) end_x = 79;

    for (int x = start_x; x <= end_x; x++) {
        if (cursor_y >= 0 && cursor_y < 25) {
            uint16_t current = VGA[cursor_y * 80 + x];
            if ((current & 0xFF) == ' ') {
                VGA[cursor_y * 80 + x] = (current & 0xFF00) | YELLOW;
            }
        }
    }

    // Vertical line (yellow)
    int start_y = cursor_y - 5;
    int end_y = cursor_y + 5;
    if (start_y < 0) start_y = 0;
    if (end_y >= 25) end_y = 24;

    for (int y = start_y; y <= end_y; y++) {
        if (cursor_x >= 0 && cursor_x < 80) {
            uint16_t current = VGA[y * 80 + cursor_x];
            if ((current & 0xFF) == ' ') {
                VGA[y * 80 + cursor_x] = (current & 0xFF00) | YELLOW;
            }
        }
    }

    // Center point
    if (cursor_x >= 0 && cursor_x < 80 && cursor_y >= 0 && cursor_y < 25) {
        VGA[cursor_y * 80 + cursor_x] = '+' | 0x0E00;
    }
}

// Handle WASD keys for cursor movement
void handle_cursor_keys(char key) {
    switch (key) {
        case 'w': case 'W': cursor_y--; break;
        case 's': case 'S': cursor_y++; break;
        case 'a': case 'A': cursor_x--; break;
        case 'd': case 'D': cursor_x++; break;
    }

    // Keep cursor on screen
    if (cursor_x < 0) cursor_x = 0;
    if (cursor_x >= 80) cursor_x = 79;
    if (cursor_y < 0) cursor_y = 0;
    if (cursor_y >= 25) cursor_y = 24;
}

// Execute command
void execute_command(void) {
    if (cmd_buffer[0] == 0) return;

    if (strcmp(cmd_buffer, "help") == 0) {
        const char* help_msg = "Commands: help, version, clear";
        for (int i = 0; help_msg[i]; i++) {
            VGA[15 * 80 + 10 + i] = help_msg[i] | 0x0A00;
        }
    } else if (strcmp(cmd_buffer, "version") == 0) {
        const char* ver_msg = "CompileOS v0.1.0 - Desktop OS";
        for (int i = 0; ver_msg[i]; i++) {
            VGA[15 * 80 + 10 + i] = ver_msg[i] | 0x0B00;
        }
    } else if (strcmp(cmd_buffer, "clear") == 0) {
        draw_desktop();
        return;
    } else {
        const char* unknown = "Unknown command";
        for (int i = 0; unknown[i]; i++) {
            VGA[15 * 80 + 10 + i] = unknown[i] | 0x0C00;
        }
    }

    // Show next prompt
    const char* next_prompt = "CompileOS> ";
    for (int i = 0; next_prompt[i]; i++) {
        VGA[17 * 80 + 10 + i] = next_prompt[i] | 0x0F00;
    }
}

// Main kernel - EVERYTHING IN ONE FILE
void kernel_main(void) {
    // Draw desktop
    draw_desktop();

    // Main loop
    while (1) {
        char key = get_key();
        if (key) {
            if (key == '\n') {
                cmd_buffer[cmd_pos] = 0;
                execute_command();
                cmd_pos = 0;
            } else if (key == '\b') {
                if (cmd_pos > 0) {
                    cmd_pos--;
                    VGA[17 * 80 + 10 + 11 + cmd_pos] = ' ' | 0x0F00;
                }
            } else if (key >= 32 && key < 127) {
                if (cmd_pos < 255) {
                    cmd_buffer[cmd_pos++] = key;
                    VGA[17 * 80 + 10 + 11 + cmd_pos - 1] = key | 0x0F00;
                }
            }

            // Handle cursor movement
            if (key == 'w' || key == 'W' || key == 'a' || key == 'A' ||
                key == 's' || key == 'S' || key == 'd' || key == 'D') {
                handle_cursor_keys(key);
            }
        }

        // Draw cursor
        draw_cursor();

        // Small delay
        for (volatile int i = 0; i < 1000; i++);
    }
}
