/**
 * CompileOS Kernel Memory Allocator
 * Simple heap allocator for kernel memory
 */

#ifndef KMALLOC_H
#define KMALLOC_H

#include <stdint.h>
#include <stddef.h>

// Initialize kernel heap
void kmalloc_init(void);

// Allocate memory
void* kmalloc(size_t size);

// Allocate aligned memory
void* kmalloc_aligned(size_t size, uint32_t align);

// Free memory
void kfree(void* ptr);

// Get heap statistics
void kmalloc_stats(uint32_t* total, uint32_t* used, uint32_t* free);

#endif // KMALLOC_H
