/**
 * CompileOS Desktop Implementation
 */

#include "desktop.h"
#include "vga_graphics.h"
#include <string.h>

// Global desktop instance
desktop_t g_desktop = {0};

// Desktop implementation - uses standard library functions from util.c

/**
 * Initialize desktop environment
 */
void desktop_init(void) {
    g_desktop.initialized = true;
    g_desktop.window_count = 0;
    g_desktop.active_window = NULL;

    // Set graphics mode for desktop
    vga_set_graphics_mode();
}

/**
 * Create a new window
 */
window_t* desktop_create_window(int x, int y, int width, int height, const char* title) {
    if (g_desktop.window_count >= 10) {
        return NULL;  // Max windows reached
    }

    window_t* window = &g_desktop.windows[g_desktop.window_count];
    window->x = x;
    window->y = y;
    window->width = width;
    window->height = height;
    window->color = COLOR_WINDOW_BG;
    window->visible = true;
    window->active = (g_desktop.active_window == NULL);

    strcpy(window->title, title);

    if (window->active) {
        g_desktop.active_window = window;
    }

    g_desktop.window_count++;
    return window;
}

/**
 * Draw desktop background
 */
void desktop_draw_background(void) {
    // Blue gradient background
    for (int y = 0; y < DESKTOP_HEIGHT; y++) {
        for (int x = 0; x < DESKTOP_WIDTH; x++) {
            uint8_t color = COLOR_DESKTOP_BG;
            // Add some variation for gradient effect
            if ((x + y) % 3 == 0) color = COLOR_DESKTOP_BG + 1;
            vga_plot_pixel(x, y, color);
        }
    }
}

/**
 * Draw all windows
 */
void desktop_draw_windows(void) {
    for (int i = 0; i < g_desktop.window_count; i++) {
        if (g_desktop.windows[i].visible) {
            desktop_draw_window(&g_desktop.windows[i]);
        }
    }
}

/**
 * Draw a single window
 */
void desktop_draw_window(window_t* window) {
    if (!window || !window->visible) return;

    // Draw window background
    vga_draw_rect(window->x, window->y,
                  window->x + window->width - 1,
                  window->y + window->height - 1,
                  window->color);

    // Draw window border
    vga_draw_rect(window->x, window->y,
                  window->x + window->width - 1,
                  window->y + window->height - 1,
                  COLOR_WINDOW_BORDER);

    // Draw window decorations (title bar, etc.)
    desktop_draw_window_decorations(window);

    // Draw window contents
    desktop_draw_window_contents(window);
}

/**
 * Draw window borders and decorations
 */
void desktop_draw_window_decorations(window_t* window) {
    if (!window) return;

    // Title bar
    vga_draw_rect(window->x, window->y,
                  window->x + window->width - 1,
                  window->y + 20,
                  window->active ? COLOR_TEXT_BG : COLOR_WINDOW_BORDER);

    // Window title
    vga_draw_text(window->title, window->x + 5, window->y + 5, COLOR_TEXT);

    // Close button (simple X)
    vga_draw_text("X", window->x + window->width - 15, window->y + 5, COLOR_TEXT);
}

/**
 * Draw window contents
 */
void desktop_draw_window_contents(window_t* window) {
    if (!window) return;

    // Content area (below title bar)
    int content_y = window->y + 25;

    if (strcmp(window->title, "Terminal") == 0) {
        // Terminal window
        vga_draw_text("CompileOS Terminal v0.1.0", window->x + 5, content_y + 5, COLOR_TEXT);
        vga_draw_text("$ ", window->x + 5, content_y + 25, COLOR_TEXT);
    } else if (strcmp(window->title, "File Manager") == 0) {
        // File manager window
        vga_draw_text("Files:", window->x + 5, content_y + 5, COLOR_TEXT);
        vga_draw_text("/home/user/", window->x + 10, content_y + 20, COLOR_TEXT);
    } else {
        // Generic content
        vga_draw_text("Window content", window->x + 5, content_y + 5, COLOR_TEXT);
    }
}

/**
 * Show login screen
 */
