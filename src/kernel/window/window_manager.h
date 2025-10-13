/**
 * CompileOS Hero Window Manager
 * Professional compositing window manager with transparency and effects
 */

#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

// Window types
typedef enum {
    WINDOW_TYPE_TERMINAL,
    WINDOW_TYPE_FILE_MANAGER,
    WINDOW_TYPE_BROWSER,
    WINDOW_TYPE_SETTINGS,
    WINDOW_TYPE_CALCULATOR,
    WINDOW_TYPE_TEXT_EDITOR,
    WINDOW_TYPE_SYSTEM_MONITOR,
    WINDOW_TYPE_ABOUT,
    WINDOW_TYPE_DESKTOP
} window_type_t;

// Window states
typedef enum {
    WINDOW_STATE_NORMAL,
    WINDOW_STATE_MINIMIZED,
    WINDOW_STATE_MAXIMIZED,
    WINDOW_STATE_ACTIVE,
    WINDOW_STATE_INACTIVE,
    WINDOW_STATE_CLOSING,
    WINDOW_STATE_OPENING
} window_state_t;

// Window effects
typedef enum {
    WINDOW_EFFECT_NONE = 0,
    WINDOW_EFFECT_DROP_SHADOW = (1 << 0),
    WINDOW_EFFECT_TRANSPARENCY = (1 << 1),
    WINDOW_EFFECT_GRADIENT = (1 << 2),
    WINDOW_EFFECT_ROUNDED_CORNERS = (1 << 3)
} window_effects_t;

// Window structure
typedef struct window_t {
    uint16_t x, y;           // Position
    uint16_t width, height;  // Size
    char title[64];          // Window title
    window_type_t type;      // Window type
    window_state_t state;    // Current state
    bool visible;            // Visibility
    bool draggable;          // Can be dragged
    bool resizable;          // Can be resized
    bool minimizable;        // Can be minimized
    bool closable;           // Can be closed
    uint8_t bg_color;        // Background color (for text mode)
    uint8_t fg_color;        // Foreground color (for text mode)
    uint32_t bg_color_rgba;  // Background color (for graphics mode)
    uint32_t fg_color_rgba;  // Foreground color (for graphics mode)
    uint32_t border_color;   // Border color
    window_effects_t effects; // Visual effects
    uint8_t transparency;    // Transparency level (0-255)
    void* content;           // Window-specific content
    struct window_t* next;   // Next window in list
    struct window_t* prev;   // Previous window in list
    uint32_t animation_frame; // For animations
    uint32_t creation_time;  // For window management
} window_t;

// Desktop theme
typedef struct {
    uint32_t desktop_bg_color;
    uint32_t desktop_bg_color2;  // For gradient/patterns
    uint32_t window_bg_color;
    uint32_t window_title_bg_color;
    uint32_t window_border_color;
    uint32_t window_shadow_color;
    uint32_t taskbar_bg_color;
    uint32_t taskbar_fg_color;
    uint8_t font_size;
    char font_family[32];
} desktop_theme_t;

// Window manager functions
int window_manager_init(void);
window_t* window_create(const char* title, uint16_t x, uint16_t y, uint16_t width, uint16_t height, window_type_t type);
int window_destroy(window_t* window);
int window_show(window_t* window);
int window_hide(window_t* window);
int window_minimize(window_t* window);
int window_maximize(window_t* window);
int window_restore(window_t* window);
int window_focus(window_t* window);
int window_move(window_t* window, uint16_t x, uint16_t y);
int window_resize(window_t* window, uint16_t width, uint16_t height);
window_t* window_get_at(uint16_t x, uint16_t y);
window_t* window_get_active(void);
int window_draw_all(void);
int window_update_mouse(uint16_t mouse_x, uint16_t mouse_y);
int window_handle_click(uint16_t x, uint16_t y);

// Desktop management
void desktop_set_theme(const desktop_theme_t* theme);
void desktop_draw_background(void);
void desktop_draw_icons(void);
void desktop_draw_taskbar(void);
void desktop_refresh(void);

// Window effects and animations
void window_apply_effects(window_t* window);
void window_draw_shadow(window_t* window);
void window_draw_transparency(window_t* window);
void window_animate_open(window_t* window);
void window_animate_close(window_t* window);

// Advanced window management
void window_bring_to_front(window_t* window);
void window_send_to_back(window_t* window);
void window_tile_windows(void);
void window_cascade_windows(void);

// Window content rendering
void window_render_content(window_t* window);
void window_draw_title_bar(window_t* window);
void window_draw_borders(window_t* window);
void window_draw_content_area(window_t* window);

// Event handling
typedef void (*window_event_callback_t)(window_t* window, uint32_t event_type, void* event_data);
int window_register_event_handler(window_event_callback_t callback);
void window_handle_events(void);

// Modern UI elements
void ui_draw_button(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const char* text, bool pressed);
void ui_draw_scrollbar(uint16_t x, uint16_t y, uint16_t height, float position, float size);
void ui_draw_progress_bar(uint16_t x, uint16_t y, uint16_t width, uint16_t height, float progress);
void ui_draw_menu(uint16_t x, uint16_t y, const char* items[], int item_count, int selected);

// Graphics integration
int window_set_graphics_mode(bool use_graphics);
bool window_is_graphics_mode(void);

#endif // WINDOW_MANAGER_H
