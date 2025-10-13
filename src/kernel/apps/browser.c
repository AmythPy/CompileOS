/**
 * CompileOS Hero Web Browser Implementation
 * Feature-rich web browser with HTML rendering and networking
 */

#include "browser.h"
#include "../../drivers/graphics/graphics.h"
#include "../net/net.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Minimal serial helper (COM1) for debug traces
static inline void outb_local(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}
static inline uint8_t inb_local(uint16_t port) {
    uint8_t v; __asm__ volatile ("inb %1, %0" : "=a"(v) : "Nd"(port)); return v;
}
static void serial_puts_local(const char* s) {
    while (*s) {
        while (!(inb_local(0x3F8 + 5) & 0x20)) {}
        outb_local(0x3F8, (uint8_t)*s++);
    }
}

// Some minimal libc implementations may not provide strdup; provide a simple fallback
#ifndef HAVE_STRDUP
char* strdup(const char* s) {
    if (!s) return NULL;
    size_t l = strlen(s) + 1;
    char* p = malloc(l);
    if (!p) return NULL;
    memcpy(p, s, l);
    return p;
}
#endif

// Global browser state
static browser_state_t g_browser_state = {
    .current_url = "http://example.com",
    .page_title = "CompileOS Browser",
    .status_message = "Ready",
    .current_page = NULL,
    .loading = false,
    .load_progress = 0.0f,
    .scroll_position = 0,
    .content_height = 0,
    .show_images = true,
    .enable_javascript = false
};

// User agent string
static const char* g_user_agent = "CompileOS/1.0 (Hero Browser)";

/**
 * Initialize browser
 */
int browser_init(void) {
    serial_puts_local("[TRACE] browser_init\n");
    // Initialize networking stack
    if (net_stack_init() != 0) {
        return -1;
    }

    // Initialize network devices
    if (network_init() != 0) {
        return -1;
    }

    return 0;
}

/**
 * Create browser window
 */
int browser_create_window(void) {
    serial_puts_local("[TRACE] browser_create_window\n");
    // Load default page
    return browser_load_url("http://example.com");
}

/**
 * Load URL in browser
 */
int browser_load_url(const char* url) {
    if (!url) return -1;

    // Set loading state
    g_browser_state.loading = true;
    g_browser_state.load_progress = 0.0f;
    strcpy(g_browser_state.status_message, "Connecting...");
    strncpy(g_browser_state.current_url, url, sizeof(g_browser_state.current_url) - 1);

    // Parse URL to extract hostname and path
    char hostname[128];
    char path[256];

    // Simple URL parsing (http://hostname/path)
    if (strncmp(url, "http://", 7) == 0) {
        const char* host_start = url + 7;
        const char* path_start = strchr(host_start, '/');

        if (path_start) {
            size_t host_len = path_start - host_start;
            strncpy(hostname, host_start, host_len);
            hostname[host_len] = '\0';
            strcpy(path, path_start);
        } else {
            strcpy(hostname, host_start);
            strcpy(path, "/");
        }
    } else {
        strcpy(hostname, "example.com");
        strcpy(path, "/");
    }

    // Resolve hostname to IP address
    uint32_t ip_addr;
    if (dns_resolve_hostname(hostname, &ip_addr) != 0) {
        strcpy(g_browser_state.status_message, "DNS resolution failed");
        g_browser_state.loading = false;
        return -1;
    }

    // Create HTTP request
    http_request_t* req = http_request_create(url);
    if (!req) {
        strcpy(g_browser_state.status_message, "Failed to create request");
        g_browser_state.loading = false;
        return -1;
    }

    // Send HTTP request
    g_browser_state.load_progress = 0.3f;
    strcpy(g_browser_state.status_message, "Sending request...");

    if (http_request_send(req) != 0) {
        strcpy(g_browser_state.status_message, "Failed to send request");
        http_request_destroy(req);
        g_browser_state.loading = false;
        return -1;
    }

    // Receive HTTP response
    g_browser_state.load_progress = 0.6f;
    strcpy(g_browser_state.status_message, "Receiving response...");

    char response_buffer[8192];
    memset(response_buffer, 0, sizeof(response_buffer));

    if (http_request_recv(req) <= 0) {
        strcpy(g_browser_state.status_message, "Failed to receive response");
        http_request_destroy(req);
        g_browser_state.loading = false;
        return -1;
    }

    // Parse HTTP response
    g_browser_state.load_progress = 0.9f;
    strcpy(g_browser_state.status_message, "Parsing HTML...");

    // Copy response to buffer (simplified)
    strncpy(response_buffer, req->body, sizeof(response_buffer) - 1);

    // Parse HTML content
    if (g_browser_state.current_page) {
        html_free_element(g_browser_state.current_page);
    }

    // For demo purposes, create a simple HTML structure
    g_browser_state.current_page = html_create_demo_page();

    // Update browser state
    g_browser_state.loading = false;
    g_browser_state.load_progress = 1.0f;
    strcpy(g_browser_state.status_message, "Page loaded");
    strcpy(g_browser_state.page_title, "Demo Page - CompileOS Browser");

    http_request_destroy(req);
    return 0;
}

