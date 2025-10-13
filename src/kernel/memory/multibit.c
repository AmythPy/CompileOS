/**
 * CompileOS Multi-Bit Memory Addressing - Implementation
 * 
 * Support for 16-bit, 32-bit, and 64-bit memory addressing
 * Optimized for physics engines and legacy code compatibility
 */

#include "multibit.h"
#include "memory.h"
#include <string.h>

// Global state for multi-bit memory management
static struct {
    bool initialized;
    multibit_memory_region_t regions[64];
    size_t region_count;
    multibit_memory_stats_t stats;
} g_multibit_state = {0};

/**
 * Initialize multi-bit memory system
 */
int multibit_memory_init(void) {
    if (g_multibit_state.initialized) {
        return 0;
    }
    
    g_multibit_state.region_count = 0;
    memset(&g_multibit_state.stats, 0, sizeof(g_multibit_state.stats));
    
    g_multibit_state.initialized = true;
    return 0;
}

// Alias for main.c compatibility
int multibit_init(void) {
    return multibit_memory_init();
}

/**
 * 16-bit memory access
 */
uint16_t memory_read16(void* address) {
    if (!address) return 0;
    return *(volatile uint16_t*)address;
}

void memory_write16(void* address, uint16_t value) {
    if (!address) return;
    *(volatile uint16_t*)address = value;
}

uint16_t* memory_alloc16(size_t count) {
    if (count == 0) return NULL;
    return (uint16_t*)memory_alloc(count * sizeof(uint16_t));
}

void memory_free16(uint16_t* ptr) {
    if (ptr) memory_free(ptr);
}

/**
 * 32-bit memory access
 */
uint32_t memory_read32(void* address) {
    if (!address) return 0;
    return *(volatile uint32_t*)address;
}

void memory_write32(void* address, uint32_t value) {
    if (!address) return;
    *(volatile uint32_t*)address = value;
}

uint32_t* memory_alloc32(size_t count) {
    if (count == 0) return NULL;
    return (uint32_t*)memory_alloc(count * sizeof(uint32_t));
}

void memory_free32(uint32_t* ptr) {
    if (ptr) memory_free(ptr);
}

/**
 * 64-bit memory access
 */
uint64_t memory_read64(void* address) {
    if (!address) return 0;
    return *(volatile uint64_t*)address;
}

void memory_write64(void* address, uint64_t value) {
    if (!address) return;
    *(volatile uint64_t*)address = value;
}

uint64_t* memory_alloc64(size_t count) {
    if (count == 0) return NULL;
    return (uint64_t*)memory_alloc(count * sizeof(uint64_t));
}

void memory_free64(uint64_t* ptr) {
    if (ptr) memory_free(ptr);
}

/**
 * Generic memory access
 */
void* memory_read_generic(void* address, memory_mode_t mode) {
    if (!address) return NULL;
    
    switch (mode) {
        case MEMORY_MODE_16BIT: {
            static uint16_t value;
            value = memory_read16(address);
            return &value;
        }
        case MEMORY_MODE_32BIT: {
            static uint32_t value;
            value = memory_read32(address);
            return &value;
        }
        case MEMORY_MODE_64BIT: {
            static uint64_t value;
            value = memory_read64(address);
            return &value;
        }
        default:
            return NULL;
    }
}

void memory_write_generic(void* address, void* value, memory_mode_t mode) {
    if (!address || !value) return;
    
    switch (mode) {
        case MEMORY_MODE_16BIT:
            memory_write16(address, *(uint16_t*)value);
            break;
        case MEMORY_MODE_32BIT:
            memory_write32(address, *(uint32_t*)value);
            break;
        case MEMORY_MODE_64BIT:
            memory_write64(address, *(uint64_t*)value);
            break;
    }
}

/**
 * Memory region management
 */
int multibit_memory_alloc_region(memory_mode_t mode, size_t size, multibit_memory_region_t* region) {
    if (!region || size == 0 || g_multibit_state.region_count >= 64) {
        return -1;
    }
    
    // Allocate memory
    void* ptr = memory_alloc(size);
    if (!ptr) {
        return -1;
    }
    
    // Set up region
    region->base_address = (uint64_t)ptr;
    region->size = size;
    region->bit_mode = mode;
    region->is_available = true;
    region->is_executable = false;
    region->is_writable = true;
    region->is_readable = true;
    
    // Add to region list
    g_multibit_state.regions[g_multibit_state.region_count] = *region;
    g_multibit_state.region_count++;
    
    // Update statistics
    switch (mode) {
        case MEMORY_MODE_16BIT:
            g_multibit_state.stats.total_16bit_allocations++;
            g_multibit_state.stats.total_16bit_memory += size;
            break;
        case MEMORY_MODE_32BIT:
            g_multibit_state.stats.total_32bit_allocations++;
            g_multibit_state.stats.total_32bit_memory += size;
            break;
        case MEMORY_MODE_64BIT:
            g_multibit_state.stats.total_64bit_allocations++;
            g_multibit_state.stats.total_64bit_memory += size;
            break;
    }
    
    return 0;
}

