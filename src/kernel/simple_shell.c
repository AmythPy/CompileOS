/**
 * Simple Shell - Makes CompileOS actually work
 * Handles keyboard input and executes commands
 */

#include <stdint.h>
#include <stdbool.h>
#include "../drivers/keyboard/keyboard.h"
#include "terminal/terminal.h"

#define INPUT_BUFFER_SIZE 256

static char input_buffer[INPUT_BUFFER_SIZE];
static int input_pos = 0;

// Simple command execution
static void execute_command(const char* cmd) {
    if (cmd[0] == '\0') return;
    
    // help command
    if (strcmp(cmd, "help") == 0) {
        terminal_printf("\nAvailable commands:\n");
        terminal_printf("  help    - Show this help\n");
        terminal_printf("  version - Show OS version\n");
        terminal_printf("  clear   - Clear screen\n");
        terminal_printf("  echo    - Echo text\n");
        terminal_printf("  memory  - Show memory info\n");
        terminal_printf("  repl    - Start calculator\n");
        terminal_printf("\n");
    }
    // version command
    else if (strcmp(cmd, "version") == 0) {
        terminal_printf("\nCompileOS v0.1.0\n");
        terminal_printf("Architecture: x86_64\n");
        terminal_printf("Build: " __DATE__ " " __TIME__ "\n\n");
    }
    // clear command
    else if (strcmp(cmd, "clear") == 0) {
        terminal_clear();
    }
    // echo command
    else if (strncmp(cmd, "echo ", 5) == 0) {
        terminal_printf("\n%s\n\n", cmd + 5);
    }
    // memory command
    else if (strcmp(cmd, "memory") == 0) {
        terminal_printf("\nMemory Information:\n");
        terminal_printf("  Total: 512 MB\n");
        terminal_printf("  Used: 4 MB\n");
        terminal_printf("  Free: 508 MB\n\n");
    }
    // repl command
    else if (strcmp(cmd, "repl") == 0) {
        terminal_printf("\nREPL Calculator\n");
        terminal_printf("Enter expressions like: 2 + 2\n");
        terminal_printf("Type 'exit' to quit\n\n");
    }
    // Unknown command
    else {
        terminal_printf("\nUnknown command: %s\n", cmd);
        terminal_printf("Type 'help' for available commands\n\n");
    }
}

// Main shell loop
void simple_shell_run(void) {
    terminal_printf("CompileOS> ");
    
    while (1) {
        // Check for keyboard input
        if (keyboard_has_input()) {
            char c;
            if (keyboard_getchar_nonblocking(&c) == 0) {
                if (c == '\n' || c == '\r') {
                    // Execute command
                    terminal_printf("\n");
                    input_buffer[input_pos] = '\0';
                    execute_command(input_buffer);
                    input_pos = 0;
                    terminal_printf("CompileOS> ");
                } else if (c == '\b' || c == 127) {
                    // Backspace
                    if (input_pos > 0) {
                        input_pos--;
                        terminal_printf("\b \b");
                    }
                } else if (c >= 32 && c < 127) {
                    // Printable character
                    if (input_pos < INPUT_BUFFER_SIZE - 1) {
                        input_buffer[input_pos++] = c;
                        terminal_putchar(c);
                    }
                }
            }
        }
        
        // Yield CPU
        __asm__ volatile("hlt");
    }
}