/**
 * Render current page
 */
int browser_render_page(void) {
    serial_puts_local("[TRACE] browser_render_page\n");
    if (!g_browser_state.current_page) return -1;

    color_rgba_t bg_color = graphics_rgba(17, 34, 51, 255);
    color_rgba_t fg_color = graphics_rgba(255, 255, 255, 255);

    // Clear content area
    graphics_fill_rect(1, BROWSER_URL_BAR_HEIGHT + 2,
                      BROWSER_WINDOW_WIDTH - 2,
                      BROWSER_CONTENT_HEIGHT,
                      bg_color);

    // Render HTML elements
    browser_render_element(g_browser_state.current_page, 2, BROWSER_URL_BAR_HEIGHT + 3);

    return 0;
}

/**
 * Handle keyboard input
 */
int browser_handle_input(char key) {
    static char url_buffer[256] = {0};
    static int url_pos = 0;
    static bool editing_url = false;

    if (key == '\n' || key == '\r') {
        if (editing_url && url_pos > 0) {
            url_buffer[url_pos] = '\0';
            browser_load_url(url_buffer);
            editing_url = false;
            url_pos = 0;
        }
        return 0;
    }

    if (key == 27) {  // Escape key
        editing_url = false;
        url_pos = 0;
        return 0;
    }

    if (key == 'u' || key == 'U') {  // Start URL editing
        editing_url = true;
        url_pos = 0;
        strcpy(g_browser_state.status_message, "Enter URL:");
        return 0;
    }

    if (key == 'r' || key == 'R') {  // Refresh
        browser_refresh();
        return 0;
    }

    if (key == 'h' || key == 'H') {  // Home
        browser_home();
        return 0;
    }

    if (editing_url) {
        if (key == '\b' && url_pos > 0) {  // Backspace
            url_pos--;
            url_buffer[url_pos] = '\0';
        } else if (key >= 32 && key <= 126 && url_pos < sizeof(url_buffer) - 1) {  // Regular characters
            url_buffer[url_pos++] = key;
            url_buffer[url_pos] = '\0';
        }
    }

    return 0;
}

/**
 * Draw URL bar
 */
void browser_draw_url_bar(const char* url) {
    color_rgba_t url_bg = graphics_rgba(17, 34, 34, 255);
    color_rgba_t url_fg = graphics_rgba(255, 255, 255, 255);

    // Draw URL bar background
    graphics_fill_rect(1, 1, BROWSER_WINDOW_WIDTH - 2, BROWSER_URL_BAR_HEIGHT, url_bg);

    // Draw URL text
    graphics_draw_string(2, 2, "URL: ", url_fg, url_bg);

    // Draw current URL (truncated if too long)
    char display_url[50];
    if (strlen(url) > 45) {
        strncpy(display_url, url, 42);
        strcpy(display_url + 42, "...");
    } else {
        strcpy(display_url, url);
    }

    graphics_draw_string(7, 2, display_url, url_fg, url_bg);
}

/**
 * Draw status bar
 */
void browser_draw_status_bar(const char* status) {
    color_rgba_t status_bg = graphics_rgba(34, 51, 68, 255);
    color_rgba_t status_fg = graphics_rgba(255, 255, 255, 255);

    int y = BROWSER_WINDOW_HEIGHT - BROWSER_STATUS_BAR_HEIGHT;

    // Draw status bar background
    graphics_fill_rect(1, y, BROWSER_WINDOW_WIDTH - 2, BROWSER_STATUS_BAR_HEIGHT, status_bg);

    // Draw status text
    graphics_draw_string(2, y + 1, status, status_fg, status_bg);

    // Draw loading progress if loading
    if (g_browser_state.loading) {
        browser_draw_loading_animation(g_browser_state.load_progress);
    }
}

/**
 * Draw loading animation
 */
