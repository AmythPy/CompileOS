/**
 * CompileOS System Call Implementation
 */

#include "syscall.h"
#include "process.h"
#include "timer.h"
#include "memory/kmalloc.h"
#include "interrupts.h"
#include <stdint.h>

// Syscall registers (passed from user space)
typedef struct {
    uint32_t eax;  // Syscall number
    uint32_t ebx;  // Arg 1
    uint32_t ecx;  // Arg 2
    uint32_t edx;  // Arg 3
    uint32_t esi;  // Arg 4
    uint32_t edi;  // Arg 5
} syscall_regs_t;

// Forward declarations
static int sys_exit(int exit_code);
static int sys_write(int fd, const char* buf, uint32_t count);
static int sys_read(int fd, char* buf, uint32_t count);
static int sys_getpid(void);
static int sys_sleep(uint32_t ms);
static int sys_yield(void);
static void* sys_malloc(uint32_t size);
static int sys_free(void* ptr);

/**
 * Syscall handler (called from INT 0x80)
 */
void syscall_handler(void) {
    uint32_t syscall_num;
    uint32_t arg1, arg2, arg3;
    int result = -1;
    
    // Get syscall number and arguments from registers
    __asm__ volatile(
        "mov %%eax, %0\n"
        "mov %%ebx, %1\n"
        "mov %%ecx, %2\n"
        "mov %%edx, %3\n"
        : "=r"(syscall_num), "=r"(arg1), "=r"(arg2), "=r"(arg3)
    );
    
    // Dispatch syscall
    switch (syscall_num) {
        case SYS_EXIT:
            result = sys_exit((int)arg1);
            break;
            
        case SYS_WRITE:
            result = sys_write((int)arg1, (const char*)arg2, arg3);
            break;
            
        case SYS_READ:
            result = sys_read((int)arg1, (char*)arg2, arg3);
            break;
            
        case SYS_GETPID:
            result = sys_getpid();
            break;
            
        case SYS_SLEEP:
            result = sys_sleep(arg1);
            break;
            
        case SYS_YIELD:
            result = sys_yield();
            break;
            
        case SYS_MALLOC:
            result = (int)sys_malloc(arg1);
            break;
            
        case SYS_FREE:
            result = sys_free((void*)arg1);
            break;
            
        default:
            result = -1;  // Invalid syscall
            break;
    }
    
    // Return result in EAX
    __asm__ volatile("mov %0, %%eax" : : "r"(result));
}

/**
 * Initialize syscall handler
 */
void syscall_init(void) {
    // Register syscall interrupt handler (INT 0x80)
    register_interrupt_handler(128, syscall_handler);
}

/**
 * Syscall implementations
 */

static int sys_exit(int exit_code) {
    process_exit(exit_code);
    return 0;  // Never reached
}

static int sys_write(int fd, const char* buf, uint32_t count) {
    // For now, only support stdout (fd=1) to VGA
    if (fd != 1) return -1;
    
    // Simple VGA output
    volatile uint16_t* vga = (volatile uint16_t*)0xB8000;
    static int vga_pos = 0;
    
    for (uint32_t i = 0; i < count; i++) {
        if (buf[i] == '\n') {
            vga_pos = (vga_pos / 80 + 1) * 80;
        } else {
            vga[vga_pos++] = 0x0F00 | buf[i];
        }
        if (vga_pos >= 80 * 25) vga_pos = 0;
    }
    
    return (int)count;
}

static int sys_read(int fd, char* buf, uint32_t count) {
    // TODO: Implement keyboard input
    (void)fd;
    (void)buf;
    (void)count;
    return 0;
}

static int sys_getpid(void) {
    process_t* proc = process_current();
    return proc ? (int)proc->pid : 0;
}

static int sys_sleep(uint32_t ms) {
    timer_sleep(ms);
    return 0;
}

static int sys_yield(void) {
    process_yield();
    return 0;
}

static void* sys_malloc(uint32_t size) {
    return kmalloc(size);
}

static int sys_free(void* ptr) {
    kfree(ptr);
    return 0;
}
