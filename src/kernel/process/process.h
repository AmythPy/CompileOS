/**
 * CompileOS Process Management - Header
 * 
 * Process creation, scheduling, and management interfaces
 */

#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// Process states
typedef enum {
    PROCESS_STATE_NEW = 0,
    PROCESS_STATE_READY,
    PROCESS_STATE_RUNNING,
    PROCESS_STATE_BLOCKED,
    PROCESS_STATE_TERMINATED
} process_state_t;

// Process priority levels
typedef enum {
    PROCESS_PRIORITY_LOW = 0,
    PROCESS_PRIORITY_NORMAL,
    PROCESS_PRIORITY_HIGH,
    PROCESS_PRIORITY_CRITICAL
} process_priority_t;

// Process ID type
typedef uint32_t process_id_t;

// Process control block
typedef struct process {
    process_id_t pid;
    process_state_t state;
    process_priority_t priority;
    
    // Memory information
    void* stack_pointer;
    void* heap_start;
    void* heap_end;
    size_t stack_size;
    
    // Scheduling information
    uint64_t cpu_time_used;
    uint64_t last_run_time;
    uint32_t timeslice_remaining;
    
    // Process information
    char name[64];
    uint32_t exit_code;
    
    // Linked list pointers
    struct process* next;
    struct process* prev;
} process_t;

// Process management functions
int process_init(void);
process_id_t process_create(const char* name, void* entry_point, size_t stack_size);
int process_terminate(process_id_t pid, uint32_t exit_code);
int process_suspend(process_id_t pid);
int process_resume(process_id_t pid);
int process_set_priority(process_id_t pid, process_priority_t priority);

// Process information
process_t* process_get_by_pid(process_id_t pid);
process_t* process_get_current(void);
int process_get_list(process_t** processes, size_t max_count, size_t* actual_count);

// Process scheduling
void process_schedule(void);
void process_yield(void);
void process_sleep(uint32_t milliseconds);

// Process statistics
typedef struct {
    uint32_t total_processes;
    uint32_t running_processes;
    uint32_t blocked_processes;
    uint32_t terminated_processes;
} process_stats_t;

void process_get_stats(process_stats_t* stats);

#endif // PROCESS_H

