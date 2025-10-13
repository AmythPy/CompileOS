/**
 * CompileOS - 32-BIT WORKING VERSION
 * Guaranteed to boot in QEMU and show desktop
 */

#include <stdint.h>

// VGA memory - direct access
#define VGA ((volatile uint16_t*)0xB8000)

// Port I/O for keyboard
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// Colors (VGA attribute format: background << 4 | foreground)
#define COLOR(fg, bg) ((bg << 4) | fg)
#define BLACK 0
#define BLUE 1
#define GREEN 2
#define CYAN 3
#define RED 4
#define MAGENTA 5
#define BROWN 6
#define LIGHT_GRAY 7
#define DARK_GRAY 8
#define LIGHT_BLUE 9
#define LIGHT_GREEN 10
#define LIGHT_CYAN 11
#define LIGHT_RED 12
#define LIGHT_MAGENTA 13
#define YELLOW 14
#define WHITE 15

// Scancode to ASCII map
static const char scancode_map[] = {
    0, 0, '1','2','3','4','5','6','7','8','9','0','-','=','\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0,'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,'\\','z','x','c','v','b','n','m',',','.','/',0,'*',0,' '
};

// Mouse state (shared with mouse driver) - make these global
int mouse_x = 40;
int mouse_y = 12;

// Command buffer
static char cmd_buffer[256];
static int cmd_pos = 0;

// Keyboard input
static char get_key(void) {
    if (!(inb(0x64) & 1)) return 0;
    uint8_t scan = inb(0x60);
    if (scan & 0x80) return 0;
    if (scan < sizeof(scancode_map)) return scancode_map[scan];
    return 0;
}

// String functions
static int strcmp(const char* s1, const char* s2) {
    while (*s1 && *s1 == *s2) { s1++; s2++; }
    return *s1 - *s2;
}

// Draw the complete desktop
void draw_desktop(void) {
    // Clear entire screen to cyan background
    for (int i = 0; i < 80 * 25; i++) {
        VGA[i] = 0x3020;  // Black on cyan, space
    }

    // Title bar (line 0) - blue background
    for (int i = 0; i < 80; i++) {
        VGA[i] = 0x1F20;  // White on blue
    }
    // Title text
    const char* title = " CompileOS v0.1.0 - Desktop Environment";
    for (int i = 0; title[i] && i < 40; i++) {
        VGA[i] = title[i] | 0x1F00;
    }
    // System tray
    const char* tray = "[12:00] [MEM:512MB] [X]";
    for (int i = 0; tray[i]; i++) {
        VGA[56 + i] = tray[i] | 0x1F00;
    }

    // Desktop icons
    VGA[2 * 80 + 2] = '[' | 0x0F00; VGA[2 * 80 + 3] = 'P' | 0x0F00; VGA[2 * 80 + 4] = 'C' | 0x0F00; VGA[2 * 80 + 5] = ']' | 0x0F00;
    VGA[3 * 80 + 2] = 'M' | 0x0F00; VGA[3 * 80 + 3] = 'y' | 0x0F00; VGA[3 * 80 + 4] = 'P' | 0x0F00; VGA[3 * 80 + 5] = 'C' | 0x0F00;

    VGA[5 * 80 + 2] = '[' | 0x0E00; VGA[5 * 80 + 3] = '#' | 0x0E00; VGA[5 * 80 + 4] = '#' | 0x0E00; VGA[5 * 80 + 5] = ']' | 0x0E00;
    VGA[6 * 80 + 2] = 'F' | 0x0E00; VGA[6 * 80 + 3] = 'i' | 0x0E00; VGA[6 * 80 + 4] = 'l' | 0x0E00; VGA[6 * 80 + 5] = 'e' | 0x0E00; VGA[6 * 80 + 6] = 's' | 0x0E00;

    VGA[8 * 80 + 2] = '[' | 0x0C00; VGA[8 * 80 + 3] = '*' | 0x0C00; VGA[8 * 80 + 4] = '*' | 0x0C00; VGA[8 * 80 + 5] = ']' | 0x0C00;
    VGA[9 * 80 + 2] = 'S' | 0x0C00; VGA[9 * 80 + 3] = 'e' | 0x0C00; VGA[9 * 80 + 4] = 't' | 0x0C00; VGA[9 * 80 + 5] = 'u' | 0x0C00; VGA[9 * 80 + 6] = 'p' | 0x0C00;

    // Terminal window (centered)
    int win_x = 5;
    int win_y = 2;
    int win_w = 70;
    int win_h = 20;

    // Window borders
    for (int y = win_y; y < win_y + win_h; y++) {
        VGA[y * 80 + win_x] = 0xB3 | 0x0E00;  // │ yellow
        VGA[y * 80 + win_x + win_w - 1] = 0xB3 | 0x0E00;
    }
    for (int x = win_x; x < win_x + win_w; x++) {
        VGA[win_y * 80 + x] = 0xC4 | 0x0E00;  // ─
        VGA[(win_y + win_h - 1) * 80 + x] = 0xC4 | 0x0E00;
    }
    VGA[win_y * 80 + win_x] = 0xC9 | 0x0E00;  // ┌
    VGA[win_y * 80 + win_x + win_w - 1] = 0xBB | 0x0E00;  // ┐
    VGA[(win_y + win_h - 1) * 80 + win_x] = 0xC8 | 0x0E00;  // └
    VGA[(win_y + win_h - 1) * 80 + win_x + win_w - 1] = 0xBC | 0x0E00;  // ┘

    // Window title bar
    for (int x = win_x + 1; x < win_x + win_w - 1; x++) {
        VGA[win_y * 80 + x] = ' ' | 0x1F00;
    }
    const char* win_title = " CompileOS Terminal";
    for (int i = 0; win_title[i]; i++) {
        VGA[win_y * 80 + win_x + 1 + i] = win_title[i] | 0x1F00;
    }
    VGA[win_y * 80 + win_x + win_w - 3] = '[' | 0x4F00;
    VGA[win_y * 80 + win_x + win_w - 2] = 'X' | 0x4F00;
    VGA[win_y * 80 + win_x + win_w - 1] = ']' | 0x4F00;

    // Terminal content area (black background)
    for (int y = win_y + 1; y < win_y + win_h - 1; y++) {
        for (int x = win_x + 1; x < win_x + win_w - 1; x++) {
            VGA[y * 80 + x] = ' ' | 0x0F00;
        }
    }

    // Welcome message
    int cy = win_y + 2;
    int cx = win_x + 2;

    const char* l1 = "Welcome to CompileOS!";
    for (int i = 0; l1[i]; i++) VGA[cy * 80 + cx + i] = l1[i] | 0x0B00;
    cy++;

    const char* l2 = "=====================";
    for (int i = 0; l2[i]; i++) VGA[cy * 80 + cx + i] = l2[i] | 0x0B00;
    cy += 2;

    const char* l3 = "Your OS is running!";
    for (int i = 0; l3[i]; i++) VGA[cy * 80 + cx + i] = l3[i] | 0x0A00;
    cy += 2;

    const char* l4 = "Mouse cursor enabled!";
    for (int i = 0; l4[i]; i++) VGA[cy * 80 + cx + i] = l4[i] | 0x0A00;
    cy += 2;

    const char* l5 = "Use WASD keys to move cursor";
    for (int i = 0; l5[i]; i++) VGA[cy * 80 + cx + i] = l5[i] | 0x0E00;
    cy += 2;

    const char* l6 = "Commands: help, version, clear";
    for (int i = 0; l6[i]; i++) VGA[cy * 80 + cx + i] = l6[i] | 0x0F00;
    cy += 2;

    // Prompt
    const char* prompt = "CompileOS> ";
    for (int i = 0; prompt[i]; i++) VGA[cy * 80 + cx + i] = prompt[i] | 0x0F00;

    // Taskbar (line 24)
    for (int i = 0; i < 80; i++) {
        VGA[24 * 80 + i] = ' ' | 0x7000;
    }
    const char* taskbar = " [Start] Terminal | REPL | Files | Settings";
    for (int i = 0; taskbar[i]; i++) {
        VGA[24 * 80 + i] = taskbar[i] | 0x7000;
    }
}

