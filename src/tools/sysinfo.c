/**
 * CompileOS System Information Tool
 * 
 * Display system information and statistics
 */

#include "../runtime/runtime.h"
#include "../kernel/memory/memory.h"
#include "../kernel/process/process.h"
#include "../hal/hal.h"
#include <string.h>

static void print_header(void) {
    println("╔════════════════════════════════════════════════════════════════════════════╗");
    println("║                    CompileOS System Information v1.0                       ║");
    println("╚════════════════════════════════════════════════════════════════════════════╝");
}

static void print_os_info(void) {
    println("\n┌─ Operating System ─────────────────────────────────────────────────────────┐");
    println("│ Name:           CompileOS");
    println("│ Version:        0.1.0 (V0.01a)");
    println("│ Architecture:   x86_64");
    println("│ Build Date:     " __DATE__ " " __TIME__);
    println("│ Description:    Hardware Agnostic Development Platform");
    println("└────────────────────────────────────────────────────────────────────────────┘");
}

static void print_cpu_info(void) {
    println("\n┌─ CPU Information ──────────────────────────────────────────────────────────┐");
    
    const char* arch = hal_get_cpu_architecture_string();
    printf_rt("│ Architecture:   %s\n", arch);
    println("│ Mode:           64-bit");
    println("│ Features:       SSE, SSE2, x87 FPU");
    println("│ Cores:          1 (detected)");
    
    println("└────────────────────────────────────────────────────────────────────────────┘");
}

static void print_memory_info(void) {
    println("\n┌─ Memory Information ───────────────────────────────────────────────────────┐");
    
    memory_stats_t stats;
    memory_get_stats(&stats);
    
    printf_rt("│ Total Memory:   %u bytes (%.2f MB)\n", 
             (unsigned int)stats.total_memory,
             stats.total_memory / (1024.0 * 1024.0));
    printf_rt("│ Used Memory:    %u bytes (%.2f MB)\n",
             (unsigned int)stats.used_memory,
             stats.used_memory / (1024.0 * 1024.0));
    printf_rt("│ Free Memory:    %u bytes (%.2f MB)\n",
             (unsigned int)stats.free_memory,
             stats.free_memory / (1024.0 * 1024.0));
    printf_rt("│ Heap Range:     %p - %p\n",
             (void*)stats.heap_start,
             (void*)stats.heap_end);
    printf_rt("│ Allocations:    %u\n", (unsigned int)stats.allocation_count);
    printf_rt("│ Frees:          %u\n", (unsigned int)stats.free_count);
    
    println("└────────────────────────────────────────────────────────────────────────────┘");
}

static void print_process_info(void) {
    println("\n┌─ Process Information ──────────────────────────────────────────────────────┐");
    
    process_stats_t stats;
    process_get_stats(&stats);
    
    printf_rt("│ Total Processes:    %u\n", (unsigned int)stats.total_processes);
    printf_rt("│ Running Processes:  %u\n", (unsigned int)stats.running_processes);
    printf_rt("│ Blocked Processes:  %u\n", (unsigned int)stats.blocked_processes);
    printf_rt("│ Current PID:        %d\n", rt_getpid());
    
    println("└────────────────────────────────────────────────────────────────────────────┘");
}

static void print_features(void) {
    println("\n┌─ System Features ──────────────────────────────────────────────────────────┐");
    println("│ ✓ PS/2 Keyboard Driver");
    println("│ ✓ VGA Text Mode (80x25)");
    println("│ ✓ Interactive Terminal");
    println("│ ✓ REPL Expression Evaluator");
    println("│ ✓ In-Memory Filesystem");
    println("│ ✓ Desktop Environment");
    println("│ ✓ Memory Management (Heap Allocator)");
    println("│ ✓ Process Scheduler (Round-Robin)");
    println("│ ✓ Interrupt Handling (IDT, PIC)");
    println("│ ✓ Multi-bit Memory Support (16/32/64-bit)");
    println("└────────────────────────────────────────────────────────────────────────────┘");
}

static void print_components(void) {
    println("\n┌─ Installed Components ─────────────────────────────────────────────────────┐");
    println("│ • Kernel Core");
    println("│ • Hardware Abstraction Layer (HAL)");
    println("│ • Memory Manager");
    println("│ • Process Manager");
    println("│ • Filesystem");
    println("│ • Terminal System");
    println("│ • REPL System");
    println("│ • Desktop Environment");
    println("│ • Debugger (stub)");
    println("│ • VGA Driver");
    println("│ • Keyboard Driver");
    println("│ • Runtime Library");
    println("│ • Userland Applications");
    println("│ • Development Tools");
    println("└────────────────────────────────────────────────────────────────────────────┘");
}

int sysinfo_main(void) {
    print_header();
    print_os_info();
    print_cpu_info();
    print_memory_info();
    print_process_info();
    print_features();
    print_components();
    
    println("");
    return 0;
}
