/**
 * CompileOS Window Manager Implementation
 * Advanced windowing system with multiple overlapping windows
 */

#include "window_manager.h"
#include "../../drivers/vga/vga.h"
#include <string.h>
#include <stdlib.h>

// Forward declarations
void window_draw(window_t* window);
void window_draw_content(window_t* window);
void window_draw_calculator(window_t* window);
void window_draw_text_editor(window_t* window);

// Window list management
static window_t* window_list_head = NULL;
static window_t* window_list_tail = NULL;
static int window_count = 0;
static window_t* g_active_window = NULL;

// Desktop background window
static window_t desktop_window = {
    .x = 0, .y = 0,
    .width = 80, .height = 25,
    .title = "Desktop",
    .type = WINDOW_TYPE_TERMINAL,
    .state = WINDOW_STATE_ACTIVE,
    .visible = true,
    .draggable = false,
    .resizable = false,
    .bg_color = 0x00,
    .fg_color = 0x0F,
    .next = NULL,
    .prev = NULL
};

/**
 * Initialize window manager
 */
int window_manager_init(void) {
    g_active_window = &desktop_window;
    window_list_head = &desktop_window;
    window_list_tail = &desktop_window;
    vga_clear_screen();
    return 0;
}

/**
 * Create a new window
 */
window_t* window_create(const char* title, uint16_t x, uint16_t y, uint16_t width, uint16_t height, window_type_t type) {
    window_t* window = (window_t*)malloc(sizeof(window_t));
    if (!window) return NULL;

    strncpy(window->title, title, sizeof(window->title) - 1);
    window->title[sizeof(window->title) - 1] = '\0';
    window->x = x;
    window->y = y;
    window->width = width;
    window->height = height;
    window->type = type;
    window->state = WINDOW_STATE_NORMAL;
    window->visible = true;
    window->draggable = true;
    window->resizable = true;
    window->minimizable = true;
    window->closable = true;
    window->bg_color = 0x00;
    window->fg_color = 0x0F;
    window->bg_color_rgba = 0x00000000;
    window->fg_color_rgba = 0x00FFFFFF;
    window->border_color = 0x00FFFFFF;
    window->effects = WINDOW_EFFECT_NONE;
    window->transparency = 255;
    window->content = NULL;
    window->next = NULL;
    window->prev = NULL;
    window->animation_frame = 0;
    window->creation_time = 0;

    if (window_list_tail) {
        window_list_tail->next = window;
        window->prev = window_list_tail;
        window_list_tail = window;
    } else {
        window_list_head = window;
        window_list_tail = window;
    }
    window_count++;

    return window;
}

/**
 * Destroy a window
 */
int window_destroy(window_t* window) {
    if (!window) return -1;

    if (window_list_head == window) window_list_head = window->next;
    if (window_list_tail == window) window_list_tail = window->prev;

    if (window->prev) window->prev->next = window->next;
    if (window->next) window->next->prev = window->prev;

    free(window);
    window_count--;

    return 0;
}

/**
 * Show a window
 */
int window_show(window_t* window) {
    if (!window) return -1;
    window->visible = true;
    return 0;
}

/**
 * Hide a window
 */
int window_hide(window_t* window) {
    if (!window) return -1;
    window->visible = false;
    return 0;
}

/**
 * Minimize a window
 */
int window_minimize(window_t* window) {
    if (!window) return -1;
    window->state = WINDOW_STATE_MINIMIZED;
    return 0;
}

/**
 * Maximize a window
 */
int window_maximize(window_t* window) {
    if (!window) return -1;
    window->state = WINDOW_STATE_MAXIMIZED;
    return 0;
}

/**
 * Restore a window
 */
int window_restore(window_t* window) {
    if (!window) return -1;
    window->state = WINDOW_STATE_NORMAL;
    return 0;
}

/**
 * Focus a window
 */
int window_focus(window_t* window) {
    if (!window) return -1;
    g_active_window = window;
    return 0;
}

/**
 * Move window to new position
 */
int window_move(window_t* window, uint16_t x, uint16_t y) {
    if (!window) return -1;
    window->x = x;
    window->y = y;
    return 0;
}