int multibit_memory_free_region(multibit_memory_region_t* region) {
    if (!region) return -1;
    
    // Find and remove region
    for (size_t i = 0; i < g_multibit_state.region_count; i++) {
        if (g_multibit_state.regions[i].base_address == region->base_address) {
            // Free memory
            memory_free((void*)region->base_address);
            
            // Remove from list
            for (size_t j = i; j < g_multibit_state.region_count - 1; j++) {
                g_multibit_state.regions[j] = g_multibit_state.regions[j + 1];
            }
            g_multibit_state.region_count--;
            
            // Update statistics
            switch (region->bit_mode) {
                case MEMORY_MODE_16BIT:
                    g_multibit_state.stats.total_16bit_allocations--;
                    g_multibit_state.stats.total_16bit_memory -= region->size;
                    break;
                case MEMORY_MODE_32BIT:
                    g_multibit_state.stats.total_32bit_allocations--;
                    g_multibit_state.stats.total_32bit_memory -= region->size;
                    break;
                case MEMORY_MODE_64BIT:
                    g_multibit_state.stats.total_64bit_allocations--;
                    g_multibit_state.stats.total_64bit_memory -= region->size;
                    break;
            }
            
            return 0;
        }
    }
    
    return -1;
}

int multibit_memory_get_regions(multibit_memory_region_t* regions, size_t max_count, size_t* actual_count) {
    if (!regions || !actual_count) return -1;
    
    size_t count = (g_multibit_state.region_count < max_count) ? g_multibit_state.region_count : max_count;
    memcpy(regions, g_multibit_state.regions, count * sizeof(multibit_memory_region_t));
    *actual_count = count;
    
    return 0;
}

/**
 * Memory mode conversion
 */
uint64_t memory_16_to_64(uint16_t value) {
    return (uint64_t)value;
}

uint32_t memory_16_to_32(uint16_t value) {
    return (uint32_t)value;
}

uint16_t memory_32_to_16(uint32_t value) {
    return (uint16_t)(value & 0xFFFF);
}

uint64_t memory_32_to_64(uint32_t value) {
    return (uint64_t)value;
}

uint32_t memory_64_to_32(uint64_t value) {
    return (uint32_t)(value & 0xFFFFFFFF);
}

uint16_t memory_64_to_16(uint64_t value) {
    return (uint16_t)(value & 0xFFFF);
}

/**
 * Memory alignment functions
 */
bool multibit_is_aligned(void* address, memory_mode_t mode) {
    if (!address) return false;
    
    uint64_t addr = (uint64_t)address;
    switch (mode) {
        case MEMORY_MODE_16BIT:
            return (addr % 2) == 0;
        case MEMORY_MODE_32BIT:
            return (addr % 4) == 0;
        case MEMORY_MODE_64BIT:
            return (addr % 8) == 0;
        default:
            return false;
    }
}

void* multibit_align(void* address, memory_mode_t mode) {
    if (!address) return NULL;
    
    uint64_t addr = (uint64_t)address;
    switch (mode) {
        case MEMORY_MODE_16BIT:
            return (void*)((addr + 1) & ~1);
        case MEMORY_MODE_32BIT:
            return (void*)((addr + 3) & ~3);
        case MEMORY_MODE_64BIT:
            return (void*)((addr + 7) & ~7);
        default:
            return address;
    }
}

size_t multibit_align_size(size_t size, memory_mode_t mode) {
    switch (mode) {
        case MEMORY_MODE_16BIT:
            return (size + 1) & ~1;
        case MEMORY_MODE_32BIT:
            return (size + 3) & ~3;
        case MEMORY_MODE_64BIT:
            return (size + 7) & ~7;
        default:
            return size;
    }
}

/**
 * Memory copying with different bit modes
 */
void memory_copy_16_to_16(void* dest, const void* src, size_t count) {
    if (!dest || !src) return;
    memcpy(dest, src, count * sizeof(uint16_t));
}

void memory_copy_32_to_32(void* dest, const void* src, size_t count) {
    if (!dest || !src) return;
    memcpy(dest, src, count * sizeof(uint32_t));
}

void memory_copy_64_to_64(void* dest, const void* src, size_t count) {
    if (!dest || !src) return;
    memcpy(dest, src, count * sizeof(uint64_t));
}

void memory_copy_16_to_32(void* dest, const void* src, size_t count) {
    if (!dest || !src) return;
    
    uint16_t* src16 = (uint16_t*)src;
    uint32_t* dest32 = (uint32_t*)dest;
    
    for (size_t i = 0; i < count; i++) {
        dest32[i] = (uint32_t)src16[i];
    }
}

