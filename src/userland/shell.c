/**
 * CompileOS Shell - Interactive Command Shell
 * 
 * A simple shell for running commands and programs
 */

#include "../runtime/runtime.h"
#include <string.h>

#define MAX_ARGS 16
#define MAX_INPUT 256

// Built-in commands
static int cmd_cd(int argc, char** argv);
static int cmd_pwd(int argc, char** argv);
static int cmd_exit(int argc, char** argv);

typedef struct {
    const char* name;
    int (*handler)(int argc, char** argv);
} builtin_cmd_t;

static builtin_cmd_t builtins[] = {
    {"cd", cmd_cd},
    {"pwd", cmd_pwd},
    {"exit", cmd_exit},
    {NULL, NULL}
};

// Current directory
static char current_dir[256] = "/";

// Built-in command implementations
static int cmd_cd(int argc, char** argv) {
    if (argc < 2) {
        strncpy(current_dir, "/", sizeof(current_dir));
    } else {
        strncpy(current_dir, argv[1], sizeof(current_dir) - 1);
        current_dir[sizeof(current_dir) - 1] = '\0';
    }
    return 0;
}

static int cmd_pwd(int argc, char** argv) {
    (void)argc;
    (void)argv;
    println(current_dir);
    return 0;
}

static int cmd_exit(int argc, char** argv) {
    (void)argc;
    (void)argv;
    println("Goodbye!");
    syscall_exit(0);
    return 0;
}

// Parse command line
static int parse_command(char* input, char** argv) {
    int argc = 0;
    char* token = input;
    bool in_token = false;
    
    for (size_t i = 0; input[i] && argc < MAX_ARGS; i++) {
        if (input[i] == ' ' || input[i] == '\t' || input[i] == '\n') {
            if (in_token) {
                input[i] = '\0';
                in_token = false;
            }
        } else {
            if (!in_token) {
                argv[argc++] = &input[i];
                in_token = true;
            }
        }
    }
    
    argv[argc] = NULL;
    return argc;
}

// Execute command
static int execute_command(int argc, char** argv) {
    if (argc == 0) return 0;
    
    // Check built-ins
    for (int i = 0; builtins[i].name != NULL; i++) {
        if (strcmp(argv[0], builtins[i].name) == 0) {
            return builtins[i].handler(argc, argv);
        }
    }
    
    // Try to spawn external program
    int pid = rt_spawn(argv[0], argv);
    if (pid > 0) {
        return rt_wait(pid);
    }
    
    printf_rt("Command not found: %s\n", argv[0]);
    return -1;
}

// Main shell loop
int shell_main(void) {
    char input[MAX_INPUT];
    char* argv[MAX_ARGS];
    
    println("CompileOS Shell v1.0");
    println("Type 'exit' to quit");
    println("");
    
    while (1) {
        // Display prompt
        print(current_dir);
        print("> ");
        
        // Read input
        char* line = readline();
        if (!line || strlen(line) == 0) {
            continue;
        }
        
        // Copy to input buffer
        strncpy(input, line, MAX_INPUT - 1);
        input[MAX_INPUT - 1] = '\0';
        
        // Parse and execute
        int argc = parse_command(input, argv);
        if (argc > 0) {
            execute_command(argc, argv);
        }
    }
    
    return 0;
}
