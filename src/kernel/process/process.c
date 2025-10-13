/**
 * CompileOS Process Management - Implementation
 * 
 * Implements process creation, scheduling, and management
 */

#include "process.h"
#include "../kernel.h"
#include "../memory/memory.h"
#include <string.h>

// Process management state
static struct {
    bool initialized;
    process_t* process_list;
    process_t* current_process;
    process_id_t next_pid;
    uint32_t process_count;
} g_process_state = {0};

// Process list management
static void process_add_to_list(process_t* process) {
    if (!g_process_state.process_list) {
        g_process_state.process_list = process;
        process->next = process;
        process->prev = process;
    } else {
        process->next = g_process_state.process_list;
        process->prev = g_process_state.process_list->prev;
        g_process_state.process_list->prev->next = process;
        g_process_state.process_list->prev = process;
    }
}

static void process_remove_from_list(process_t* process) {
    if (process->next == process) {
        // Only process in list
        g_process_state.process_list = NULL;
    } else {
        process->prev->next = process->next;
        process->next->prev = process->prev;
        if (g_process_state.process_list == process) {
            g_process_state.process_list = process->next;
        }
    }
}

/**
 * Initialize process management
 */
int process_init(void) {
    if (g_process_state.initialized) {
        return 0;
    }
    
    g_process_state.process_list = NULL;
    g_process_state.current_process = NULL;
    g_process_state.next_pid = 1;
    g_process_state.process_count = 0;
    
    g_process_state.initialized = true;
    return 0;
}

/**
 * Create a new process
 */
process_id_t process_create(const char* name, void* entry_point, size_t stack_size) {
    if (!g_process_state.initialized || !name || !entry_point || stack_size == 0) {
        return 0;
    }
    
    // Allocate process control block
    process_t* process = (process_t*)memory_alloc(sizeof(process_t));
    if (!process) {
        return 0;
    }
    
    // Allocate stack
    void* stack = memory_alloc(stack_size);
    if (!stack) {
        memory_free(process);
        return 0;
    }
    
    // Initialize process
    process->pid = g_process_state.next_pid++;
    process->state = PROCESS_STATE_NEW;
    process->priority = PROCESS_PRIORITY_NORMAL;
    process->stack_pointer = (void*)((char*)stack + stack_size);
    process->stack_size = stack_size;
    process->cpu_time_used = 0;
    process->last_run_time = 0;
    process->timeslice_remaining = 100; // Default timeslice
    process->exit_code = 0;
    
    // Copy name
    strncpy(process->name, name, sizeof(process->name) - 1);
    process->name[sizeof(process->name) - 1] = '\0';
    
    // Add to process list
    process_add_to_list(process);
    g_process_state.process_count++;
    
    // Set to ready state
    process->state = PROCESS_STATE_READY;
    
    return process->pid;
}

/**
 * Terminate a process
 */
int process_terminate(process_id_t pid, uint32_t exit_code) {
    process_t* process = process_get_by_pid(pid);
    if (!process) {
        return -1;
    }
    
    process->state = PROCESS_STATE_TERMINATED;
    process->exit_code = exit_code;
    
    // Free stack memory
    void* stack = (void*)((char*)process->stack_pointer - process->stack_size);
    memory_free(stack);
    
    // Remove from process list
    process_remove_from_list(process);
    g_process_state.process_count--;
    
    // Free process control block
    memory_free(process);
    
    return 0;
}

/**
 * Suspend a process
 */
int process_suspend(process_id_t pid) {
    process_t* process = process_get_by_pid(pid);
    if (!process || process->state != PROCESS_STATE_RUNNING) {
        return -1;
    }
    
    process->state = PROCESS_STATE_BLOCKED;
    return 0;
}

/**
 * Resume a process
 */
int process_resume(process_id_t pid) {
    process_t* process = process_get_by_pid(pid);
    if (!process || process->state != PROCESS_STATE_BLOCKED) {
        return -1;
    }
    
    process->state = PROCESS_STATE_READY;
    return 0;
}

/**
 * Set process priority
 */
int process_set_priority(process_id_t pid, process_priority_t priority) {
    process_t* process = process_get_by_pid(pid);
    if (!process) {
        return -1;
    }
    
    process->priority = priority;
    return 0;
}

/**
 * Get process by PID
 */
process_t* process_get_by_pid(process_id_t pid) {
    if (!g_process_state.process_list) {
        return NULL;
    }
    
    process_t* current = g_process_state.process_list;
    do {
        if (current->pid == pid) {
            return current;
        }
        current = current->next;
    } while (current != g_process_state.process_list);
    
    return NULL;
}

/**
 * Get current process
 */
process_t* process_get_current(void) {
    return g_process_state.current_process;
}

/**
 * Get process list
 */
int process_get_list(process_t** processes, size_t max_count, size_t* actual_count) {
    if (!processes || !actual_count) {
        return -1;
    }
    
    *actual_count = 0;
    
    if (!g_process_state.process_list) {
        return 0;
    }
    
    process_t* current = g_process_state.process_list;
    do {
        if (*actual_count >= max_count) {
            break;
        }
        
        processes[*actual_count] = current;
        (*actual_count)++;
        
        current = current->next;
    } while (current != g_process_state.process_list);
    
    return 0;
}

/**
 * Process scheduler (round-robin with priority)
 */
void process_schedule(void) {
    if (!g_process_state.process_list) {
        return;
    }
    
    // Find the next ready process
    process_t* next_process = NULL;
    process_t* current = g_process_state.process_list;
    
    do {
        if (current->state == PROCESS_STATE_READY) {
            if (!next_process || current->priority > next_process->priority) {
                next_process = current;
            }
        }
        current = current->next;
    } while (current != g_process_state.process_list);
    
    // Switch to the next process
    if (next_process && next_process != g_process_state.current_process) {
        if (g_process_state.current_process) {
            g_process_state.current_process->state = PROCESS_STATE_READY;
        }
        
        g_process_state.current_process = next_process;
        next_process->state = PROCESS_STATE_RUNNING;
    }
}

/**
 * Yield CPU to another process
 */
void process_yield(void) {
    if (g_process_state.current_process) {
        g_process_state.current_process->state = PROCESS_STATE_READY;
    }
    process_schedule();
}

/**
 * Put process to sleep
 */
void process_sleep(uint32_t milliseconds) {
    (void)milliseconds; // Suppress unused parameter warning
    
    if (g_process_state.current_process) {
        g_process_state.current_process->state = PROCESS_STATE_BLOCKED;
        // TODO: Implement sleep timer
    }
    process_schedule();
}

/**
 * Get process statistics
 */
void process_get_stats(process_stats_t* stats) {
    if (!stats) {
        return;
    }
    
    stats->total_processes = g_process_state.process_count;
    stats->running_processes = 0;
    stats->blocked_processes = 0;
    stats->terminated_processes = 0;
    
    if (!g_process_state.process_list) {
        return;
    }
    
    process_t* current = g_process_state.process_list;
    do {
        switch (current->state) {
            case PROCESS_STATE_RUNNING:
                stats->running_processes++;
                break;
            case PROCESS_STATE_BLOCKED:
                stats->blocked_processes++;
                break;
            case PROCESS_STATE_TERMINATED:
                stats->terminated_processes++;
                break;
            default:
                break;
        }
        current = current->next;
    } while (current != g_process_state.process_list);
}










