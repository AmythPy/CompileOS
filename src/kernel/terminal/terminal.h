/**
 * CompileOS Terminal System - Header
 * 
 * Full-featured terminal with command system and REPL
 */

#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// Terminal modes
typedef enum {
    TERMINAL_MODE_NORMAL = 0,
    TERMINAL_MODE_INSERT,
    TERMINAL_MODE_COMMAND,
    TERMINAL_MODE_DEBUG,
    TERMINAL_MODE_REPL
} terminal_mode_t;

// Terminal colors
typedef enum {
    TERMINAL_COLOR_BLACK = 0,
    TERMINAL_COLOR_RED,
    TERMINAL_COLOR_GREEN,
    TERMINAL_COLOR_YELLOW,
    TERMINAL_COLOR_BLUE,
    TERMINAL_COLOR_MAGENTA,
    TERMINAL_COLOR_CYAN,
    TERMINAL_COLOR_WHITE,
    TERMINAL_COLOR_BRIGHT_BLACK,
    TERMINAL_COLOR_BRIGHT_RED,
    TERMINAL_COLOR_BRIGHT_GREEN,
    TERMINAL_COLOR_BRIGHT_YELLOW,
    TERMINAL_COLOR_BRIGHT_BLUE,
    TERMINAL_COLOR_BRIGHT_MAGENTA,
    TERMINAL_COLOR_BRIGHT_CYAN,
    TERMINAL_COLOR_BRIGHT_WHITE
} terminal_color_t;

// Terminal cursor
typedef struct {
    uint16_t x;
    uint16_t y;
    bool visible;
    bool blink;
} terminal_cursor_t;

// Terminal line
typedef struct {
    char* text;
    size_t length;
    size_t capacity;
    terminal_color_t foreground;
    terminal_color_t background;
    bool bold;
    bool italic;
    bool underline;
} terminal_line_t;

// Terminal history
typedef struct {
    char** commands;
    size_t count;
    size_t capacity;
    size_t current;
} terminal_history_t;

// Terminal command
typedef struct {
    char* name;
    char* description;
    int (*handler)(int argc, char** argv);
    bool requires_args;
    int min_args;
    int max_args;
} terminal_command_t;

// Terminal configuration
typedef struct {
    uint16_t width;
    uint16_t height;
    terminal_color_t default_foreground;
    terminal_color_t default_background;
    bool auto_wrap;
    bool show_cursor;
    bool enable_history;
    size_t max_history;
    size_t max_line_length;
} terminal_config_t;

// Terminal state
typedef struct {
    terminal_mode_t mode;
    terminal_cursor_t cursor;
    terminal_line_t* lines;
    size_t line_count;
    size_t line_capacity;
    terminal_history_t history;
    terminal_config_t config;
    char* current_line;
    size_t current_line_length;
    size_t current_line_capacity;
    bool running;
} terminal_state_t;

// Terminal initialization and control
int terminal_init(void);
void terminal_shutdown(void);
int terminal_start(void);
void terminal_stop(void);
bool terminal_is_running(void);

// Terminal display
int terminal_clear(void);
int terminal_clear_line(void);
int terminal_clear_screen(void);
int terminal_refresh(void);
int terminal_redraw(void);

// Cursor control
int terminal_set_cursor(uint16_t x, uint16_t y);
terminal_cursor_t terminal_get_cursor(void);
int terminal_move_cursor(int dx, int dy);
int terminal_cursor_home(void);
int terminal_cursor_end(void);
int terminal_cursor_next_line(void);
int terminal_cursor_prev_line(void);
int terminal_set_cursor_visible(bool visible);

// Character and text output
int terminal_putchar(char c);
int terminal_putchar_at(uint16_t x, uint16_t y, char c);
int terminal_puts(const char* str);
int terminal_puts_at(uint16_t x, uint16_t y, const char* str);
int terminal_printf(const char* format, ...);
int terminal_printf_at(uint16_t x, uint16_t y, const char* format, ...);

// Color and formatting
int terminal_set_color(terminal_color_t foreground, terminal_color_t background);
int terminal_set_foreground(terminal_color_t color);
int terminal_set_background(terminal_color_t color);
int terminal_set_bold(bool bold);
int terminal_set_italic(bool italic);
int terminal_set_underline(bool underline);
int terminal_reset_formatting(void);