void browser_draw_loading_animation(float progress) {
    int y = BROWSER_WINDOW_HEIGHT - 1;
    int width = (int)(BROWSER_WINDOW_WIDTH * progress);

    color_rgba_t loading_color = graphics_rgba(0, 255, 0, 255);

    for (int i = 0; i < width && i < BROWSER_WINDOW_WIDTH; i++) {
        graphics_put_pixel(i, y, loading_color);
    }
}

/**
 * Draw scrollbar
 */
void browser_draw_scrollbar(int position, int max_position) {
    if (max_position <= 0) return;

    int scrollbar_height = BROWSER_CONTENT_HEIGHT;
    int thumb_height = scrollbar_height * BROWSER_CONTENT_HEIGHT / (max_position + BROWSER_CONTENT_HEIGHT);
    int thumb_position = scrollbar_height * position / (max_position + BROWSER_CONTENT_HEIGHT);

    color_rgba_t scrollbar_bg = graphics_rgba(68, 68, 68, 255);
    color_rgba_t scrollbar_fg = graphics_rgba(136, 136, 136, 255);

    // Draw scrollbar background
    graphics_fill_rect(BROWSER_WINDOW_WIDTH - 3, BROWSER_URL_BAR_HEIGHT + 2, 2, scrollbar_height, scrollbar_bg);

    // Draw scrollbar thumb
    graphics_fill_rect(BROWSER_WINDOW_WIDTH - 3,
                      BROWSER_URL_BAR_HEIGHT + 2 + thumb_position,
                      2, thumb_height, scrollbar_fg);
}

/**
 * Render HTML element
 */
void browser_render_element(html_element_t* element, int x, int y) {
    if (!element) return;

    color_rgba_t text_color = graphics_rgba(255, 255, 255, 255);

    switch (element->type) {
        case HTML_TAG_H1:
            browser_render_heading(element->content, 1, x, y);
            break;
        case HTML_TAG_H2:
            browser_render_heading(element->content, 2, x, y);
            break;
        case HTML_TAG_H3:
            browser_render_heading(element->content, 3, x, y);
            break;
        case HTML_TAG_P:
            browser_render_paragraph(element->content, x, y);
            break;
        case HTML_TAG_A:
            browser_render_link(element->content, html_get_attribute(element, "href"), x, y);
            break;
        case HTML_TAG_TEXT:
            browser_render_text(element->content, x, y, text_color);
            break;
        default:
            // Render children
            html_element_t* child = element->children;
            while (child) {
                browser_render_element(child, x, y);
                child = child->next;
            }
            break;
    }
}

/**
 * Render text
 */
void browser_render_text(const char* text, int x, int y, color_rgba_t color) {
    if (!text) return;

    color_rgba_t text_color = color;
    color_rgba_t bg_color = graphics_rgba(17, 34, 51, 255);

    graphics_draw_string(x, y, text, text_color, bg_color);
}

/**
 * Render link
 */
void browser_render_link(const char* text, const char* url, int x, int y) {
    if (!text) return;

    color_rgba_t link_color = graphics_rgba(0, 255, 136, 255);
    color_rgba_t bg_color = graphics_rgba(17, 34, 51, 255);

    // Draw link text
    graphics_draw_string(x, y, text, link_color, bg_color);

    // Draw underline
    for (int i = 0; i < strlen(text) && x + i < BROWSER_WINDOW_WIDTH - 2; i++) {
        graphics_put_pixel(x + i, y + 1, link_color);
    }

    // Draw URL tooltip
    if (url && strlen(url) > 0) {
        char tooltip[64];
        snprintf(tooltip, sizeof(tooltip), " [%s]", url);
        graphics_draw_string(x + strlen(text), y, tooltip, graphics_rgba(136, 136, 136, 255), bg_color);
    }
}

/**
 * Render heading
 */
void browser_render_heading(const char* text, int level, int x, int y) {
    if (!text) return;

    color_rgba_t heading_color = graphics_rgba(255, 255, 255, 255);
    color_rgba_t bg_color = graphics_rgba(17, 34, 51, 255);

    // Draw heading with appropriate size
    int font_size = 3 - level;  // H1 = larger, H3 = smaller
    if (font_size < 1) font_size = 1;

    // For now, just use regular string rendering
    graphics_draw_string(x, y, text, heading_color, bg_color);
}

/**
 * Render paragraph
 */
void browser_render_paragraph(const char* text, int x, int y) {
    if (!text) return;

    color_rgba_t text_color = graphics_rgba(255, 255, 255, 255);
    color_rgba_t bg_color = graphics_rgba(17, 34, 51, 255);

    graphics_draw_string(x, y, text, text_color, bg_color);
}

