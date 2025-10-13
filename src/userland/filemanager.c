/**
 * CompileOS File Manager - Interactive File Browser
 * 
 * A simple file manager for browsing and managing files
 */

#include "../runtime/runtime.h"
#include "../kernel/fs/fs.h"
#include <string.h>

#define MAX_FILES 64

// File manager state
typedef struct {
    char current_path[256];
    fs_file_info_t files[MAX_FILES];
    size_t file_count;
    size_t selected;
    bool running;
} filemanager_t;

static filemanager_t g_fm = {0};

// Initialize file manager
static void fm_init(void) {
    strcpy(g_fm.current_path, "/");
    g_fm.selected = 0;
    g_fm.running = true;
    g_fm.file_count = 0;
}

// Refresh file list
static void fm_refresh(void) {
    fs_list(g_fm.current_path, g_fm.files, MAX_FILES, &g_fm.file_count);
}

// Draw file manager UI
static void fm_draw(void) {
    println("\n╔════════════════════════════════════════════════════════════════════════════╗");
    printf_rt("║ CompileOS File Manager - %s%-50s ║\n", g_fm.current_path, "");
    println("╠════════════════════════════════════════════════════════════════════════════╣");
    println("║ Type  │ Name                                    │ Size                      ║");
    println("╠════════════════════════════════════════════════════════════════════════════╣");
    
    // Draw files
    for (size_t i = 0; i < g_fm.file_count && i < 15; i++) {
        char type = (g_fm.files[i].type == FS_TYPE_DIRECTORY) ? 'D' : 'F';
        char marker = (i == g_fm.selected) ? '>' : ' ';
        
        printf_rt("║ %c [%c] │ %-40s│ %8u bytes          ║\n",
                 marker, type, g_fm.files[i].name, (unsigned int)g_fm.files[i].size);
    }
    
    println("╠════════════════════════════════════════════════════════════════════════════╣");
    println("║ Commands: ls, cd <dir>, cat <file>, rm <file>, mkdir <dir>, touch <file>  ║");
    println("║           cp <src> <dst>, mv <src> <dst>, exit                             ║");
    println("╚════════════════════════════════════════════════════════════════════════════╝");
    print("\nCommand: ");
}

// Process command
static void fm_command(int argc, char** argv) {
    if (argc == 0) return;
    
    if (strcmp(argv[0], "ls") == 0) {
        fm_refresh();
    } else if (strcmp(argv[0], "cd") == 0 && argc > 1) {
        strncpy(g_fm.current_path, argv[1], sizeof(g_fm.current_path) - 1);
        fm_refresh();
    } else if (strcmp(argv[0], "cat") == 0 && argc > 1) {
        char buffer[1024];
        int bytes = rt_file_read(argv[1], buffer, sizeof(buffer) - 1);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            println("\n--- File Contents ---");
            println(buffer);
            println("--- End ---");
        } else {
            println("Error reading file!");
        }
    } else if (strcmp(argv[0], "rm") == 0 && argc > 1) {
        if (fs_delete(argv[1]) == 0) {
            println("File deleted!");
            fm_refresh();
        } else {
            println("Error deleting file!");
        }
    } else if (strcmp(argv[0], "mkdir") == 0 && argc > 1) {
        if (fs_mkdir(argv[1]) == 0) {
            println("Directory created!");
            fm_refresh();
        } else {
            println("Error creating directory!");
        }
    } else if (strcmp(argv[0], "touch") == 0 && argc > 1) {
        if (fs_create(argv[1]) == 0) {
            println("File created!");
            fm_refresh();
        } else {
            println("Error creating file!");
        }
    } else if (strcmp(argv[0], "cp") == 0 && argc > 2) {
        if (fs_copy(argv[1], argv[2]) == 0) {
            println("File copied!");
            fm_refresh();
        } else {
            println("Error copying file!");
        }
    } else if (strcmp(argv[0], "mv") == 0 && argc > 2) {
        if (fs_rename(argv[1], argv[2]) == 0) {
            println("File moved!");
            fm_refresh();
        } else {
            println("Error moving file!");
        }
    } else if (strcmp(argv[0], "exit") == 0) {
        g_fm.running = false;
    } else {
        println("Unknown command!");
    }
}

// Parse command
static int fm_parse(char* input, char** argv) {
    int argc = 0;
    char* token = input;
    bool in_token = false;
    
    for (size_t i = 0; input[i] && argc < 16; i++) {
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
    
    return argc;
}

// Main file manager loop
int filemanager_main(void) {
    fm_init();
    fm_refresh();
    
    char input[256];
    char* argv[16];
    
    while (g_fm.running) {
        fm_draw();
        
        // Read command
        char* line = readline();
        if (!line || strlen(line) == 0) {
            continue;
        }
        
        // Parse and execute
        strncpy(input, line, sizeof(input) - 1);
        int argc = fm_parse(input, argv);
        fm_command(argc, argv);
    }
    
    println("\nFile Manager closed.");
    return 0;
}