// Line management
int terminal_add_line(const char* text);
int terminal_insert_line(size_t index, const char* text);
int terminal_delete_line(size_t index);
// int terminal_clear_line(size_t index);
int terminal_get_line(size_t index, char* buffer, size_t buffer_size);
int terminal_set_line(size_t index, const char* text);

// Input handling
int terminal_handle_input(char c);
int terminal_handle_key(uint16_t key_code);
int terminal_read_line(char* buffer, size_t buffer_size);
int terminal_read_char(char* c);

// Command system
int terminal_register_command(const terminal_command_t* command);
int terminal_unregister_command(const char* name);
int terminal_execute_command(const char* command);
int terminal_parse_command(const char* input, int* argc, char*** argv);
int terminal_list_commands(char* output, size_t output_size);

// Built-in commands
int terminal_cmd_help(int argc, char** argv);
int terminal_cmd_clear(int argc, char** argv);
int terminal_cmd_history(int argc, char** argv);
int terminal_cmd_exit(int argc, char** argv);
int terminal_cmd_ls(int argc, char** argv);
int terminal_cmd_cd(int argc, char** argv);
int terminal_cmd_pwd(int argc, char** argv);
int terminal_cmd_memory(int argc, char** argv);
int terminal_cmd_debug(int argc, char** argv);
int terminal_cmd_repl(int argc, char** argv);

// REPL (Read-Eval-Print Loop)
int terminal_repl_start(void);
int terminal_repl_stop(void);
bool terminal_repl_is_active(void);
int terminal_repl_evaluate(const char* expression, char* output, size_t output_size);
int terminal_repl_register_function(const char* name, void* function);
int terminal_repl_register_variable(const char* name, void* value);

// History management
int terminal_history_add(const char* command);
int terminal_history_get(size_t index, char* buffer, size_t buffer_size);
int terminal_history_clear(void);
int terminal_history_search(const char* pattern, char* output, size_t output_size);
int terminal_history_navigate(int direction);

// Terminal configuration
int terminal_set_config(const terminal_config_t* config);
int terminal_get_config(terminal_config_t* config);
int terminal_set_size(uint16_t width, uint16_t height);

// Terminal modes
int terminal_set_mode(terminal_mode_t mode);
terminal_mode_t terminal_get_mode(void);
int terminal_enter_insert_mode(void);
int terminal_exit_insert_mode(void);
int terminal_enter_command_mode(void);
int terminal_exit_command_mode(void);

// Terminal output
int terminal_print(const char* format, ...);
int terminal_print_error(const char* format, ...);
int terminal_print_warning(const char* format, ...);
int terminal_print_success(const char* format, ...);
int terminal_print_info(const char* format, ...);

// Terminal prompt
int terminal_set_prompt(const char* prompt);
int terminal_get_prompt(char* buffer, size_t buffer_size);
int terminal_display_prompt(void);

// Terminal scrolling
int terminal_scroll_up(void);
int terminal_scroll_down(void);
int terminal_scroll_to_top(void);
int terminal_scroll_to_bottom(void);
int terminal_scroll_to_line(size_t line);

// Terminal selection and copy
int terminal_select_text(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
int terminal_copy_selection(char* buffer, size_t buffer_size);
int terminal_paste_text(const char* text);

// Terminal tabs and windows
int terminal_create_tab(const char* name);
int terminal_switch_tab(size_t index);
int terminal_close_tab(size_t index);
int terminal_list_tabs(char* output, size_t output_size);

// Terminal themes
typedef struct {
    terminal_color_t background;
    terminal_color_t foreground;
    terminal_color_t cursor;
    terminal_color_t selection;
    terminal_color_t error;
    terminal_color_t warning;
    terminal_color_t success;
    terminal_color_t info;
} terminal_theme_t;

int terminal_set_theme(const terminal_theme_t* theme);
int terminal_get_theme(terminal_theme_t* theme);
int terminal_load_theme(const char* name);
int terminal_save_theme(const char* name);

#endif // TERMINAL_H










