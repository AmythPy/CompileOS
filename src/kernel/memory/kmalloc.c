/**
 * CompileOS Kernel Memory Allocator Implementation
 * Simple first-fit allocator with free list
 */

#include "kmalloc.h"
#include <stdint.h>
#include <stddef.h>
#include <string.h>

// Heap start and end (defined in linker script or set here)
#define HEAP_START  0x00400000  // 4MB
#define HEAP_SIZE   0x00400000  // 4MB heap

// Block header structure
typedef struct block_header {
    uint32_t size;              // Size of block (excluding header)
    uint32_t is_free;           // 1 if free, 0 if allocated
    struct block_header* next;  // Next block in list
} block_header_t;

// Heap state
static block_header_t* heap_start = NULL;
static uint32_t heap_initialized = 0;
static uint32_t total_memory = 0;
static uint32_t used_memory = 0;

/**
 * Initialize kernel heap
 */
void kmalloc_init(void) {
    if (heap_initialized) return;

    // Set up initial free block
    heap_start = (block_header_t*)HEAP_START;
    heap_start->size = HEAP_SIZE - sizeof(block_header_t);
    heap_start->is_free = 1;
    heap_start->next = NULL;

    total_memory = HEAP_SIZE;
    used_memory = 0;
    heap_initialized = 1;
}

/**
 * Find free block using first-fit algorithm
 */
static block_header_t* find_free_block(size_t size) {
    block_header_t* current = heap_start;
    
    while (current) {
        if (current->is_free && current->size >= size) {
            return current;
        }
        current = current->next;
    }
    
    return NULL;  // No suitable block found
}

/**
 * Split block if it's much larger than needed
 */
static void split_block(block_header_t* block, size_t size) {
    // Only split if remainder is large enough for a new block
    if (block->size >= size + sizeof(block_header_t) + 32) {
        block_header_t* new_block = (block_header_t*)((uint8_t*)block + sizeof(block_header_t) + size);
        new_block->size = block->size - size - sizeof(block_header_t);
        new_block->is_free = 1;
        new_block->next = block->next;
        
        block->size = size;
        block->next = new_block;
    }
}

/**
 * Allocate memory
 */
void* kmalloc(size_t size) {
    if (!heap_initialized) {
        kmalloc_init();
    }

    if (size == 0) return NULL;

    // Align size to 4 bytes
    size = (size + 3) & ~3;

    // Find free block
    block_header_t* block = find_free_block(size);
    if (!block) {
        return NULL;  // Out of memory
    }

    // Split block if possible
    split_block(block, size);

    // Mark as allocated
    block->is_free = 0;
    used_memory += block->size + sizeof(block_header_t);

    // Return pointer to data (after header)
    return (void*)((uint8_t*)block + sizeof(block_header_t));
}

/**
 * Allocate aligned memory
 */
void* kmalloc_aligned(size_t size, uint32_t align) {
    // Simple implementation: allocate extra and align
    void* ptr = kmalloc(size + align);
    if (!ptr) return NULL;

    uint32_t addr = (uint32_t)ptr;
    uint32_t aligned_addr = (addr + align - 1) & ~(align - 1);
    
    return (void*)aligned_addr;
}

/**
 * Free memory
 */
void kfree(void* ptr) {
    if (!ptr) return;

    // Get block header
    block_header_t* block = (block_header_t*)((uint8_t*)ptr - sizeof(block_header_t));
    
    // Mark as free
    block->is_free = 1;
    used_memory -= block->size + sizeof(block_header_t);

    // Coalesce with next block if it's free
    if (block->next && block->next->is_free) {
        block->size += sizeof(block_header_t) + block->next->size;
        block->next = block->next->next;
    }

    // TODO: Coalesce with previous block (requires doubly-linked list)
}

/**
 * Get heap statistics
 */
void kmalloc_stats(uint32_t* total, uint32_t* used, uint32_t* free) {
    if (total) *total = total_memory;
    if (used) *used = used_memory;
    if (free) *free = total_memory - used_memory;
}
