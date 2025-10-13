/**
 * CompileOS Hero Web Browser
 * Feature-rich web browser with HTML rendering and networking
 */

#ifndef BROWSER_H
#define BROWSER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
// Graphics color type
#include "../drivers/graphics/graphics.h"

// Browser window dimensions
#define BROWSER_WINDOW_WIDTH  70
#define BROWSER_WINDOW_HEIGHT 20

// Browser UI elements
#define BROWSER_URL_BAR_HEIGHT 3
#define BROWSER_STATUS_BAR_HEIGHT 2
#define BROWSER_CONTENT_HEIGHT (BROWSER_WINDOW_HEIGHT - BROWSER_URL_BAR_HEIGHT - BROWSER_STATUS_BAR_HEIGHT - 4)

// Browser colors
#define BROWSER_BG_COLOR        0x00112233
#define BROWSER_FG_COLOR        0x00FFFFFF
#define BROWSER_LINK_COLOR      0x0000FF88
#define BROWSER_TITLE_BG_COLOR  0x00223344
#define BROWSER_URL_BG_COLOR    0x00112222

// HTML tag types
typedef enum {
    HTML_TAG_UNKNOWN = 0,
    HTML_TAG_HTML,
    HTML_TAG_HEAD,
    HTML_TAG_TITLE,
    HTML_TAG_BODY,
    HTML_TAG_H1,
    HTML_TAG_H2,
    HTML_TAG_H3,
    HTML_TAG_P,
    HTML_TAG_A,
    HTML_TAG_BR,
    HTML_TAG_DIV,
    HTML_TAG_IMG,
    HTML_TAG_B,
    HTML_TAG_I,
    HTML_TAG_U,
    HTML_TAG_UL,
    HTML_TAG_OL,
    HTML_TAG_LI,
    HTML_TAG_TABLE,
    HTML_TAG_TR,
    HTML_TAG_TD,
    HTML_TAG_TH,
    HTML_TAG_FORM,
    HTML_TAG_INPUT,
    HTML_TAG_BUTTON,
    HTML_TAG_TEXTAREA,
    HTML_TAG_SELECT,
    HTML_TAG_OPTION,
    HTML_TAG_TEXT
} html_tag_type_t;

// HTML element structure
typedef struct html_element_t {
    html_tag_type_t type;
    char* tag_name;
    char* attributes[32];
    char* content;
    struct html_element_t* children;
    struct html_element_t* next;
    struct html_element_t* parent;
    int x, y;  // Position for rendering
    int width, height;
} html_element_t;

// Browser state
typedef struct {
    char current_url[256];
    char page_title[128];
    char status_message[128];
    html_element_t* current_page;
    bool loading;
    float load_progress;
    int scroll_position;
    int content_height;
    bool show_images;
    bool enable_javascript;  // Placeholder for future JS support
} browser_state_t;

// Browser functions
int browser_init(void);
int browser_create_window(void);
int browser_load_url(const char* url);
int browser_render_page(void);
int browser_handle_input(char key);

// HTML parsing
html_element_t* html_parse(const char* html_text);
// Helper to create demo DOM used by the browser (implemented in browser.c)
html_element_t* html_create_demo_page(void);
void html_free_element(html_element_t* element);
html_element_t* html_find_element(html_element_t* root, html_tag_type_t type);
char* html_get_attribute(html_element_t* element, const char* name);

// HTTP client integration
int browser_http_get(const char* url, char* buffer, size_t buffer_size);
int browser_http_post(const char* url, const char* data, char* buffer, size_t buffer_size);

// Rendering functions
void browser_render_element(html_element_t* element, int x, int y);
void browser_render_text(const char* text, int x, int y, color_rgba_t color);
void browser_render_link(const char* text, const char* url, int x, int y);
void browser_render_heading(const char* text, int level, int x, int y);
void browser_render_paragraph(const char* text, int x, int y);
void browser_render_list(html_element_t* list_element, int x, int y);
void browser_render_table(html_element_t* table_element, int x, int y);

// UI functions
void browser_draw_url_bar(const char* url);
void browser_draw_status_bar(const char* status);
void browser_draw_scrollbar(int position, int max_position);
void browser_draw_loading_animation(float progress);

// Navigation functions
void browser_back(void);
void browser_forward(void);
void browser_refresh(void);
void browser_home(void);

// Bookmark management
int browser_add_bookmark(const char* title, const char* url);
int browser_load_bookmarks(void);

// Browser settings
void browser_set_user_agent(const char* user_agent);
void browser_set_enable_images(bool enable);
void browser_set_enable_javascript(bool enable);

#endif // BROWSER_H