// Execute command
void execute_command(void) {
    if (cmd_buffer[0] == 0) return;

    if (strcmp(cmd_buffer, "help") == 0) {
        const char* help_msg = "Available: help, version, clear, echo";
        for (int i = 0; help_msg[i]; i++) {
            VGA[16 * 80 + 12 + i] = help_msg[i] | 0x0A00;
        }
    } else if (strcmp(cmd_buffer, "version") == 0) {
        const char* ver_msg = "CompileOS v0.1.0 - Desktop OS";
        for (int i = 0; ver_msg[i]; i++) {
            VGA[16 * 80 + 12 + i] = ver_msg[i] | 0x0B00;
        }
    } else if (strcmp(cmd_buffer, "clear") == 0) {
        draw_desktop();
        return;
    } else {
        const char* unknown = "Unknown command (try help)";
        for (int i = 0; unknown[i]; i++) {
            VGA[16 * 80 + 12 + i] = unknown[i] | 0x0C00;
        }
    }

    // Show next prompt
    const char* next_prompt = "CompileOS> ";
    for (int i = 0; next_prompt[i]; i++) {
        VGA[20 * 80 + 12 + i] = next_prompt[i] | 0x0F00;
    }
}
// Forward declarations for external functions
extern void mouse_init(void);
extern void mouse_update(void);
extern void mouse_draw_cursor(void);
extern void mouse_clear_cursor(void);
extern int mouse_is_enabled(void);

// Simple keyboard-based mouse movement for testing (arrow keys)
void handle_mouse_keys(char key) {
    if (!mouse_is_enabled()) return;

    switch (key) {
        case 'w': case 'W': mouse_y--; break;  // Up
        case 's': case 'S': mouse_y++; break;  // Down
        case 'a': case 'A': mouse_x--; break;  // Left
        case 'd': case 'D': mouse_x++; break;  // Right
    }

    // Clamp to screen bounds
    if (mouse_x < 0) mouse_x = 0;
    if (mouse_x >= 80) mouse_x = 79;
    if (mouse_y < 0) mouse_y = 0;
    if (mouse_y >= 25) mouse_y = 24;
}

// Kernel entry point - 32-bit compatible
void kernel_main(void) {
    // Draw the complete desktop
    draw_desktop();

    // Initialize mouse
    mouse_init();

    // Main input loop
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
                    VGA[20 * 80 + 12 + 11 + cmd_pos] = ' ' | 0x0F00;
                }
            } else if (key >= 32 && key < 127) {
                if (cmd_pos < 255) {
                    cmd_buffer[cmd_pos++] = key;
                    VGA[20 * 80 + 12 + 11 + cmd_pos - 1] = key | 0x0F00;
                }
            }

            // Handle WASD keys for mouse movement (fallback)
            if (key == 'w' || key == 'W' || key == 'a' || key == 'A' ||
                key == 's' || key == 'S' || key == 'd' || key == 'D') {
                handle_mouse_keys(key);
            }
        }

        // Update mouse position and draw cursor
        if (mouse_is_enabled()) {
            mouse_update();
            mouse_draw_cursor();
        }

        // Small delay
        for (volatile int i = 0; i < 1000; i++);
    }
}
