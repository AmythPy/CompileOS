/**
 * CompileOS Text Editor - Simple Text Editor
 * 
 * A basic text editor for creating and editing files
 */

#include "../runtime/runtime.h"
#include <string.h>

#define MAX_LINES 100
#define MAX_LINE_LENGTH 80

// Editor state
typedef struct {
    char* lines[MAX_LINES];
    size_t line_count;
    size_t cursor_line;
    size_t cursor_col;
    char filename[256];
    bool modified;
    bool running;
} editor_t;

static editor_t g_editor = {0};

// Initialize editor
static void editor_init(const char* filename) {
    g_editor.line_count = 1;
    g_editor.cursor_line = 0;
    g_editor.cursor_col = 0;
    g_editor.modified = false;
    g_editor.running = true;
    
    if (filename) {
        strncpy(g_editor.filename, filename, sizeof(g_editor.filename) - 1);
    } else {
        strcpy(g_editor.filename, "untitled.txt");
    }
    
    // Allocate first line
    g_editor.lines[0] = (char*)rt_malloc(MAX_LINE_LENGTH);
    if (g_editor.lines[0]) {
        g_editor.lines[0][0] = '\0';
    }
}

// Draw editor UI
static void editor_draw(void) {
    println("\n╔════════════════════════════════════════════════════════════════════════════╗");
    printf_rt("║ CompileOS Text Editor - %s %s                                    ║\n", 
             g_editor.filename, g_editor.modified ? "*" : " ");
    println("╠════════════════════════════════════════════════════════════════════════════╣");
    
    // Draw lines
    for (size_t i = 0; i < g_editor.line_count && i < 20; i++) {
        if (g_editor.lines[i]) {
            printf_rt("║ %2d │ %-70s ║\n", (int)(i + 1), g_editor.lines[i]);
        }
    }
    
    println("╠════════════════════════════════════════════════════════════════════════════╣");
    println("║ Commands: :w (save) :q (quit) :wq (save & quit) i (insert) ESC (command)  ║");
    println("╚════════════════════════════════════════════════════════════════════════════╝");
    printf_rt("\nLine %d, Col %d > ", (int)(g_editor.cursor_line + 1), (int)(g_editor.cursor_col + 1));
}

// Save file
static int editor_save(void) {
    // Build content
    char* content = (char*)rt_malloc(MAX_LINES * MAX_LINE_LENGTH);
    if (!content) return -1;
    
    content[0] = '\0';
    for (size_t i = 0; i < g_editor.line_count; i++) {
        if (g_editor.lines[i]) {
            strcat(content, g_editor.lines[i]);
            strcat(content, "\n");
        }
    }
    
    // Write to file
    int result = rt_file_write(g_editor.filename, content, strlen(content));
    rt_free(content);
    
    if (result >= 0) {
        g_editor.modified = false;
        println("File saved!");
        return 0;
    }
    
    println("Error saving file!");
    return -1;
}

// Process command
static void editor_command(const char* cmd) {
    if (strcmp(cmd, ":w") == 0) {
        editor_save();
    } else if (strcmp(cmd, ":q") == 0) {
        if (g_editor.modified) {
            println("File modified! Use :wq to save and quit, or :q! to quit without saving.");
        } else {
            g_editor.running = false;
        }
    } else if (strcmp(cmd, ":wq") == 0) {
        if (editor_save() == 0) {
            g_editor.running = false;
        }
    } else if (strcmp(cmd, ":q!") == 0) {
        g_editor.running = false;
    } else {
        println("Unknown command!");
    }
}

// Main editor loop
int texteditor_main(const char* filename) {
    editor_init(filename);
    
    // Try to load file if it exists
    if (filename && rt_file_exists(filename)) {
        char buffer[MAX_LINES * MAX_LINE_LENGTH];
        int bytes = rt_file_read(filename, buffer, sizeof(buffer));
        if (bytes > 0) {
            // TODO: Parse lines from buffer
            println("File loaded!");
        }
    }
    
    while (g_editor.running) {
        editor_draw();
        
        // Read command
        char* line = readline();
        if (!line || strlen(line) == 0) {
            continue;
        }
        
        // Process command
        if (line[0] == ':') {
            editor_command(line);
        } else {
            // Insert text
            if (g_editor.lines[g_editor.cursor_line]) {
                strncpy(g_editor.lines[g_editor.cursor_line], line, MAX_LINE_LENGTH - 1);
                g_editor.modified = true;
            }
        }
    }
    
    // Cleanup
    for (size_t i = 0; i < g_editor.line_count; i++) {
        if (g_editor.lines[i]) {
            rt_free(g_editor.lines[i]);
        }
    }
    
    println("\nEditor closed.");
    return 0;
}
