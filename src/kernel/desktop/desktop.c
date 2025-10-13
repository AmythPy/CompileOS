/**
 * CompileOS Desktop Environment - Implementation
 * 
 * Full desktop with window management, taskbar, and terminal integration
 */

#include "desktop.h"
#include "../../drivers/vga/vga.h"
#include "../../drivers/keyboard/keyboard.h"
#include "../terminal/terminal.h"
#include "../memory/memory.h"
#include <string.h>

extern int sprintf(char* str, const char* format, ...);

// Desktop state
static desktop_t g_desktop = {0};

// Terminal window content buffer
#define TERMINAL_WINDOW_LINES 18
#define TERMINAL_WINDOW_COLS 74
static char terminal_buffer[TERMINAL_WINDOW_LINES][TERMINAL_WINDOW_COLS];
static int terminal_line = 0;
static int terminal_col = 0;

// Desktop initialization
int desktop_init(void) {
    if (g_desktop.running) {
        return 0;
    }
    
    // Initialize VGA
    if (vga_init() != 0) {
        return -1;
    }
    
    // Set up desktop
    g_desktop.mode = DESKTOP_MODE_TEXT;
    g_desktop.width = 80;
    g_desktop.height = 25;
    g_desktop.windows = NULL;
    g_desktop.focused_window = NULL;
    g_desktop.next_window_id = 1;
    g_desktop.running = false;
    
    return 0;
}

/**
 * Shutdown desktop
 */
void desktop_shutdown(void) {
    if (!g_desktop.running) {
        return;
    }
    
    // Destroy all windows
    window_t* window = g_desktop.windows;
    while (window) {
        window_t* next = window->next;
        desktop_destroy_window(window);
        window = next;
    }
    
    g_desktop.running = false;
}

/**
 * Start desktop
 */
int desktop_start(void) {
    if (g_desktop.running) {
        return 0;
    }
    
    g_desktop.running = true;
    
    // Clear screen
    vga_clear_screen();
    
    // Initialize terminal buffer
    memset(terminal_buffer, 0, sizeof(terminal_buffer));
    terminal_line = 0;
    terminal_col = 0;
    
    // Draw desktop background
    desktop_draw_background();
    
    // Create main terminal window
    desktop_create_window("CompileOS Terminal - Interactive Shell", 1, 1, 78, 21);
    
    // Create status bar at bottom
    desktop_create_window("Status Bar", 0, 23, 80, 2);
    
    // Draw initial desktop
    desktop_render();
    
    return 0;
}

/**
 * Stop desktop
 */
void desktop_stop(void) {
    g_desktop.running = false;
}

/**
 * Check if desktop is running
 */
bool desktop_is_running(void) {
    return g_desktop.running;
}

/**
 * Create window
 */
window_t* desktop_create_window(const char* title, uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
    if (!g_desktop.running || !title) {
        return NULL;
    }
    
    // Allocate window structure
    window_t* window = (window_t*)memory_alloc(sizeof(window_t));
    if (!window) {
        return NULL;
    }
    
    // Initialize window
    window->id = g_desktop.next_window_id++;
    strncpy(window->title, title, sizeof(window->title) - 1);
    window->title[sizeof(window->title) - 1] = '\0';
    window->x = x;
    window->y = y;
    window->width = width;
    window->height = height;
    window->state = WINDOW_STATE_NORMAL;
    window->type = WINDOW_TYPE_APPLICATION;
    window->visible = true;
    window->focused = false;
    window->content = NULL;
    window->next = NULL;
    window->prev = NULL;
    
    // Add to window list
    if (!g_desktop.windows) {
        g_desktop.windows = window;
    } else {
        window_t* last = g_desktop.windows;
        while (last->next) {
            last = last->next;
        }
        last->next = window;
        window->prev = last;
    }
    
    // Focus window
    desktop_focus_window(window);
    
    return window;
}

/**
 * Destroy window
 */
