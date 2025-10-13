/**
 * CompileOS Process Management Implementation
 * Simple round-robin scheduler
 */

#include "process.h"
#include "memory/kmalloc.h"
#include <string.h>

// Process table
#define MAX_PROCESSES 64
static process_t* process_table[MAX_PROCESSES];
static uint32_t next_pid = 1;

// Current and ready queue
static process_t* current_process = NULL;
static process_t* ready_queue_head = NULL;
static process_t* ready_queue_tail = NULL;

// Kernel idle process
static process_t idle_process;

/**
 * Idle process - runs when no other process is ready
 */
static void idle_task(void) {
    while (1) {
        __asm__ volatile("hlt");  // Halt until next interrupt
    }
}

/**
 * Add process to ready queue
 */
static void enqueue_process(process_t* proc) {
    proc->next = NULL;
    if (ready_queue_tail) {
        ready_queue_tail->next = proc;
        ready_queue_tail = proc;
    } else {
        ready_queue_head = ready_queue_tail = proc;
    }
}

/**
 * Remove process from ready queue
 */
static process_t* dequeue_process(void) {
    if (!ready_queue_head) return NULL;
    
    process_t* proc = ready_queue_head;
    ready_queue_head = proc->next;
    if (!ready_queue_head) {
        ready_queue_tail = NULL;
    }
    proc->next = NULL;
    return proc;
}

/**
 * Initialize process management
 */
void process_init(void) {
    // Clear process table
    memset(process_table, 0, sizeof(process_table));
    
    // Set up idle process
    idle_process.pid = 0;
    strcpy(idle_process.name, "idle");
    idle_process.state = PROCESS_STATE_READY;
    idle_process.next = NULL;
    
    current_process = &idle_process;
}

/**
 * Create a new process
 */
process_t* process_create(const char* name, void (*entry_point)(void)) {
    // Allocate PCB
    process_t* proc = (process_t*)kmalloc(sizeof(process_t));
    if (!proc) return NULL;
    
    // Allocate kernel stack (4KB)
    uint32_t* stack = (uint32_t*)kmalloc(4096);
    if (!stack) {
        kfree(proc);
        return NULL;
    }
    
    // Initialize PCB
    proc->pid = next_pid++;
    strncpy(proc->name, name, sizeof(proc->name) - 1);
    proc->name[sizeof(proc->name) - 1] = '\0';
    proc->state = PROCESS_STATE_READY;
    proc->page_directory = NULL;  // TODO: Set up when we have paging
    proc->next = NULL;
    
    // Set up initial stack
    proc->kernel_stack = (uint32_t)stack + 4096;  // Stack grows down
    
    // Initialize context for first run
    memset(&proc->context, 0, sizeof(cpu_context_t));
    proc->context.eip = (uint32_t)entry_point;
    proc->context.esp = proc->kernel_stack;
    proc->context.eflags = 0x202;  // Interrupts enabled
    proc->context.cs = 0x08;       // Kernel code segment
    proc->context.ds = 0x10;       // Kernel data segment
    proc->context.es = 0x10;
    proc->context.fs = 0x10;
    proc->context.gs = 0x10;
    proc->context.ss = 0x10;
    
    // Add to process table
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (!process_table[i]) {
            process_table[i] = proc;
            break;
        }
    }
    
    // Add to ready queue
    enqueue_process(proc);
    
    return proc;
}

/**
 * Get current process
 */
process_t* process_current(void) {
    return current_process;
}

/**
 * Terminate current process
 */
void process_exit(int exit_code) {
    if (!current_process || current_process == &idle_process) {
        return;  // Can't exit idle process
    }
    
    current_process->state = PROCESS_STATE_TERMINATED;
    
    // Remove from process table
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i] == current_process) {
            process_table[i] = NULL;
            break;
        }
    }
    
    // Free resources
    if (current_process->kernel_stack) {
        kfree((void*)(current_process->kernel_stack - 4096));
    }
    
    // Schedule next process
    process_schedule();
    
    // Suppress unused parameter warning
    (void)exit_code;
}

/**
 * Context switch (defined in process.asm)
 */
extern void context_switch(cpu_context_t* old_context, cpu_context_t* new_context);

/**
 * Schedule next process (round-robin)
 */
void process_schedule(void) {
    // Save current process state if it's still running
    if (current_process && current_process->state == PROCESS_STATE_RUNNING) {
        current_process->state = PROCESS_STATE_READY;
        if (current_process != &idle_process) {
            enqueue_process(current_process);
        }
    }
    
    // Get next process from ready queue
    process_t* next_process = dequeue_process();
    
    // If no process ready, run idle
    if (!next_process) {
        next_process = &idle_process;
    }
    
    // Switch to next process
    process_t* old_process = current_process;
    current_process = next_process;
    current_process->state = PROCESS_STATE_RUNNING;
    
    // Perform context switch if processes are different
    if (old_process != current_process) {
        context_switch(&old_process->context, &current_process->context);
    }
}

/**
 * Yield CPU voluntarily
 */
void process_yield(void) {
    process_schedule();
}