void show_login_screen(void) {
    // Trace to serial so we can see this in headless logs
    {
        __asm__ volatile ("push %eax\n\tmov $0x3F8, %dx\n1:\n\tinb %dx, %al\n\tand $0x20, %al\n\tjnz 2f\n\tjmp 1b\n2:\n\toutb %al, %dx\n\tpop %eax\n" ::: "eax", "dx");
    }
    // Set graphics mode
    vga_set_graphics_mode();

    // Draw desktop background
    desktop_draw_background();

    // Draw login dialog (centered)
    int dialog_width = 200;
    int dialog_height = 100;
    int dialog_x = (DESKTOP_WIDTH - dialog_width) / 2;
    int dialog_y = (DESKTOP_HEIGHT - dialog_height) / 2;

    // Login dialog background
    vga_draw_rect(dialog_x, dialog_y, dialog_x + dialog_width - 1, dialog_y + dialog_height - 1, COLOR_WINDOW_BG);

    // Login dialog border
    vga_draw_rect(dialog_x, dialog_y, dialog_x + dialog_width - 1, dialog_y + dialog_height - 1, COLOR_WINDOW_BORDER);

    // Title
    vga_draw_text("CompileOS Login", dialog_x + 60, dialog_y + 10, COLOR_TEXT);

    // Username field
    vga_draw_text("Username:", dialog_x + 10, dialog_y + 30, COLOR_TEXT);
    vga_draw_rect(dialog_x + 80, dialog_y + 25, dialog_x + 180, dialog_y + 35, COLOR_TEXT_BG);

    // Password field
    vga_draw_text("Password:", dialog_x + 10, dialog_y + 50, COLOR_TEXT);
    vga_draw_rect(dialog_x + 80, dialog_y + 45, dialog_x + 180, dialog_y + 55, COLOR_TEXT_BG);

    // Login button
    vga_draw_rect(dialog_x + 70, dialog_y + 70, dialog_x + 130, dialog_y + 85, COLOR_TEXT);
    vga_draw_text("Login", dialog_x + 90, dialog_y + 75, COLOR_TEXT_BG);

    // Wait for login (in real implementation, would handle input)
    delay_ms(3000);  // Show for 3 seconds for demo

    // Auto-login for demo (in real implementation, would verify credentials)
    show_desktop();
}

/**
 * Handle login input
 */
bool handle_login(const char* username, const char* password) {
    // Simple demo: accept any non-empty username/password
    return username && password && strlen(username) > 0 && strlen(password) > 0;
}

/**
 * Show desktop after successful login
 */
void show_desktop(void) {
    // Clear screen
    desktop_draw_background();

    // Draw taskbar at bottom
    desktop_draw_taskbar();

    // Create initial windows
    desktop_create_window(20, 30, 280, 140, "Terminal");
    desktop_create_window(50, 50, 200, 100, "File Manager");

    // Draw all windows
    desktop_draw_windows();

    // Refresh display
    desktop_refresh();
}

/**
 * Draw taskbar
 */
void desktop_draw_taskbar(void) {
    int taskbar_y = DESKTOP_HEIGHT - 20;

    // Taskbar background
    vga_draw_rect(0, taskbar_y, DESKTOP_WIDTH - 1, DESKTOP_HEIGHT - 1, COLOR_WINDOW_BG);

    // Start button
    vga_draw_rect(5, taskbar_y + 2, 50, DESKTOP_HEIGHT - 3, COLOR_TEXT);
    vga_draw_text("Start", 15, taskbar_y + 5, COLOR_TEXT_BG);

    // Clock
    vga_draw_text("00:00", DESKTOP_WIDTH - 35, taskbar_y + 5, COLOR_TEXT);
}

/**
 * Handle window events (mouse, keyboard)
 */
void desktop_handle_events(void) {
    // In a real implementation, this would handle:
    // - Mouse clicks and movement
    // - Keyboard input
    // - Window dragging, resizing
    // - Button clicks

    // For now, just refresh periodically
    delay_ms(100);
}

/**
 * Refresh desktop display
 */
void desktop_refresh(void) {
    // Redraw everything
    desktop_draw_background();
    desktop_draw_taskbar();
    desktop_draw_windows();

    // Handle events
    desktop_handle_events();
}

/**
 * Get mouse position (when mouse driver is available)
 */
void desktop_get_mouse(int* x, int* y) {
    // In real implementation, would read mouse position
    if (x) *x = 0;
    if (y) *y = 0;
}

/**
 * Set mouse position
 */
void desktop_set_mouse(int x, int y) {
    // In real implementation, would set mouse cursor position
    (void)x;
    (void)y;
}