int desktop_destroy_window(window_t* window) {
    if (!window) {
        return -1;
    }
    
    // Remove from window list
    if (window->prev) {
        window->prev->next = window->next;
    } else {
        g_desktop.windows = window->next;
    }
    
    if (window->next) {
        window->next->prev = window->prev;
    }
    
    // Clear window area
    if (window->visible) {
        for (uint16_t y = window->y; y < window->y + window->height; y++) {
            for (uint16_t x = window->x; x < window->x + window->width; x++) {
                vga_putchar_at(x, y, ' ');
            }
        }
    }
    
    // Free window
    memory_free(window);
    
    return 0;
}

/**
 * Show window
 */
int desktop_show_window(window_t* window) {
    if (!window) {
        return -1;
    }
    
    window->visible = true;
    desktop_render();
    
    return 0;
}

/**
 * Hide window
 */
int desktop_hide_window(window_t* window) {
    if (!window) {
        return -1;
    }
    
    window->visible = false;
    desktop_render();
    
    return 0;
}

/**
 * Focus window
 */
int desktop_focus_window(window_t* window) {
    if (!window) {
        return -1;
    }
    
    // Unfocus current window
    if (g_desktop.focused_window) {
        g_desktop.focused_window->focused = false;
    }
    
    // Focus new window
    window->focused = true;
    g_desktop.focused_window = window;
    
    return 0;
}

/**
 * Move window
 */
int desktop_move_window(window_t* window, uint16_t x, uint16_t y) {
    if (!window) {
        return -1;
    }
    
    window->x = x;
    window->y = y;
    desktop_render();
    
    return 0;
}

/**
 * Resize window
 */
int desktop_resize_window(window_t* window, uint16_t width, uint16_t height) {
    if (!window) {
        return -1;
    }
    
    window->width = width;
    window->height = height;
    desktop_render();
    
    return 0;
}

/**
 * Get window by ID
 */
window_t* desktop_get_window_by_id(uint32_t id) {
    window_t* window = g_desktop.windows;
    while (window) {
        if (window->id == id) {
            return window;
        }
        window = window->next;
    }
    return NULL;
}

/**
 * Get focused window
 */
window_t* desktop_get_focused_window(void) {
    return g_desktop.focused_window;
}

/**
 * Render desktop
 */
int desktop_render(void) {
    if (!g_desktop.running) {
        return -1;
    }
    
    // Clear screen
    vga_clear_screen();
    
    // Draw background
    desktop_draw_background();
    
    // Draw windows
    desktop_draw_windows();
    
    // Draw cursor
    desktop_draw_cursor();
    
    return 0;
}

/**
 * Draw background
 */
int desktop_draw_background(void) {
    if (!g_desktop.running) {
        return -1;
    }
    
    // Clear screen with direct VGA writes
    volatile uint16_t* vga_buffer = (uint16_t*)0xB8000;
    
    // Fill with cyan background (desktop color)
    for (int i = 0; i < 80 * 25; i++) {
        vga_buffer[i] = 0x3020;  // Black on cyan, space character
    }
    
    // Draw title bar at top (line 0)
    for (int i = 0; i < 80; i++) {
        vga_buffer[i] = 0x1F20;  // White on blue
    }
    
    const char* title = " CompileOS v0.1.0 - Desktop Environment";
    for (int i = 0; title[i] && i < 40; i++) {
        vga_buffer[i] = (uint16_t)title[i] | 0x1F00;
    }
    
    // Draw system tray on right
    const char* tray = "[12:00] [MEM:512MB] [X]";
    int tray_start = 80 - 24;
    for (int i = 0; tray[i]; i++) {
        vga_buffer[tray_start + i] = (uint16_t)tray[i] | 0x1F00;
    }
    
    // Draw taskbar at bottom (line 24)
    for (int i = 80 * 24; i < 80 * 25; i++) {
        vga_buffer[i] = 0x70 | ' ';  // Black on light gray
    }
    
    const char* taskbar = " [Start] Terminal | REPL | Files | Settings";
    for (int i = 0; taskbar[i] && i < 80; i++) {
        vga_buffer[80 * 24 + i] = (uint16_t)taskbar[i] | 0x70;
    }
    
    return 0;
}

