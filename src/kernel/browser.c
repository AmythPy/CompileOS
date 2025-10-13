/**
 * CompileOS Text Browser - HTTP Client
 * Browse the web from CompileOS terminal
 */

#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include "../drivers/vga/vga.h"

// Forward declarations
extern void network_init(void);
extern void network_get_status(char* buffer, size_t size);
extern uint32_t network_dns_lookup(const char* hostname);
extern int network_http_get(const char* url, char* response, size_t max_size);

static bool browser_active = false;
static char browser_buffer[2048];

static char* browser_get_next_line(char* buffer, char** next_ptr) {
    if (!buffer || !*next_ptr) return NULL;

    char* line_start = *next_ptr;
    char* line_end = strchr(line_start, '\n');

    if (line_end) {
        *line_end = '\0';
        *next_ptr = line_end + 1;
    } else {
        *next_ptr = NULL;
    }

    return line_start;
}

// Browser command handler
void browser_handle_command(const char* cmd) {
    if (strcmp(cmd, "exit") == 0) {
        browser_active = false;
        return;
    }

    if (strncmp(cmd, "http://", 7) == 0 || strncmp(cmd, "https://", 8) == 0) {
        // Fetch URL
        memset(browser_buffer, 0, sizeof(browser_buffer));
        int result = network_http_get(cmd, browser_buffer, sizeof(browser_buffer));

        if (result > 0) {
            // Display response (first 10 lines for demo)
            vga_puts_at(12, 16, "=== Web Page Content ===");
            vga_set_foreground_color(VGA_COLOR_LIGHT_CYAN);

            char* line_ptr = browser_buffer;
            char* line = browser_get_next_line(browser_buffer, &line_ptr);
            int line_count = 0;
            int display_y = 17;

            while (line && line_count < 6 && display_y < 22) {
                // Skip HTTP headers for demo
                if (strncmp(line, "HTTP/", 5) != 0 &&
                    strncmp(line, "Content-Type:", 13) != 0 &&
                    strncmp(line, "Server:", 7) != 0 &&
                    strlen(line) > 1) {

                    vga_puts_at(12, display_y++, line);
                    line_count++;
                }
                line = browser_get_next_line(browser_buffer, &line_ptr);
            }

            if (line_count >= 6) {
                vga_puts_at(12, display_y, "... (truncated - use real browser for full content)");
                vga_set_foreground_color(VGA_COLOR_YELLOW);
            }
        } else {
            vga_puts_at(12, 16, "Error: Failed to fetch URL");
            vga_set_foreground_color(VGA_COLOR_LIGHT_RED);
        }
    } else {
        vga_puts_at(12, 16, "Browser Commands:");
        vga_set_foreground_color(VGA_COLOR_YELLOW);
        vga_puts_at(12, 17, "  http://example.com  - Browse URL");
        vga_set_foreground_color(VGA_COLOR_WHITE);
        vga_puts_at(12, 18, "  https://github.com  - Browse HTTPS");
        vga_set_foreground_color(VGA_COLOR_WHITE);
        vga_puts_at(12, 19, "  exit               - Return to terminal");
        vga_set_foreground_color(VGA_COLOR_WHITE);
    }

    // Show browser prompt
    vga_puts_at(12, 23, "Browser> ");
    vga_set_foreground_color(VGA_COLOR_LIGHT_GREEN);
}

// Initialize browser mode
void browser_init(void) {
    browser_active = true;

    // Clear terminal area
    for (int y = 15; y < 24; y++) {
        vga_puts_at(10, y, "|                                                                               |");
    }

    vga_puts_at(12, 15, "CompileOS Text Browser v0.1");
    vga_set_foreground_color(VGA_COLOR_LIGHT_CYAN);
    vga_puts_at(12, 16, "===========================");
    vga_set_foreground_color(VGA_COLOR_WHITE);
    vga_puts_at(12, 17, "Type URLs to browse the web!");
    vga_set_foreground_color(VGA_COLOR_YELLOW);
    vga_puts_at(12, 18, "Examples:");
    vga_set_foreground_color(VGA_COLOR_LIGHT_BLUE);
    vga_puts_at(12, 19, "  http://example.com");
    vga_puts_at(12, 20, "  https://github.com");
    vga_set_foreground_color(VGA_COLOR_WHITE);
    vga_puts_at(12, 21, "");
    vga_puts_at(12, 22, "Browser> ");
    vga_set_foreground_color(VGA_COLOR_LIGHT_GREEN);
}

// Browser main loop
void browser_run(char* input_buffer, int* input_pos) {
    static char browser_cmd[256];

    if (!browser_active) {
        browser_init();
    }

    // Handle input
    if (strlen(input_buffer) > 0) {
        strcpy(browser_cmd, input_buffer);
        browser_handle_command(browser_cmd);
        input_buffer[0] = '\0';
        *input_pos = 0;
    }
}

// Check if browser is active
bool browser_is_active(void) {
    return browser_active;
}

// Stub network functions for compilation
// void network_init(void) {
//     // TODO: Implement network initialization
// }