/**
 * Resize a window
 */
int window_resize(window_t* window, uint16_t width, uint16_t height) {
    if (!window) return -1;
    window->width = width;
    window->height = height;
    return 0;
}

/**
 * Get window at coordinates
 */
window_t* window_get_at(uint16_t x, uint16_t y) {
    window_t* current = window_list_head;
    while (current) {
        if (current->visible &&
            x >= current->x && x < current->x + current->width &&
            y >= current->y && y < current->y + current->height) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

/**
 * Get active window
 */
window_t* window_get_active(void) {
    return g_active_window;
}

/**
 * Draw all windows
 */
int window_draw_all(void) {
    window_t* window = window_list_head;
    while (window) {
        if (window->visible) {
            window_draw(window);
        }
        window = window->next;
    }
    return 0;
}

/**
 * Update mouse position for windowing
 */
int window_update_mouse(uint16_t mouse_x, uint16_t mouse_y) {
    window_t* window_under_mouse = window_get_at(mouse_x / 8, mouse_y / 16);

    if (g_active_window && g_active_window->draggable &&
        mouse_x >= g_active_window->x * 8 && mouse_x < (g_active_window->x + g_active_window->width) * 8 &&
        mouse_y >= g_active_window->y * 16 && mouse_y < (g_active_window->y + 1) * 16) {
        static uint16_t drag_offset_x = 0, drag_offset_y = 0;
        static bool dragging = false;

        if (!dragging) {
            drag_offset_x = mouse_x - (g_active_window->x * 8);
            drag_offset_y = mouse_y - (g_active_window->y * 16);
            dragging = true;
        }

        uint16_t new_x = (mouse_x - drag_offset_x) / 8;
        uint16_t new_y = (mouse_y - drag_offset_y) / 16;

        window_move(g_active_window, new_x, new_y);
    } else {
        static bool dragging = false;
        dragging = false;
    }

    return 0;
}

/**
 * Handle mouse click for windowing
 */
int window_handle_click(uint16_t x, uint16_t y) {
    uint16_t char_x = x / 8;
    uint16_t char_y = y / 16;

    window_t* clicked_window = window_get_at(char_x, char_y);

    if (clicked_window && clicked_window != g_active_window) {
        window_focus(clicked_window);
    }

    return 0;
}

/**
 * Draw a single window
 */
void window_draw(window_t* window) {
    if (!window || !window->visible) return;

    // Draw window border
    for (uint16_t x = window->x; x < window->x + window->width; x++) {
        vga_puts_at(x, window->y, "-");
        vga_puts_at(x, window->y + window->height - 1, "-");
    }

    for (uint16_t y = window->y + 1; y < window->y + window->height - 1; y++) {
        vga_puts_at(window->x, y, "|");
        vga_puts_at(window->x + window->width - 1, y, "|");
    }

    // Corners
    vga_puts_at(window->x, window->y, "+");
    vga_puts_at(window->x + window->width - 1, window->y, "+");
    vga_puts_at(window->x, window->y + window->height - 1, "+");
    vga_puts_at(window->x + window->width - 1, window->y + window->height - 1, "+");

    // Title bar
    for (uint16_t x = window->x + 1; x < window->x + window->width - 1; x++) {
        vga_puts_at(x, window->y, " ");
    }

    // Draw title (centered)
    uint16_t title_len = strlen(window->title);
    uint16_t title_x = window->x + (window->width - title_len) / 2;
    vga_puts_at(title_x, window->y, (const char*)window->title);

    // Window content area
    for (uint16_t y = window->y + 2; y < window->y + window->height - 1; y++) {
        for (uint16_t x = window->x + 1; x < window->x + window->width - 1; x++) {
            vga_puts_at(x, y, " ");
        }
    }

    // Draw window-specific content
    window_draw_content(window);
}

/**
 * Draw window content based on type
 */
void window_draw_content(window_t* window) {
    if (!window) return;

    uint16_t content_x = window->x + 2;
    uint16_t content_y = window->y + 2;

    switch (window->type) {
        case WINDOW_TYPE_TERMINAL:
            vga_puts_at(content_x, content_y, (const char*)"CompileOS Terminal v1.0");
            vga_puts_at(content_x, content_y + 1, (const char*)"=======================");
            vga_puts_at(content_x, content_y + 3, (const char*)"Type 'help' for commands");
            vga_puts_at(content_x, content_y + 5, (const char*)"Ready > ");
            break;

        case WINDOW_TYPE_FILE_MANAGER:
            vga_puts_at(content_x, content_y, (const char*)"File Manager");
            vga_puts_at(content_x, content_y + 1, (const char*)"============");
            vga_puts_at(content_x, content_y + 3, (const char*)"/                    [DIR]");
            vga_puts_at(content_x, content_y + 4, (const char*)"boot/                [DIR]");
            vga_puts_at(content_x, content_y + 5, (const char*)"kernel.bin          256KB");
            vga_puts_at(content_x, content_y + 6, (const char*)"README.md            4KB");
            break;

        case WINDOW_TYPE_BROWSER:
            vga_puts_at(content_x, content_y, (const char*)"Web Browser");
            vga_puts_at(content_x, content_y + 1, (const char*)"===========");
            vga_puts_at(content_x, content_y + 3, (const char*)"http://example.com");
            vga_puts_at(content_x, content_y + 5, (const char*)"Welcome to Example.com!");
            break;

        case WINDOW_TYPE_CALCULATOR:
            window_draw_calculator(window);
            break;

        case WINDOW_TYPE_TEXT_EDITOR:
            window_draw_text_editor(window);
            break;

        case WINDOW_TYPE_SYSTEM_MONITOR:
            vga_puts_at(content_x, content_y, (const char*)"System Monitor");
            vga_puts_at(content_x, content_y + 1, (const char*)"==============");
            vga_puts_at(content_x, content_y + 3, (const char*)"CPU Usage:  15%");
            vga_puts_at(content_x, content_y + 4, (const char*)"Memory:     32MB / 512MB");
            vga_puts_at(content_x, content_y + 5, (const char*)"Processes:  8");
            vga_puts_at(content_x, content_y + 6, (const char*)"Uptime:     00:05:23");
            break;

        default:
            vga_puts_at(content_x, content_y, (const char*)"Window content");
            break;
    }
}

/**
 * Draw calculator content
 */
void window_draw_calculator(window_t* window) {
    uint16_t x = window->x + 2;
    uint16_t y = window->y + 2;

    vga_puts_at(x, y, (const char*)"┌─────────────┐");
    vga_puts_at(x, y + 1, (const char*)"│    123.45   │");
    vga_puts_at(x, y + 2, (const char*)"└─────────────┘");

    vga_puts_at(x + 2, y + 4, (const char*)"7 8 9 +");
    vga_puts_at(x + 2, y + 5, (const char*)"4 5 6 -");
    vga_puts_at(x + 2, y + 6, (const char*)"1 2 3 *");
    vga_puts_at(x + 2, y + 7, (const char*)"0 . = /");
    vga_puts_at(x + 2, y + 8, (const char*)"C √ ^ ( )");
}

/**
 * Draw text editor content
 */
void window_draw_text_editor(window_t* window) {
    uint16_t x = window->x + 2;
    uint16_t y = window->y + 2;

    vga_puts_at(x, y, (const char*)"1 | # CompileOS Text Editor");
    vga_puts_at(x, y + 1, (const char*)"2 | # Professional text editing");
    vga_puts_at(x, y + 2, (const char*)"3 | ");
    vga_puts_at(x, y + 3, (const char*)"4 | Features:");
    vga_puts_at(x, y + 4, (const char*)"5 | - Syntax highlighting");
    vga_puts_at(x, y + 5, (const char*)"6 | - File operations");
    vga_puts_at(x, y + 6, (const char*)"7 | - Search and replace");
    vga_puts_at(x, y + 7, (const char*)"8 | ");
    vga_puts_at(x, y + 8, (const char*)"9 | Line 9 of 20");
}
