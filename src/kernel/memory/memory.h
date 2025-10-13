/**
 * CompileOS Memory Management - Header
 * 
 * Memory allocation, virtual memory, and memory protection interfaces
 */

#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// Memory statistics structure
typedef struct {
    uint64_t total_memory;
    uint64_t used_memory;
    uint64_t free_memory;
    uint64_t heap_start;
    uint64_t heap_end;
} memory_stats_t;

// Memory allocation functions
int memory_init(void);
void* memory_alloc(size_t size);
void memory_free(void* ptr);
void* memory_realloc(void* ptr, size_t new_size);

// Memory statistics
void memory_get_stats(memory_stats_t* stats);

// Memory utility functions
void* memory_copy(void* dest, const void* src, size_t n);
void* memory_set(void* s, int c, size_t n);
int memory_compare(const void* s1, const void* s2, size_t n);

// Memory protection (to be implemented)
int memory_protect(void* address, size_t size, bool read, bool write, bool execute);
int memory_unprotect(void* address, size_t size);

// Virtual memory (to be implemented)
int memory_map_virtual(void* virtual_addr, void* physical_addr, size_t size);
int memory_unmap_virtual(void* virtual_addr, size_t size);

#endif // MEMORY_H

