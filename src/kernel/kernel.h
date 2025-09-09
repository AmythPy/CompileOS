/**
 * CompileOS Kernel - Main Header
 * 
 * Core kernel definitions and interfaces
 */

#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// Kernel status enumeration
typedef enum {
    KERNEL_STATUS_UNINITIALIZED = 0,
    KERNEL_STATUS_INITIALIZING,
    KERNEL_STATUS_RUNNING,
    KERNEL_STATUS_ERROR,
    KERNEL_STATUS_HALTED
} kernel_status_t;

// Kernel state structure
typedef struct {
    uint32_t version_major;
    uint32_t version_minor;
    uint32_t version_patch;
    kernel_status_t status;
    uint64_t uptime_ticks;
    uint64_t memory_total;
    uint64_t memory_used;
    uint32_t process_count;
} kernel_state_t;

// Function declarations
void kernel_main(void);
const char* kernel_get_version_string(void);
const kernel_state_t* kernel_get_state(void);

// Kernel panic function
void kernel_panic(const char* message);

// System call interface (to be implemented)
typedef enum {
    SYS_CALL_EXIT = 0,
    SYS_CALL_READ,
    SYS_CALL_WRITE,
    SYS_CALL_OPEN,
    SYS_CALL_CLOSE,
    SYS_CALL_MALLOC,
    SYS_CALL_FREE,
    SYS_CALL_MAX
} syscall_t;

// System call handler type
typedef int (*syscall_handler_t)(uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4);

// System call registration
int syscall_register(syscall_t call, syscall_handler_t handler);
int syscall_invoke(syscall_t call, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4);

#endif // KERNEL_H