void memory_copy_32_to_16(void* dest, const void* src, size_t count) {
    if (!dest || !src) return;
    
    uint32_t* src32 = (uint32_t*)src;
    uint16_t* dest16 = (uint16_t*)dest;
    
    for (size_t i = 0; i < count; i++) {
        dest16[i] = (uint16_t)(src32[i] & 0xFFFF);
    }
}

void memory_copy_16_to_64(void* dest, const void* src, size_t count) {
    if (!dest || !src) return;
    
    uint16_t* src16 = (uint16_t*)src;
    uint64_t* dest64 = (uint64_t*)dest;
    
    for (size_t i = 0; i < count; i++) {
        dest64[i] = (uint64_t)src16[i];
    }
}

void memory_copy_64_to_16(void* dest, const void* src, size_t count) {
    if (!dest || !src) return;
    
    uint64_t* src64 = (uint64_t*)src;
    uint16_t* dest16 = (uint16_t*)dest;
    
    for (size_t i = 0; i < count; i++) {
        dest16[i] = (uint16_t)(src64[i] & 0xFFFF);
    }
}

void memory_copy_32_to_64(void* dest, const void* src, size_t count) {
    if (!dest || !src) return;
    
    uint32_t* src32 = (uint32_t*)src;
    uint64_t* dest64 = (uint64_t*)dest;
    
    for (size_t i = 0; i < count; i++) {
        dest64[i] = (uint64_t)src32[i];
    }
}

void memory_copy_64_to_32(void* dest, const void* src, size_t count) {
    if (!dest || !src) return;
    
    uint64_t* src64 = (uint64_t*)src;
    uint32_t* dest32 = (uint32_t*)dest;
    
    for (size_t i = 0; i < count; i++) {
        dest32[i] = (uint32_t)(src64[i] & 0xFFFFFFFF);
    }
}

/**
 * Memory comparison with different bit modes
 */
int memory_compare_16(const void* ptr1, const void* ptr2, size_t count) {
    if (!ptr1 || !ptr2) return -1;
    return memcmp(ptr1, ptr2, count * sizeof(uint16_t));
}

int memory_compare_32(const void* ptr1, const void* ptr2, size_t count) {
    if (!ptr1 || !ptr2) return -1;
    return memcmp(ptr1, ptr2, count * sizeof(uint32_t));
}

int memory_compare_64(const void* ptr1, const void* ptr2, size_t count) {
    if (!ptr1 || !ptr2) return -1;
    return memcmp(ptr1, ptr2, count * sizeof(uint64_t));
}

/**
 * Memory search functions
 */
void* memory_search_16(const void* haystack, size_t haystack_size, uint16_t needle) {
    if (!haystack) return NULL;
    
    uint16_t* haystack16 = (uint16_t*)haystack;
    size_t count = haystack_size / sizeof(uint16_t);
    
    for (size_t i = 0; i < count; i++) {
        if (haystack16[i] == needle) {
            return &haystack16[i];
        }
    }
    
    return NULL;
}

void* memory_search_32(const void* haystack, size_t haystack_size, uint32_t needle) {
    if (!haystack) return NULL;
    
    uint32_t* haystack32 = (uint32_t*)haystack;
    size_t count = haystack_size / sizeof(uint32_t);
    
    for (size_t i = 0; i < count; i++) {
        if (haystack32[i] == needle) {
            return &haystack32[i];
        }
    }
    
    return NULL;
}

void* memory_search_64(const void* haystack, size_t haystack_size, uint64_t needle) {
    if (!haystack) return NULL;
    
    uint64_t* haystack64 = (uint64_t*)haystack;
    size_t count = haystack_size / sizeof(uint64_t);
    
    for (size_t i = 0; i < count; i++) {
        if (haystack64[i] == needle) {
            return &haystack64[i];
        }
    }
    
    return NULL;
}

/**
 * Memory statistics
 */
void multibit_memory_get_stats(multibit_memory_stats_t* stats) {
    if (!stats) return;
    *stats = g_multibit_state.stats;
}

/**
 * Physics engine specific functions
 */
physics_vector_16_t* physics_alloc_vectors_16(size_t count) {
    if (count == 0) return NULL;
    return (physics_vector_16_t*)memory_alloc(count * sizeof(physics_vector_16_t));
}

physics_vector_32_t* physics_alloc_vectors_32(size_t count) {
    if (count == 0) return NULL;
    return (physics_vector_32_t*)memory_alloc(count * sizeof(physics_vector_32_t));
}

physics_vector_64_t* physics_alloc_vectors_64(size_t count) {
    if (count == 0) return NULL;
    return (physics_vector_64_t*)memory_alloc(count * sizeof(physics_vector_64_t));
}

void physics_free_vectors_16(physics_vector_16_t* vectors) {
    if (vectors) memory_free(vectors);
}

void physics_free_vectors_32(physics_vector_32_t* vectors) {
    if (vectors) memory_free(vectors);
}

void physics_free_vectors_64(physics_vector_64_t* vectors) {
    if (vectors) memory_free(vectors);
}










