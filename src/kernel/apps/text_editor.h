/**
 * CompileOS Text Editor Application
 * Full-featured text editor with file operations
 */

#ifndef TEXT_EDITOR_H
#define TEXT_EDITOR_H

#include <stdbool.h>

// Editor constants
#define EDITOR_MAX_LINES 20
#define EDITOR_MAX_LINE_LENGTH 60
#define EDITOR_MAX_FILES 10

// Editor state
typedef struct {
    char lines[EDITOR_MAX_LINES][EDITOR_MAX_LINE_LENGTH];
    int current_line;
    int current_col;
    int total_lines;
    char filename[32];
    bool modified;
    bool insert_mode;
} text_editor_t;

// Text editor functions
int text_editor_init(void);
int text_editor_create_window(void);
int text_editor_handle_input(char input);
int text_editor_draw(void);
int text_editor_load_file(const char* filename);
int text_editor_save_file(const char* filename);
int text_editor_new_file(void);
void text_editor_insert_char(char c);
void text_editor_delete_char(void);
void text_editor_move_cursor(int dx, int dy);

#endif // TEXT_EDITOR_H