/**
 * Draw windows
 */
int desktop_draw_windows(void) {
    if (!g_desktop.running) {
        return -1;
    }
    
    volatile uint16_t* vga = (uint16_t*)0xB8000;
    
    window_t* window = g_desktop.windows;
    while (window) {
        if (window->visible) {
            // Draw window with direct VGA access for speed
            uint16_t border_color = (window == g_desktop.focused_window) ? 0x0E00 : 0x0700;  // Yellow or gray
            uint16_t title_color = 0x1F00;  // White on blue
            uint16_t content_color = 0x0F00;  // White on black
            
            // Top border with title bar
            for (int x = window->x; x < window->x + window->width; x++) {
                vga[window->y * 80 + x] = ' ' | title_color;
            }
            
            // Draw title
            const char* title = window->title;
            int title_x = window->x + 2;
            for (int i = 0; title[i] && i < window->width - 4; i++) {
                vga[window->y * 80 + title_x + i] = title[i] | title_color;
            }
            
            // Draw close button
            vga[window->y * 80 + window->x + window->width - 3] = '[' | 0x4F00;
            vga[window->y * 80 + window->x + window->width - 2] = 'X' | 0x4F00;
            vga[window->y * 80 + window->x + window->width - 1] = ']' | 0x4F00;
            
            // Draw window content area
            for (int y = window->y + 1; y < window->y + window->height - 1; y++) {
                // Left border
                vga[y * 80 + window->x] = 0xB3 | border_color;  // │
                
                // Content area
                for (int x = window->x + 1; x < window->x + window->width - 1; x++) {
                    vga[y * 80 + x] = ' ' | content_color;
                }
                
                // Right border
                vga[y * 80 + window->x + window->width - 1] = 0xB3 | border_color;  // │
            }
            
            // Bottom border
            vga[(window->y + window->height - 1) * 80 + window->x] = 0xC0 | border_color;  // └
            for (int x = window->x + 1; x < window->x + window->width - 1; x++) {
                vga[(window->y + window->height - 1) * 80 + x] = 0xC4 | border_color;  // ─
            }
            vga[(window->y + window->height - 1) * 80 + window->x + window->width - 1] = 0xD9 | border_color;  // ┘
            
            // Draw window content
            if (window->id == 1) { // Main terminal window
                int content_y = window->y + 2;
                int content_x = window->x + 2;
                
                // Welcome header
                const char* line1 = "CompileOS Interactive Terminal";
                for (int i = 0; line1[i]; i++) {
                    vga[content_y * 80 + content_x + i] = line1[i] | 0x0B00;  // Cyan
                }
                content_y++;
                
                const char* line2 = "================================";
                for (int i = 0; line2[i]; i++) {
                    vga[content_y * 80 + content_x + i] = line2[i] | 0x0B00;
                }
                content_y += 2;
                
                // Features
                const char* line3 = "Available Commands:";
                for (int i = 0; line3[i]; i++) {
                    vga[content_y * 80 + content_x + i] = line3[i] | 0x0E00;  // Yellow
                }
                content_y++;
                
                const char* line4 = "  help    - Show all commands";
                for (int i = 0; line4[i]; i++) {
                    vga[content_y * 80 + content_x + i] = line4[i] | 0x0F00;
                }
                content_y++;
                
                const char* line5 = "  clear   - Clear screen";
                for (int i = 0; line5[i]; i++) {
                    vga[content_y * 80 + content_x + i] = line5[i] | 0x0F00;
                }
                content_y++;
                
                const char* line6 = "  version - OS version";
                for (int i = 0; line6[i]; i++) {
                    vga[content_y * 80 + content_x + i] = line6[i] | 0x0F00;
                }
                content_y++;
                
                const char* line7 = "  memory  - Memory info";
                for (int i = 0; line7[i]; i++) {
                    vga[content_y * 80 + content_x + i] = line7[i] | 0x0F00;
                }
                content_y++;
                
                const char* line8 = "  repl    - Start calculator";
                for (int i = 0; line8[i]; i++) {
                    vga[content_y * 80 + content_x + i] = line8[i] | 0x0F00;
                }
                content_y += 2;
                
                // System info
                const char* line9 = "System Status:";
                for (int i = 0; line9[i]; i++) {
                    vga[content_y * 80 + content_x + i] = line9[i] | 0x0E00;
                }
                content_y++;
                
                const char* line10 = "  Architecture: x86_64";
                for (int i = 0; line10[i]; i++) {
                    vga[content_y * 80 + content_x + i] = line10[i] | 0x0700;
                }
                content_y++;
                
                const char* line11 = "  Memory: 512 MB";
                for (int i = 0; line11[i]; i++) {
                    vga[content_y * 80 + content_x + i] = line11[i] | 0x0700;
                }
                content_y++;
                
                const char* line12 = "  Status: READY";
                for (int i = 0; line12[i]; i++) {
                    vga[content_y * 80 + content_x + i] = line12[i] | 0x0A00;  // Green
                }
                content_y += 2;
                
                // Prompt
                const char* prompt = "CompileOS> _";
                for (int i = 0; prompt[i]; i++) {
                    vga[content_y * 80 + content_x + i] = prompt[i] | 0x0F00;
                }
            } else if (window->id == 2) { // Status bar - already drawn in background
                vga_puts_at(window->x + 4, window->y + 5, "* PS/2 Keyboard Input");
                vga_puts_at(window->x + 4, window->y + 6, "* Interactive Terminal Shell");
                vga_puts_at(window->x + 4, window->y + 7, "* REPL Expression Evaluator");
                vga_puts_at(window->x + 4, window->y + 8, "* In-Memory Filesystem");
                vga_puts_at(window->x + 4, window->y + 9, "* Window Management");
                
                vga_set_foreground_color(VGA_COLOR_YELLOW);
                vga_puts_at(window->x + 2, window->y + 11, "Type 'help' for available commands");
                
                vga_set_foreground_color(VGA_COLOR_WHITE);
                vga_puts_at(window->x + 2, window->y + 13, "System Info:");
                vga_set_foreground_color(VGA_COLOR_LIGHT_GRAY);
                vga_puts_at(window->x + 4, window->y + 14, "Architecture: x86_64");
                vga_puts_at(window->x + 4, window->y + 15, "Memory: 16/32/64-bit support");
                vga_puts_at(window->x + 4, window->y + 16, "Status: Ready");
                
                vga_set_foreground_color(VGA_COLOR_LIGHT_CYAN);
                vga_puts_at(window->x + 2, window->y + 18, "CompileOS>");
                
            } else if (window->id == 2) { // Status bar
                // Clear status bar
                for (uint16_t x = window->x + 1; x < window->x + window->width - 1; x++) {
                    vga_putchar_at(x, window->y + 1, ' ');
                }
                
                // Draw status information
                vga_set_foreground_color(VGA_COLOR_BLACK);
                vga_set_background_color(VGA_COLOR_LIGHT_GRAY);
                
                char status[80];
                sprintf(status, " F1-Help | F2-Files | F3-REPL | F4-Memory | F5-Debug | F10-Exit ");
                vga_puts_at(window->x + 1, window->y + 1, status);
                
                // Right side status
                vga_set_foreground_color(VGA_COLOR_WHITE);
                vga_set_background_color(VGA_COLOR_BLUE);
                vga_puts_at(window->x + window->width - 15, window->y + 1, "Status: Ready");
            }
        }
        
        window = window->next;
    }
    
    return 0;
}

