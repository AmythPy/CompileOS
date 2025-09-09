/**
 * CompileOS Desktop Environment - Header
 * 
 * Basic desktop with window management and GUI
 */

#ifndef DESKTOP_H
#define DESKTOP_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// Desktop modes
typedef enum {
    DESKTOP_MODE_TEXT = 0,
    DESKTOP_MODE_GRAPHICS
} desktop_mode_t;

// Window states
typedef enum {
    WINDOW_STATE_NORMAL = 0,
    WINDOW_STATE_MINIMIZED,
    WINDOW_STATE_MAXIMIZED,
    WINDOW_STATE_HIDDEN
} window_state_t;

// Window types
typedef enum {
    WINDOW_TYPE_APPLICATION = 0,
    WINDOW_TYPE_DIALOG,
    WINDOW_TYPE_MENU,
    WINDOW_TYPE_TOOLTIP
} window_type_t;

// Window structure
typedef struct {
    uint32_t id;
    char title[64];
    uint16_t x, y;
    uint16_t width, height;
    window_state_t state;
    window_type_t type;
    bool visible;
    bool focused;
    void* content;
    struct window* next;
    struct window* prev;
} window_t;

// Desktop structure
typedef struct {
    desktop_mode_t mode;
    uint16_t width;
    uint16_t height;
    window_t* windows;
    window_t* focused_window;
    uint32_t next_window_id;
    bool running;
} desktop_t;

// Desktop initialization and control
int desktop_init(void);
void desktop_shutdown(void);
int desktop_start(void);
void desktop_stop(void);
bool desktop_is_running(void);

// Window management
window_t* desktop_create_window(const char* title, uint16_t x, uint16_t y, uint16_t width, uint16_t height);
int desktop_destroy_window(window_t* window);
int desktop_show_window(window_t* window);
int desktop_hide_window(window_t* window);
int desktop_focus_window(window_t* window);
int desktop_move_window(window_t* window, uint16_t x, uint16_t y);
int desktop_resize_window(window_t* window, uint16_t width, uint16_t height);

// Window operations
window_t* desktop_get_window_by_id(uint32_t id);
window_t* desktop_get_focused_window(void);
int desktop_list_windows(window_t** windows, size_t max_count, size_t* actual_count);
int desktop_bring_to_front(window_t* window);
int desktop_send_to_back(window_t* window);

// Desktop rendering
int desktop_render(void);
int desktop_clear(void);
int desktop_draw_background(void);
int desktop_draw_windows(void);
int desktop_draw_cursor(void);

// Desktop input handling
int desktop_handle_mouse(uint16_t x, uint16_t y, uint8_t buttons);
int desktop_handle_keyboard(uint16_t key_code, bool pressed);
int desktop_handle_click(uint16_t x, uint16_t y, uint8_t button);

// Desktop themes
typedef struct {
    uint8_t background_color;
    uint8_t window_border_color;
    uint8_t window_title_color;
    uint8_t text_color;
    uint8_t highlight_color;
} desktop_theme_t;

int desktop_set_theme(const desktop_theme_t* theme);
int desktop_get_theme(desktop_theme_t* theme);

// Desktop applications
typedef struct {
    char name[32];
    char description[64];
    int (*init)(void);
    int (*run)(void);
    int (*cleanup)(void);
    window_t* window;
} desktop_app_t;

int desktop_register_app(const desktop_app_t* app);
int desktop_unregister_app(const char* name);
int desktop_launch_app(const char* name);
int desktop_list_apps(char* output, size_t output_size);

// Built-in desktop apps
int desktop_app_terminal(void);
int desktop_app_file_manager(void);
int desktop_app_memory_viewer(void);
int desktop_app_debugger(void);
int desktop_app_repl(void);

// Desktop menu system
typedef struct {
    char text[32];
    int (*action)(void);
    struct menu_item* submenu;
} menu_item_t;

typedef struct {
    char title[32];
    menu_item_t* items;
    size_t item_count;
    uint16_t x, y;
    bool visible;
} menu_t;

int desktop_create_menu(const char* title, menu_item_t* items, size_t item_count);
int desktop_show_menu(menu_t* menu, uint16_t x, uint16_t y);
int desktop_hide_menu(menu_t* menu);
int desktop_handle_menu_click(uint16_t x, uint16_t y);

// Desktop taskbar
typedef struct {
    uint16_t height;
    bool visible;
    char* app_buttons;
    size_t button_count;
} taskbar_t;

int desktop_create_taskbar(void);
int desktop_add_taskbar_button(const char* app_name, uint8_t icon);
int desktop_remove_taskbar_button(const char* app_name);
int desktop_render_taskbar(void);

// Desktop wallpaper
int desktop_set_wallpaper(const char* image_path);
int desktop_draw_wallpaper(void);
int desktop_clear_wallpaper(void);

// Desktop cursor
typedef struct {
    uint16_t x, y;
    bool visible;
    uint8_t shape;
    bool blinking;
} desktop_cursor_t;

int desktop_set_cursor(uint16_t x, uint16_t y);
int desktop_get_cursor(desktop_cursor_t* cursor);
int desktop_show_cursor(void);
int desktop_hide_cursor(void);
int desktop_set_cursor_shape(uint8_t shape);

// Desktop notifications
typedef struct {
    char title[32];
    char message[128];
    uint32_t duration_ms;
    bool visible;
} notification_t;

int desktop_show_notification(const char* title, const char* message, uint32_t duration_ms);
int desktop_hide_notification(notification_t* notification);
int desktop_render_notifications(void);

// Desktop configuration
typedef struct {
    desktop_mode_t mode;
    uint16_t width;
    uint16_t height;
    bool auto_start_apps;
    bool show_taskbar;
    bool show_desktop_icons;
    uint8_t window_border_width;
    uint8_t taskbar_height;
} desktop_config_t;

int desktop_set_config(const desktop_config_t* config);
int desktop_get_config(desktop_config_t* config);

#endif // DESKTOP_H
