/**
 * CompileOS Runtime Library - Implementation
 * 
 * Standard runtime functions for userland applications
 */

#include "runtime.h"
#include "../kernel/memory/memory.h"
#include "../kernel/fs/fs.h"
#include "../kernel/terminal/terminal.h"
#include <string.h>
#include <stdarg.h>

extern int sprintf(char* str, const char* format, ...);
extern int vsprintf(char* str, const char* format, va_list args);

// Runtime state
static struct {
    bool initialized;
} g_runtime = {0};

// Initialize runtime
int runtime_init(void) {
    if (g_runtime.initialized) {
        return 0;
    }
    
    g_runtime.initialized = true;
    return 0;
}

// Shutdown runtime
void runtime_shutdown(void) {
    g_runtime.initialized = false;
}

// System call wrappers
int syscall_exit(int code) {
    (void)code;
    // TODO: Implement proper exit
    return 0;
}

int syscall_write(const char* str) {
    return terminal_puts(str);
}

int syscall_read(char* buffer, size_t size) {
    (void)buffer;
    (void)size;
    // TODO: Implement read
    return 0;
}

// Standard I/O
int print(const char* str) {
    return terminal_puts(str);
}

int println(const char* str) {
    terminal_puts(str);
    return terminal_putchar('\n');
}

int printf_rt(const char* format, ...) {
    char buffer[512];
    va_list args;
    va_start(args, format);
    int written = vsprintf(buffer, format, args);
    va_end(args);
    
    terminal_puts(buffer);
    return written;
}

char* readline(void) {
    static char buffer[256];
    size_t pos = 0;
    
    while (pos < 255) {
        // TODO: Get character from keyboard
        // For now, return empty string
        break;
    }
    
    buffer[pos] = '\0';
    return buffer;
}

int getchar_rt(void) {
    // TODO: Get character from keyboard
    return 0;
}

// Memory management
void* rt_malloc(size_t size) {
    return memory_alloc(size);
}

void rt_free(void* ptr) {
    memory_free(ptr);
}

void* rt_realloc(void* ptr, size_t size) {
    return memory_realloc(ptr, size);
}

// String utilities
char* rt_strdup(const char* str) {
    if (!str) return NULL;
    
    size_t len = strlen(str);
    char* dup = (char*)rt_malloc(len + 1);
    if (dup) {
        strcpy(dup, str);
    }
    return dup;
}

int rt_atoi(const char* str) {
    if (!str) return 0;
    
    int result = 0;
    int sign = 1;
    
    // Skip whitespace
    while (*str == ' ' || *str == '\t') str++;
    
    // Handle sign
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }
    
    // Convert digits
    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }
    
    return result * sign;
}

char* rt_itoa(int value, char* buffer) {
    if (!buffer) return NULL;
    
    sprintf(buffer, "%d", value);
    return buffer;
}

// File operations
int rt_file_exists(const char* path) {
    return fs_exists(path);
}

int rt_file_read(const char* path, char* buffer, size_t size) {
    fs_fd_t fd = fs_open(path, FS_O_RDONLY);
    if (fd < 0) return -1;
    
    int bytes_read = fs_read(fd, buffer, size);
    fs_close(fd);
    
    return bytes_read;
}

int rt_file_write(const char* path, const char* data, size_t size) {
    fs_fd_t fd = fs_open(path, FS_O_WRONLY | FS_O_CREATE | FS_O_TRUNC);
    if (fd < 0) return -1;
    
    int bytes_written = fs_write(fd, data, size);
    fs_close(fd);
    
    return bytes_written;
}

int rt_file_append(const char* path, const char* data, size_t size) {
    fs_fd_t fd = fs_open(path, FS_O_WRONLY | FS_O_CREATE | FS_O_APPEND);
    if (fd < 0) return -1;
    
    int bytes_written = fs_write(fd, data, size);
    fs_close(fd);
    
    return bytes_written;
}

// Process management (stubs)
int rt_spawn(const char* program, char** args) {
    (void)program;
    (void)args;
    // TODO: Implement process spawning
    return -1;
}

int rt_wait(int pid) {
    (void)pid;
    // TODO: Implement wait
    return 0;
}

int rt_kill(int pid) {
    (void)pid;
    // TODO: Implement kill
    return -1;
}

int rt_getpid(void) {
    // TODO: Get current process ID
    return 1;
}

// Time functions (stubs)
uint64_t rt_get_ticks(void) {
    // TODO: Get system ticks
    return 0;
}

void rt_sleep(uint32_t ms) {
    (void)ms;
    // TODO: Implement sleep
}