/**
 * Draw cursor
 */
int desktop_draw_cursor(void) {
    if (!g_desktop.running) {
        return -1;
    }
    
    // Position cursor in terminal window
    if (g_desktop.focused_window && g_desktop.focused_window->id == 1) {
        vga_set_cursor(g_desktop.focused_window->x + 2 + terminal_col, 
                      g_desktop.focused_window->y + 18);
    }
    
    return 0;
}

/**
 * Update desktop (call from main loop)
 */
void desktop_update(void) {
    if (!g_desktop.running) {
        return;
    }
    
    // Check for keyboard input
    if (keyboard_has_input()) {
        char c;
        if (keyboard_getchar_nonblocking(&c) == 0) {
            desktop_handle_keyboard_input(c);
        }
    }
}

/**
 * Handle keyboard input in desktop
 */
void desktop_handle_keyboard_input(char c) {
    if (!g_desktop.running) {
        return;
    }
    
    // Pass to terminal for now
    terminal_handle_input(c);
    
    // Refresh display
    desktop_render();
}

/**
 * Handle mouse input
 */
int desktop_handle_mouse(uint16_t x, uint16_t y, uint8_t buttons) {
    if (!g_desktop.running) {
        return -1;
    }
    
    (void)buttons; // Suppress unused parameter warning
    
    // Simple mouse handling - focus window under cursor
    window_t* window = g_desktop.windows;
    while (window) {
        if (window->visible && 
            x >= window->x && x < window->x + window->width &&
            y >= window->y && y < window->y + window->height) {
            desktop_focus_window(window);
            break;
        }
        window = window->next;
    }
    
    return 0;
}

