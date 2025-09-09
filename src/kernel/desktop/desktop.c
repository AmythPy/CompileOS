/**
 * CompileOS Desktop Environment - Implementation
 * 
 * Basic desktop with window management and GUI
 */

#include "desktop.h"
#include "../../drivers/vga/vga.h"
#include <string.h>

// Desktop state
static desktop_t g_desktop = {0};

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
    
    // Draw desktop
    desktop_draw_background();
    
    // Create default windows
    desktop_create_window("CompileOS Terminal", 2, 2, 76, 20);
    desktop_create_window("System Info", 2, 22, 76, 2);
    
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
    
    // Allocate window
    window_t* window = (window_t*)malloc(sizeof(window_t));
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
    free(window);
    
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
    
    // Draw desktop pattern
    vga_set_foreground_color(VGA_COLOR_BLUE);
    vga_set_background_color(VGA_COLOR_BLACK);
    
    for (uint16_t y = 0; y < g_desktop.height; y++) {
        for (uint16_t x = 0; x < g_desktop.width; x++) {
            if ((x + y) % 2 == 0) {
                vga_putchar_at(x, y, '.');
            } else {
                vga_putchar_at(x, y, ' ');
            }
        }
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
    
    window_t* window = g_desktop.windows;
    while (window) {
        if (window->visible) {
            // Draw window border
            vga_set_foreground_color(VGA_COLOR_WHITE);
            vga_set_background_color(VGA_COLOR_BLUE);
            
            // Top border
            vga_putchar_at(window->x, window->y, '+');
            for (uint16_t x = window->x + 1; x < window->x + window->width - 1; x++) {
                vga_putchar_at(x, window->y, '-');
            }
            vga_putchar_at(window->x + window->width - 1, window->y, '+');
            
            // Side borders
            for (uint16_t y = window->y + 1; y < window->y + window->height - 1; y++) {
                vga_putchar_at(window->x, y, '|');
                vga_putchar_at(window->x + window->width - 1, y, '|');
            }
            
            // Bottom border
            vga_putchar_at(window->x, window->y + window->height - 1, '+');
            for (uint16_t x = window->x + 1; x < window->x + window->width - 1; x++) {
                vga_putchar_at(x, window->y + window->height - 1, '-');
            }
            vga_putchar_at(window->x + window->width - 1, window->y + window->height - 1, '+');
            
            // Draw title
            vga_puts_at(window->x + 2, window->y, window->title);
            
            // Draw content
            vga_set_foreground_color(VGA_COLOR_WHITE);
            vga_set_background_color(VGA_COLOR_BLACK);
            
            if (window->id == 1) { // Terminal window
                vga_puts_at(window->x + 2, window->y + 2, "CompileOS v0.1.0 - Hardware Agnostic Development Platform");
                vga_puts_at(window->x + 2, window->y + 3, "Architecture: x86_64");
                vga_puts_at(window->x + 2, window->y + 4, "Memory: 16/32/64-bit support");
                vga_puts_at(window->x + 2, window->y + 5, "Debugger: Integrated");
                vga_puts_at(window->x + 2, window->y + 6, "REPL: Available");
                vga_puts_at(window->x + 2, window->y + 7, "Terminal: Active");
                vga_puts_at(window->x + 2, window->y + 8, "");
                vga_puts_at(window->x + 2, window->y + 9, "Ready for Cybergenisis development!");
                vga_puts_at(window->x + 2, window->y + 10, "");
                vga_puts_at(window->x + 2, window->y + 11, "Commands: help, memory, debug, repl");
            } else if (window->id == 2) { // System info window
                vga_puts_at(window->x + 2, window->y + 1, "System: Running | Memory: OK | CPU: x86_64");
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
    
    // Cursor is handled by VGA driver
    return 0;
}

/**
 * Handle mouse input
 */
int desktop_handle_mouse(uint16_t x, uint16_t y, uint8_t buttons) {
    if (!g_desktop.running) {
        return -1;
    }
    
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
