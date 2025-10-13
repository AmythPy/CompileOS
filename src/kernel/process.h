/**
 * CompileOS Process Management
 * Basic multitasking and process structures
 */

#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>

// Process states
typedef enum {
    PROCESS_STATE_READY,
    PROCESS_STATE_RUNNING,
    PROCESS_STATE_BLOCKED,
    PROCESS_STATE_TERMINATED
} process_state_t;

// CPU context for context switching
typedef struct {
    uint32_t eax, ebx, ecx, edx;
    uint32_t esi, edi, ebp, esp;
    uint32_t eip;
    uint32_t eflags;
    uint32_t cs, ds, es, fs, gs, ss;
} cpu_context_t;

// Process Control Block (PCB)
typedef struct process {
    uint32_t pid;                   // Process ID
    char name[64];                  // Process name
    process_state_t state;          // Current state
    cpu_context_t context;          // Saved CPU context
    uint32_t* page_directory;       // Page directory (for virtual memory)
    uint32_t kernel_stack;          // Kernel stack pointer
    uint32_t user_stack;            // User stack pointer
    struct process* next;           // Next process in queue
} process_t;

// Initialize process management
void process_init(void);

// Create a new process
process_t* process_create(const char* name, void (*entry_point)(void));

// Terminate current process
void process_exit(int exit_code);

// Get current process
process_t* process_current(void);

// Yield CPU to next process
void process_yield(void);

// Schedule next process (called by timer interrupt)
void process_schedule(void);

#endif // PROCESS_H
