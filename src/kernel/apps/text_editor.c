/**
 * CompileOS Text Editor Implementation
 * Professional text editor with full editing capabilities
 */

#include "text_editor.h"
#include "../drivers/vga/vga.h"
#include <string.h>
#include <stdlib.h>
char* strtok(char* str, const char* delim) {
    static char* last = NULL;
    if (str) last = str;

    if (!last) return NULL;

    // Skip leading delimiters
    while (*last && strchr(delim, *last)) last++;

    if (*last == '\0') return NULL;

    char* token = last;

    // Find end of token
    while (*last && !strchr(delim, *last)) last++;

    if (*last) {
        *last = '\0';
        last++;
    }

    return token;
}

// Global editor state
static text_editor_t g_editor_state = {0};

// Sample file content for demonstration
static const char* sample_files[][2] = {
    {"README.md", "# CompileOS\n\nA professional operating system.\n\n## Features\n- Mouse support\n- Windowing system\n- Multiple applications"},
    {"config.txt", "# System Configuration\n\nmemory=512MB\nresolution=80x25\nmouse=enabled"},
    {"notes.txt", "Development notes for CompileOS:\n\n- Implement calculator\n- Add text editor\n- Create file manager"}
};

/**
 * Initialize text editor
 */
int text_editor_init(void) {
    text_editor_new_file();
    strcpy(g_editor_state.filename, "untitled.txt");
    g_editor_state.insert_mode = true;
    return 0;
}

/**
 * Create text editor window
 */
int text_editor_create_window(void) {
    return text_editor_init();
}

/**
 * Handle text editor input
 */
int text_editor_handle_input(char input) {
    if (!input) return 0;

    switch (input) {
        case '\n':  // Enter key
            if (g_editor_state.current_line < EDITOR_MAX_LINES - 1) {
                g_editor_state.current_line++;
                g_editor_state.current_col = 0;
            }
            break;

        case '\b':  // Backspace
            text_editor_delete_char();
            break;

        case 27:  // Escape - toggle insert/overwrite mode
            g_editor_state.insert_mode = !g_editor_state.insert_mode;
            break;

        case ' ':  // Space
        case '\t': // Tab
        default:
            if (input >= 32 && input <= 126) {  // Printable characters
                text_editor_insert_char(input);
            }
            break;
    }

    g_editor_state.modified = true;
    return 0;
}

/**
 * Draw text editor interface
 */
int text_editor_draw(void) {
    // Draw title bar
    vga_puts_at(2, 1, "╔══════════════════════════════════════╗");
    vga_puts_at(2, 2, "║           TEXT EDITOR v1.0           ║");
    vga_puts_at(2, 3, "╚══════════════════════════════════════╝");

    // Draw status bar
    vga_puts_at(3, 21, "┌─ Status: ");
    if (g_editor_state.insert_mode) {
        vga_puts_at(13, 21, "INSERT");
    } else {
        vga_puts_at(13, 21, "OVERWRITE");
    }
    vga_puts_at(22, 21, " │ Line: ");
    char line_num[8];
    snprintf(line_num, sizeof(line_num), "%d", g_editor_state.current_line + 1);
    vga_puts_at(30, 21, line_num);
    vga_puts_at(32, 21, " │ ");
    vga_puts_at(35, 21, g_editor_state.filename);
    vga_puts_at(3, 22, "└──────────────────────────────────────┘");

    // Draw text area
    vga_puts_at(3, 5, "┌─────────────────────────────────────────────────────────────────────────┐");
    for (int y = 6; y <= 19; y++) {
        vga_puts_at(3, y, "│                                                                        │");
    }
    vga_puts_at(3, 20, "└─────────────────────────────────────────────────────────────────────────┘");

    // Draw text content
    for (int line = 0; line < EDITOR_MAX_LINES && line < 14; line++) {
        int display_line = line + 2;  // Start from line 7 (after borders)

        if (line < g_editor_state.total_lines) {
            // Draw line number
            char line_num_str[4];
            snprintf(line_num_str, sizeof(line_num_str), "%2d", line + 1);
            vga_puts_at(4, display_line, line_num_str);

            // Draw line content
            vga_puts_at(7, display_line, g_editor_state.lines[line]);

            // Draw cursor
            if (line == g_editor_state.current_line) {
                int cursor_x = 7 + g_editor_state.current_col;
                if (cursor_x < 71) {  // Stay within bounds
                    vga_puts_at(cursor_x, display_line, "_");
                }
            }
        }
    }

    return 0;
}

/**
 * Load file content
 */
