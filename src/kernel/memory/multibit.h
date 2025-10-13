/**
 * CompileOS Multi-Bit Memory Addressing - Header
 * 
 * Support for 16-bit, 32-bit, and 64-bit memory addressing
 * Optimized for physics engines and legacy code compatibility
 */

#ifndef MULTIBIT_H
#define MULTIBIT_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// Memory mode type
typedef enum {
    MEMORY_MODE_16BIT = 16,
    MEMORY_MODE_32BIT = 32,
    MEMORY_MODE_64BIT = 64
} memory_mode_t;

// Memory access types
//typedef enum {
//    MEMORY_ACCESS_READ = 0,
  //  MEMORY_ACCESS_WRITE,
    // MEMORY_ACCESS_EXECUTE
//} memory_access_t;

// Memory region descriptor for different bit modes
typedef struct {
    uint64_t base_address;
    uint64_t size;
    memory_mode_t bit_mode;
    bool is_available;
    bool is_executable;
    bool is_writable;
    bool is_readable;
} multibit_memory_region_t;

// Memory access functions for different bit modes
// 16-bit memory access
uint16_t memory_read16(void* address);
void memory_write16(void* address, uint16_t value);
uint16_t* memory_alloc16(size_t count);
void memory_free16(uint16_t* ptr);

// 32-bit memory access
uint32_t memory_read32(void* address);
void memory_write32(void* address, uint32_t value);
uint32_t* memory_alloc32(size_t count);
void memory_free32(uint32_t* ptr);

// 64-bit memory access
uint64_t memory_read64(void* address);
void memory_write64(void* address, uint64_t value);
uint64_t* memory_alloc64(size_t count);
void memory_free64(uint64_t* ptr);

// Generic memory access (auto-detects bit mode)
void* memory_read_generic(void* address, memory_mode_t mode);
void memory_write_generic(void* address, void* value, memory_mode_t mode);

// Memory region management
int multibit_memory_init(void);
int multibit_init(void);  // Alias for compatibility
int multibit_memory_alloc_region(memory_mode_t mode, size_t size, multibit_memory_region_t* region);
int multibit_memory_free_region(multibit_memory_region_t* region);
int multibit_memory_get_regions(multibit_memory_region_t* regions, size_t max_count, size_t* actual_count);

// Memory mode conversion
uint64_t memory_16_to_64(uint16_t value);
uint32_t memory_16_to_32(uint16_t value);
uint16_t memory_32_to_16(uint32_t value);
uint64_t memory_32_to_64(uint32_t value);
uint32_t memory_64_to_32(uint64_t value);
uint16_t memory_64_to_16(uint64_t value);

// Memory alignment functions
bool multibit_is_aligned(void* address, memory_mode_t mode);
void* multibit_align(void* address, memory_mode_t mode);
size_t multibit_align_size(size_t size, memory_mode_t mode);

// Memory copying with different bit modes
void memory_copy_16_to_16(void* dest, const void* src, size_t count);
void memory_copy_32_to_32(void* dest, const void* src, size_t count);
void memory_copy_64_to_64(void* dest, const void* src, size_t count);
void memory_copy_16_to_32(void* dest, const void* src, size_t count);
void memory_copy_32_to_16(void* dest, const void* src, size_t count);
void memory_copy_16_to_64(void* dest, const void* src, size_t count);
void memory_copy_64_to_16(void* dest, const void* src, size_t count);
void memory_copy_32_to_64(void* dest, const void* src, size_t count);
void memory_copy_64_to_32(void* dest, const void* src, size_t count);

// Memory comparison with different bit modes
int memory_compare_16(const void* ptr1, const void* ptr2, size_t count);
int memory_compare_32(const void* ptr1, const void* ptr2, size_t count);
int memory_compare_64(const void* ptr1, const void* ptr2, size_t count);

// Memory search functions
void* memory_search_16(const void* haystack, size_t haystack_size, uint16_t needle);
void* memory_search_32(const void* haystack, size_t haystack_size, uint32_t needle);
void* memory_search_64(const void* haystack, size_t haystack_size, uint64_t needle);

// Memory statistics for different bit modes
typedef struct {
    size_t total_16bit_allocations;
    size_t total_32bit_allocations;
    size_t total_64bit_allocations;
    size_t total_16bit_memory;
    size_t total_32bit_memory;
    size_t total_64bit_memory;
    size_t free_16bit_memory;
    size_t free_32bit_memory;
    size_t free_64bit_memory;
} multibit_memory_stats_t;

void multibit_memory_get_stats(multibit_memory_stats_t* stats);

// Physics engine specific functions (16-bit optimized)
typedef struct {
    uint16_t x, y, z;  // Position (16-bit for physics precision)
    uint16_t vx, vy, vz; // Velocity
    uint16_t ax, ay, az; // Acceleration
} physics_vector_16_t;

typedef struct {
    uint32_t x, y, z;  // Position (32-bit for higher precision)
    uint32_t vx, vy, vz; // Velocity
    uint32_t ax, ay, az; // Acceleration
} physics_vector_32_t;

typedef struct {
    uint64_t x, y, z;  // Position (64-bit for maximum precision)
    uint64_t vx, vy, vz; // Velocity
    uint64_t ax, ay, az; // Acceleration
} physics_vector_64_t;

// Physics memory allocation
physics_vector_16_t* physics_alloc_vectors_16(size_t count);
physics_vector_32_t* physics_alloc_vectors_32(size_t count);
physics_vector_64_t* physics_alloc_vectors_64(size_t count);

void physics_free_vectors_16(physics_vector_16_t* vectors);
void physics_free_vectors_32(physics_vector_32_t* vectors);
void physics_free_vectors_64(physics_vector_64_t* vectors);

#endif // MULTIBIT_H










