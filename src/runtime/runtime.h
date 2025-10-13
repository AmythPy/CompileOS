/**
 * CompileOS Runtime Library - Header
 * 
 * Standard runtime functions for userland applications
 */

#ifndef RUNTIME_H
#define RUNTIME_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// Runtime initialization
int runtime_init(void);
void runtime_shutdown(void);

// System calls wrapper
int syscall_exit(int code);
int syscall_write(const char* str);
int syscall_read(char* buffer, size_t size);
int syscall_open(const char* path, int flags);
int syscall_close(int fd);
int syscall_exec(const char* path, char** argv);

// Standard I/O
int print(const char* str);
int println(const char* str);
int printf_rt(const char* format, ...);
char* readline(void);
int getchar_rt(void);

// Memory management
void* rt_malloc(size_t size);
void rt_free(void* ptr);
void* rt_realloc(void* ptr, size_t size);

// String utilities
char* rt_strdup(const char* str);
int rt_atoi(const char* str);
char* rt_itoa(int value, char* buffer);

// File operations
int rt_file_exists(const char* path);
int rt_file_read(const char* path, char* buffer, size_t size);
int rt_file_write(const char* path, const char* data, size_t size);
int rt_file_append(const char* path, const char* data, size_t size);

// Process management
int rt_spawn(const char* program, char** args);
int rt_wait(int pid);
int rt_kill(int pid);
int rt_getpid(void);

// Time functions
uint64_t rt_get_ticks(void);
void rt_sleep(uint32_t ms);

#endif // RUNTIME_H