int text_editor_load_file(const char* filename) {
    // Find file in sample files
    for (int i = 0; i < 3; i++) {
        if (strcmp(filename, sample_files[i][0]) == 0) {
            strcpy(g_editor_state.filename, filename);

            // Parse content into lines
            char* content = (char*)sample_files[i][1];
            char* line = strtok(content, "\n");

            g_editor_state.total_lines = 0;
            while (line && g_editor_state.total_lines < EDITOR_MAX_LINES) {
                strncpy(g_editor_state.lines[g_editor_state.total_lines], line, EDITOR_MAX_LINE_LENGTH - 1);
                g_editor_state.lines[g_editor_state.total_lines][EDITOR_MAX_LINE_LENGTH - 1] = '\0';
                g_editor_state.total_lines++;
                line = strtok(NULL, "\n");
            }

            g_editor_state.modified = false;
            return 0;
        }
    }

    return -1;  // File not found
}

/**
 * Save file content
 */
int text_editor_save_file(const char* filename) {
    // In a real implementation, this would write to disk
    strcpy(g_editor_state.filename, filename);
    g_editor_state.modified = false;
    return 0;
}

/**
 * Create new file
 */
int text_editor_new_file(void) {
    memset(g_editor_state.lines, 0, sizeof(g_editor_state.lines));
    g_editor_state.current_line = 0;
    g_editor_state.current_col = 0;
    g_editor_state.total_lines = 0;
    strcpy(g_editor_state.filename, "untitled.txt");
    g_editor_state.modified = false;
    return 0;
}

/**
 * Insert character at cursor position
 */
void text_editor_insert_char(char c) {
    if (g_editor_state.current_line >= EDITOR_MAX_LINES) return;

    char* line = g_editor_state.lines[g_editor_state.current_line];

    if (g_editor_state.insert_mode) {
        // Insert mode - shift characters right
        if (strlen(line) < EDITOR_MAX_LINE_LENGTH - 1) {
            for (int i = strlen(line); i >= g_editor_state.current_col; i--) {
                line[i + 1] = line[i];
            }
            line[g_editor_state.current_col] = c;
            g_editor_state.current_col++;
        }
    } else {
        // Overwrite mode
        if (g_editor_state.current_col < EDITOR_MAX_LINE_LENGTH - 1) {
            line[g_editor_state.current_col] = c;
            g_editor_state.current_col++;
        }
    }
}

/**
 * Delete character at cursor position
 */
void text_editor_delete_char(void) {
    if (g_editor_state.current_line >= EDITOR_MAX_LINES) return;

    char* line = g_editor_state.lines[g_editor_state.current_line];

    if (g_editor_state.current_col > 0) {
        // Delete character before cursor
        for (int i = g_editor_state.current_col - 1; i < strlen(line); i++) {
            line[i] = line[i + 1];
        }
        g_editor_state.current_col--;
    } else if (g_editor_state.current_line > 0) {
        // Join with previous line
        char* prev_line = g_editor_state.lines[g_editor_state.current_line - 1];
        int prev_len = strlen(prev_line);

        if (prev_len + strlen(line) < EDITOR_MAX_LINE_LENGTH - 1) {
            strcat(prev_line, line);

            // Shift all lines up
            for (int i = g_editor_state.current_line; i < g_editor_state.total_lines - 1; i++) {
                strcpy(g_editor_state.lines[i], g_editor_state.lines[i + 1]);
            }
            g_editor_state.total_lines--;
            g_editor_state.current_line--;
            g_editor_state.current_col = strlen(g_editor_state.lines[g_editor_state.current_line]);
        }
    }
}

/**
 * Move cursor
 */
void text_editor_move_cursor(int dx, int dy) {
    g_editor_state.current_col += dx;
    g_editor_state.current_line += dy;

    // Bounds checking
    if (g_editor_state.current_line < 0) g_editor_state.current_line = 0;
    if (g_editor_state.current_line >= EDITOR_MAX_LINES) g_editor_state.current_line = EDITOR_MAX_LINES - 1;

    if (g_editor_state.current_col < 0) g_editor_state.current_col = 0;
    if (g_editor_state.current_col > EDITOR_MAX_LINE_LENGTH - 1) {
        g_editor_state.current_col = EDITOR_MAX_LINE_LENGTH - 1;
    }

    // Extend line if cursor goes beyond current line length
    if (g_editor_state.current_col > strlen(g_editor_state.lines[g_editor_state.current_line])) {
        g_editor_state.current_col = strlen(g_editor_state.lines[g_editor_state.current_line]);
    }
}