/**
 * Handle keyboard input
 */
int desktop_handle_keyboard(uint16_t key_code, bool pressed) {
    if (!g_desktop.running) {
        return -1;
    }
    
    // Simple keyboard handling
    if (pressed) {
        switch (key_code) {
            case 0x01: // Escape
                desktop_stop();
                break;
            case 0x02: // 1
                desktop_focus_window(desktop_get_window_by_id(1));
                break;
            case 0x03: // 2
                desktop_focus_window(desktop_get_window_by_id(2));
                break;
            default:
                break;
        }
    }
    
    return 0;
}

/**
 * Handle click
 */
int desktop_handle_click(uint16_t x, uint16_t y, uint8_t button) {
    return desktop_handle_mouse(x, y, button);
}

/**
 * Set theme
 */
int desktop_set_theme(const desktop_theme_t* theme) {
    // TODO: Implement theme support
    (void)theme;
    return 0;
}

/**
 * Get theme
 */
int desktop_get_theme(desktop_theme_t* theme) {
    // TODO: Implement theme support
    (void)theme;
    return 0;
}

/**
 * Register application
 */
int desktop_register_app(const desktop_app_t* app) {
    // TODO: Implement application registration
    (void)app;
    return 0;
}

/**
 * Unregister application
 */
int desktop_unregister_app(const char* name) {
    // TODO: Implement application unregistration
    (void)name;
    return 0;
}

/**
 * Launch application
 */
int desktop_launch_app(const char* name) {
    // TODO: Implement application launching
    (void)name;
    return 0;
}

/**
 * List applications
 */
int desktop_list_apps(char* output, size_t output_size) {
    // TODO: Implement application listing
    (void)output;
    (void)output_size;
    return 0;
}

/**
 * Terminal application
 */
int desktop_app_terminal(void) {
    // TODO: Implement terminal application
    return 0;
}

/**
 * File manager application
 */
int desktop_app_file_manager(void) {
    // TODO: Implement file manager application
    return 0;
}

/**
 * Memory viewer application
 */
int desktop_app_memory_viewer(void) {
    // TODO: Implement memory viewer application
    return 0;
}

/**
 * Debugger application
 */
int desktop_app_debugger(void) {
    // TODO: Implement debugger application
    return 0;
}

/**
 * REPL application
 */
int desktop_app_repl(void) {
    // TODO: Implement REPL application
    return 0;
}










