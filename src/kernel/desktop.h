/**
 * CompileOS Desktop and Login System
 * Provides graphical desktop environment with login screen
 */

#ifndef DESKTOP_H
#define DESKTOP_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>  // For size_t

// Desktop dimensions
#define DESKTOP_WIDTH 320
#define DESKTOP_HEIGHT 200

// Window structure
typedef struct {
    int x, y;           // Position
    int width, height;  // Size
    char title[32];     // Window title
    uint8_t color;      // Window color
    bool visible;       // Is window visible
    bool active;        // Is window active (focused)
} window_t;

// Desktop colors
#define COLOR_DESKTOP_BG    1   // Blue background
#define COLOR_WINDOW_BG     7   // Light gray
#define COLOR_WINDOW_BORDER 8   // Dark gray
#define COLOR_TEXT          15  // White
#define COLOR_TEXT_BG       0   // Black

// Desktop state
typedef struct {
    bool initialized;
    window_t* active_window;
    int window_count;
    window_t windows[10];  // Max 10 windows
} desktop_t;

// Global desktop instance
extern desktop_t g_desktop;

// Desktop Functions

/**
 * Initialize desktop environment
 */
void desktop_init(void);

/**
 * Create a new window
 */
window_t* desktop_create_window(int x, int y, int width, int height, const char* title);

/**
 * Draw desktop background
 */
void desktop_draw_background(void);

/**
 * Draw all windows
 */
void desktop_draw_windows(void);

/**
 * Draw a single window
 */
void desktop_draw_window(window_t* window);

/**
 * Handle window events (mouse, keyboard)
 */
void desktop_handle_events(void);

/**
 * Show login screen
 */
void show_login_screen(void);

/**
 * Handle login input
 */
bool handle_login(const char* username, const char* password);

/**
 * Show desktop after successful login
 */
void show_desktop(void);

/**
 * Draw taskbar
 */
void desktop_draw_taskbar(void);

/**
 * Draw start menu
 */
void desktop_draw_start_menu(void);

/**
 * Draw window borders and decorations
 */
void desktop_draw_window_decorations(window_t* window);

/**
 * Draw window contents
 */
void desktop_draw_window_contents(window_t* window);

/**
 * Refresh desktop display
 */
void desktop_refresh(void);

/**
 * Get mouse position (when mouse driver is available)
 */
void desktop_get_mouse(int* x, int* y);

/**
 * Set mouse position
 */
void desktop_set_mouse(int x, int y);

#endif /* DESKTOP_H */