/**
 * Create demo HTML page for testing
 */
html_element_t* html_create_demo_page(void) {
    html_element_t* root = (html_element_t*)malloc(sizeof(html_element_t));
    if (!root) return NULL;

    memset(root, 0, sizeof(html_element_t));
    root->type = HTML_TAG_HTML;
    root->tag_name = strdup("html");

    // Create body element
    html_element_t* body = (html_element_t*)malloc(sizeof(html_element_t));
    if (!body) {
        free(root);
        return NULL;
    }

    memset(body, 0, sizeof(html_element_t));
    body->type = HTML_TAG_BODY;
    body->tag_name = strdup("body");
    body->parent = root;

    // Create heading
    html_element_t* h1 = (html_element_t*)malloc(sizeof(html_element_t));
    if (!h1) {
        free(body);
        free(root);
        return NULL;
    }

    memset(h1, 0, sizeof(html_element_t));
    h1->type = HTML_TAG_H1;
    h1->tag_name = strdup("h1");
    h1->content = strdup("Welcome to CompileOS Hero Browser!");
    h1->parent = body;

    // Create paragraph
    html_element_t* p1 = (html_element_t*)malloc(sizeof(html_element_t));
    if (!p1) {
        free(h1);
        free(body);
        free(root);
        return NULL;
    }

    memset(p1, 0, sizeof(html_element_t));
    p1->type = HTML_TAG_P;
    p1->tag_name = strdup("p");
    p1->content = strdup("This is a demonstration of the CompileOS Hero web browser with full TCP/IP networking support.");
    p1->parent = body;

    // Create link
    html_element_t* link = (html_element_t*)malloc(sizeof(html_element_t));
    if (!link) {
        free(p1);
        free(h1);
        free(body);
        free(root);
        return NULL;
    }

    memset(link, 0, sizeof(html_element_t));
    link->type = HTML_TAG_A;
    link->tag_name = strdup("a");
    link->content = strdup("Visit CompileOS Homepage");
    link->attributes[0] = strdup("href=http://compileos.hero");
    link->parent = body;

    // Link elements together
    body->children = h1;
    h1->next = p1;
    p1->next = link;

    root->children = body;

    return root;
}

/**
 * Free HTML element tree
 */
void html_free_element(html_element_t* element) {
    if (!element) return;

    // Free children first
    html_element_t* child = element->children;
    while (child) {
        html_element_t* next = child->next;
        html_free_element(child);
        child = next;
    }

    // Free attributes
    for (int i = 0; i < 32 && element->attributes[i]; i++) {
        free(element->attributes[i]);
    }

    // Free content
    if (element->content) {
        free(element->content);
    }

    if (element->tag_name) {
        free(element->tag_name);
    }

    free(element);
}

/**
 * Get HTML attribute value
 */
char* html_get_attribute(html_element_t* element, const char* name) {
    if (!element || !name) return NULL;

    for (int i = 0; i < 32 && element->attributes[i]; i++) {
        char* attr = element->attributes[i];
        if (strncmp(attr, name, strlen(name)) == 0 && attr[strlen(name)] == '=') {
            return attr + strlen(name) + 1;
        }
    }

    return NULL;
}

/**
 * HTTP GET request
 */
int browser_http_get(const char* url, char* buffer, size_t buffer_size) {
    if (!url || !buffer) return -1;

    http_request_t* req = http_request_create(url);
    if (!req) return -1;

    if (http_request_send(req) != 0) {
        http_request_destroy(req);
        return -1;
    }

    int bytes = http_request_recv(req);
    if (bytes > 0) {
        strncpy(buffer, req->body, buffer_size - 1);
        buffer[buffer_size - 1] = '\0';
    }

    http_request_destroy(req);
    return bytes;
}

/**
 * Navigation functions
 */
void browser_back(void) {
    strcpy(g_browser_state.status_message, "Back navigation not implemented");
}

void browser_forward(void) {
    strcpy(g_browser_state.status_message, "Forward navigation not implemented");
}

void browser_refresh(void) {
    browser_load_url(g_browser_state.current_url);
}

void browser_home(void) {
    browser_load_url("http://example.com");
}

/**
 * Browser settings
 */
void browser_set_user_agent(const char* user_agent) {
    // Set custom user agent (simplified)
}

void browser_set_enable_images(bool enable) {
    g_browser_state.show_images = enable;
}

void browser_set_enable_javascript(bool enable) {
    g_browser_state.enable_javascript = enable;
}
