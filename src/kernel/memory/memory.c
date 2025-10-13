/**
 * CompileOS Memory Management - Implementation
 * 
 * Implements memory allocation, virtual memory, and memory protection
 */

#include "memory.h"
#include "../kernel.h"
#include "../../hal/hal.h"
#include <string.h>

// Memory management state
static struct {
    bool initialized;
    memory_region_t* regions;
    size_t region_count;
    uint64_t total_memory;
    uint64_t used_memory;
    uint64_t heap_start;
    uint64_t heap_end;
    uint64_t heap_current;
} g_memory_state = {0};

// Memory block header
typedef struct memory_block {
    size_t size;
    bool is_free;
    struct memory_block* next;
    struct memory_block* prev;
} memory_block_t;

// Heap management
static memory_block_t* g_heap_head = NULL;
static memory_block_t* g_heap_tail = NULL;

/**
 * Initialize memory management
 */
int memory_init(void) {
    if (g_memory_state.initialized) {
        return 0;
    }
    
    // Get memory map from HAL
    memory_region_t regions[32];
    size_t actual_count;
    
    if (hal_memory_map(regions, 32, &actual_count) != HAL_SUCCESS) {
        return -1;
    }
    
    // Find the largest RAM region for heap
    memory_region_t* heap_region = NULL;
    uint64_t largest_size = 0;
    
    for (size_t i = 0; i < actual_count; i++) {
        if (regions[i].type == MEMORY_TYPE_RAM && regions[i].is_available) {
            g_memory_state.total_memory += regions[i].size;
            
            if (regions[i].size > largest_size) {
                largest_size = regions[i].size;
                heap_region = &regions[i];
            }
        }
    }
    
    if (!heap_region) {
        return -1;
    }
    
    // Initialize heap
    g_memory_state.heap_start = heap_region->base_address;
    g_memory_state.heap_end = heap_region->base_address + heap_region->size;
    g_memory_state.heap_current = g_memory_state.heap_start;
    
    // Create initial heap block
    memory_block_t* initial_block = (memory_block_t*)g_memory_state.heap_start;
    initial_block->size = heap_region->size - sizeof(memory_block_t);
    initial_block->is_free = true;
    initial_block->next = NULL;
    initial_block->prev = NULL;
    
    g_heap_head = initial_block;
    g_heap_tail = initial_block;
    
    g_memory_state.initialized = true;
    return 0;
}

/**
 * Allocate memory
 */
void* memory_alloc(size_t size) {
    if (!g_memory_state.initialized || size == 0) {
        return NULL;
    }
    
    // Align size to 8-byte boundary
    size = (size + 7) & ~7;
    
    // Find a suitable free block
    memory_block_t* block = g_heap_head;
    while (block) {
        if (block->is_free && block->size >= size) {
            // Check if we need to split the block
            if (block->size > size + sizeof(memory_block_t)) {
                // Split the block
                memory_block_t* new_block = (memory_block_t*)((char*)block + sizeof(memory_block_t) + size);
                new_block->size = block->size - size - sizeof(memory_block_t);
                new_block->is_free = true;
                new_block->next = block->next;
                new_block->prev = block;
                
                if (block->next) {
                    block->next->prev = new_block;
                } else {
                    g_heap_tail = new_block;
                }
                
                block->size = size;
                block->next = new_block;
            }
            
            block->is_free = false;
            g_memory_state.used_memory += block->size;
            
            return (void*)((char*)block + sizeof(memory_block_t));
        }
        
        block = block->next;
    }
    
    return NULL; // No suitable block found
}

/**
 * Free memory
 */
void memory_free(void* ptr) {
    if (!ptr || !g_memory_state.initialized) {
        return;
    }
    
    // Get the block header
    memory_block_t* block = (memory_block_t*)((char*)ptr - sizeof(memory_block_t));
    
    if (block->is_free) {
        return; // Already free
    }
    
    block->is_free = true;
    g_memory_state.used_memory -= block->size;
    
    // Merge with adjacent free blocks
    if (block->next && block->next->is_free) {
        block->size += sizeof(memory_block_t) + block->next->size;
        block->next = block->next->next;
        if (block->next) {
            block->next->prev = block;
        } else {
            g_heap_tail = block;
        }
    }
    
    if (block->prev && block->prev->is_free) {
        block->prev->size += sizeof(memory_block_t) + block->size;
        block->prev->next = block->next;
        if (block->next) {
            block->next->prev = block->prev;
        } else {
            g_heap_tail = block->prev;
        }
    }
}

/**
 * Reallocate memory
 */
void* memory_realloc(void* ptr, size_t new_size) {
    if (!ptr) {
        return memory_alloc(new_size);
    }
    
    if (new_size == 0) {
        memory_free(ptr);
        return NULL;
    }
    
    // Get the block header
    memory_block_t* block = (memory_block_t*)((char*)ptr - sizeof(memory_block_t));
    
    // If the new size is smaller, we can just return the same pointer
    if (new_size <= block->size) {
        return ptr;
    }
    
    // Try to allocate new memory
    void* new_ptr = memory_alloc(new_size);
    if (!new_ptr) {
        return NULL;
    }
    
    // Copy the data
    memcpy(new_ptr, ptr, block->size);
    
    // Free the old memory
    memory_free(ptr);
    
    return new_ptr;
}

/**
 * Get memory statistics
 */
void memory_get_stats(memory_stats_t* stats) {
    if (!stats) {
        return;
    }
    
    stats->total_memory = g_memory_state.total_memory;
    stats->used_memory = g_memory_state.used_memory;
    stats->free_memory = g_memory_state.total_memory - g_memory_state.used_memory;
    stats->heap_start = g_memory_state.heap_start;
    stats->heap_end = g_memory_state.heap_end;
}

/**
 * Memory copy
 */
void* memory_copy(void* dest, const void* src, size_t n) {
    return memcpy(dest, src, n);
}

/**
 * Memory set
 */
void* memory_set(void* s, int c, size_t n) {
    return memset(s, c, n);
}

/**
 * Memory compare
 */
int memory_compare(const void* s1, const void* s2, size_t n) {
    return memcmp(s1, s2, n);
}

