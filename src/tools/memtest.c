/**
 * CompileOS Memory Test Tool
 * 
 * Test memory allocation and performance
 */

#include "../runtime/runtime.h"
#include "../kernel/memory/memory.h"
#include <string.h>

#define TEST_ITERATIONS 100
#define TEST_SIZES 10

static const size_t test_sizes[TEST_SIZES] = {
    16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192
};

static void test_allocation(void) {
    println("\n=== Memory Allocation Test ===");
    
    for (int i = 0; i < TEST_SIZES; i++) {
        size_t size = test_sizes[i];
        void* ptr = rt_malloc(size);
        
        if (ptr) {
            printf_rt("✓ Allocated %u bytes at %p\n", (unsigned int)size, ptr);
            
            // Test write
            memset(ptr, 0xAA, size);
            
            // Test read
            uint8_t* bytes = (uint8_t*)ptr;
            bool ok = true;
            for (size_t j = 0; j < size; j++) {
                if (bytes[j] != 0xAA) {
                    ok = false;
                    break;
                }
            }
            
            if (ok) {
                println("  ✓ Read/write test passed");
            } else {
                println("  ✗ Read/write test FAILED");
            }
            
            rt_free(ptr);
            println("  ✓ Freed");
        } else {
            printf_rt("✗ Failed to allocate %u bytes\n", (unsigned int)size);
        }
    }
}

static void test_fragmentation(void) {
    println("\n=== Memory Fragmentation Test ===");
    
    void* ptrs[20];
    
    // Allocate
    for (int i = 0; i < 20; i++) {
        ptrs[i] = rt_malloc(128);
        if (ptrs[i]) {
            printf_rt("Allocated block %d at %p\n", i, ptrs[i]);
        }
    }
    
    // Free every other block
    for (int i = 0; i < 20; i += 2) {
        if (ptrs[i]) {
            rt_free(ptrs[i]);
            printf_rt("Freed block %d\n", i);
        }
    }
    
    // Try to allocate larger block
    void* large = rt_malloc(1024);
    if (large) {
        println("✓ Large allocation succeeded despite fragmentation");
        rt_free(large);
    } else {
        println("✗ Large allocation failed (fragmentation issue)");
    }
    
    // Free remaining blocks
    for (int i = 1; i < 20; i += 2) {
        if (ptrs[i]) {
            rt_free(ptrs[i]);
        }
    }
}

static void test_statistics(void) {
    println("\n=== Memory Statistics ===");
    
    memory_stats_t stats;
    memory_get_stats(&stats);
    
    printf_rt("Total Memory:  %u bytes\n", (unsigned int)stats.total_memory);
    printf_rt("Used Memory:   %u bytes\n", (unsigned int)stats.used_memory);
    printf_rt("Free Memory:   %u bytes\n", (unsigned int)stats.free_memory);
    printf_rt("Heap Start:    %p\n", (void*)stats.heap_start);
    printf_rt("Heap End:      %p\n", (void*)stats.heap_end);
    printf_rt("Allocations:   %u\n", (unsigned int)stats.allocation_count);
    printf_rt("Frees:         %u\n", (unsigned int)stats.free_count);
}

int memtest_main(void) {
    println("╔════════════════════════════════════════╗");
    println("║  CompileOS Memory Test Tool v1.0       ║");
    println("╚════════════════════════════════════════╝");
    
    test_allocation();
    test_fragmentation();
    test_statistics();
    
    println("\n=== Memory Test Complete ===\n");
    return 0;
}
