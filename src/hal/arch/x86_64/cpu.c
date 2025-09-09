/**
 * CompileOS x86_64 CPU Implementation - Bare Metal
 * 
 * Direct CPU register access and feature detection
 */

#include "cpu.h"

// CPUID instruction wrapper
static void cpuid(uint32_t eax, uint32_t ecx, uint32_t* eax_out, uint32_t* ebx_out, uint32_t* ecx_out, uint32_t* edx_out) {
    __asm__ volatile (
        "cpuid"
        : "=a" (*eax_out), "=b" (*ebx_out), "=c" (*ecx_out), "=d" (*edx_out)
        : "a" (eax), "c" (ecx)
    );
}

// Detect CPU features and information
void cpu_detect(cpu_info_t* info) {
    if (!info) return;
    
    uint32_t eax, ebx, ecx, edx;
    
    // Get basic CPUID information
    cpuid(0, 0, &eax, &ebx, &ecx, &edx);
    info->max_cpuid = eax;
    info->vendor_id[0] = ebx;
    info->vendor_id[1] = edx;
    info->vendor_id[2] = ecx;
    info->vendor_id[3] = 0;
    
    // Get processor brand string
    if (eax >= 0x80000004) {
        cpuid(0x80000002, 0, &eax, &ebx, &ecx, &edx);
        info->processor_brand[0] = eax;
        info->processor_brand[1] = ebx;
        info->processor_brand[2] = ecx;
        info->processor_brand[3] = edx;
        
        cpuid(0x80000003, 0, &eax, &ebx, &ecx, &edx);
        info->processor_brand[4] = eax;
        info->processor_brand[5] = ebx;
        info->processor_brand[6] = ecx;
        info->processor_brand[7] = edx;
        
        cpuid(0x80000004, 0, &eax, &ebx, &ecx, &edx);
        info->processor_brand[8] = eax;
        info->processor_brand[9] = ebx;
        info->processor_brand[10] = ecx;
        info->processor_brand[11] = edx;
    }
    
    // Get extended CPUID
    cpuid(0x80000000, 0, &eax, &ebx, &ecx, &edx);
    info->max_ext_cpuid = eax;
    
    // Get processor info
    if (eax >= 0x80000001) {
        cpuid(0x80000001, 0, &eax, &ebx, &ecx, &edx);
        // Extended features in ECX, EDX
    }
    
    // Get basic features (CPUID 1)
    if (info->max_cpuid >= 1) {
        cpuid(1, 0, &eax, &ebx, &ecx, &edx);
        
        // Extract processor info
        info->stepping = eax & 0xF;
        info->model = (eax >> 4) & 0xF;
        info->family = (eax >> 8) & 0xF;
        info->type = (eax >> 12) & 0x3;
        info->ext_model = (eax >> 16) & 0xF;
        info->ext_family = (eax >> 20) & 0xFF;
        info->brand_index = ebx & 0xFF;
        info->clflush_size = (ebx >> 8) & 0xFF;
        info->max_cores = (ebx >> 16) & 0xFF;
        info->max_threads = (ebx >> 24) & 0xFF;
        info->apic_id = (ebx >> 24) & 0xFF;
        
        // Basic features (EDX)
        info->features.fpu = (edx >> 0) & 1;
        info->features.vme = (edx >> 1) & 1;
        info->features.de = (edx >> 2) & 1;
        info->features.pse = (edx >> 3) & 1;
        info->features.tsc = (edx >> 4) & 1;
        info->features.msr = (edx >> 5) & 1;
        info->features.pae = (edx >> 6) & 1;
        info->features.mce = (edx >> 7) & 1;
        info->features.cx8 = (edx >> 8) & 1;
        info->features.apic = (edx >> 9) & 1;
        info->features.sep = (edx >> 11) & 1;
        info->features.mtrr = (edx >> 12) & 1;
        info->features.pge = (edx >> 13) & 1;
        info->features.mca = (edx >> 14) & 1;
        info->features.cmov = (edx >> 15) & 1;
        info->features.pat = (edx >> 16) & 1;
        info->features.pse36 = (edx >> 17) & 1;
        info->features.psn = (edx >> 18) & 1;
        info->features.clfsh = (edx >> 19) & 1;
        info->features.ds = (edx >> 21) & 1;
        info->features.acpi = (edx >> 22) & 1;
        info->features.mmx = (edx >> 23) & 1;
        info->features.fxsr = (edx >> 24) & 1;
        info->features.sse = (edx >> 25) & 1;
        info->features.sse2 = (edx >> 26) & 1;
        info->features.ss = (edx >> 27) & 1;
        info->features.htt = (edx >> 28) & 1;
        info->features.tm = (edx >> 29) & 1;
        info->features.ia64 = (edx >> 30) & 1;
        info->features.pbe = (edx >> 31) & 1;
        
        // Extended features (ECX)
        info->features.sse3 = (ecx >> 0) & 1;
        info->features.pclmulqdq = (ecx >> 1) & 1;
        info->features.dtes64 = (ecx >> 2) & 1;
        info->features.monitor = (ecx >> 3) & 1;
        info->features.ds_cpl = (ecx >> 4) & 1;
        info->features.vmx = (ecx >> 5) & 1;
        info->features.smx = (ecx >> 6) & 1;
        info->features.est = (ecx >> 7) & 1;
        info->features.tm2 = (ecx >> 8) & 1;
        info->features.ssse3 = (ecx >> 9) & 1;
        info->features.cnxt_id = (ecx >> 10) & 1;
        info->features.sdbg = (ecx >> 11) & 1;
        info->features.fma = (ecx >> 12) & 1;
        info->features.cx16 = (ecx >> 13) & 1;
        info->features.xtpr = (ecx >> 14) & 1;
        info->features.pdcm = (ecx >> 15) & 1;
        info->features.pcid = (ecx >> 17) & 1;
        info->features.dca = (ecx >> 18) & 1;
        info->features.sse4_1 = (ecx >> 19) & 1;
        info->features.sse4_2 = (ecx >> 20) & 1;
        info->features.x2apic = (ecx >> 21) & 1;
        info->features.movbe = (ecx >> 22) & 1;
        info->features.popcnt = (ecx >> 23) & 1;
        info->features.tsc_deadline = (ecx >> 24) & 1;
        info->features.aes = (ecx >> 25) & 1;
        info->features.xsave = (ecx >> 26) & 1;
        info->features.osxsave = (ecx >> 27) & 1;
        info->features.avx = (ecx >> 28) & 1;
        info->features.f16c = (ecx >> 29) & 1;
        info->features.rdrand = (ecx >> 30) & 1;
        info->features.hypervisor = (ecx >> 31) & 1;
    }
}

// Get vendor string
const char* cpu_get_vendor_string(const cpu_info_t* info) {
    if (!info) return "Unknown";
    return (const char*)info->vendor_id;
}

// Get brand string
const char* cpu_get_brand_string(const cpu_info_t* info) {
    if (!info) return "Unknown";
    return (const char*)info->processor_brand;
}

// Check if CPU has specific feature
bool cpu_has_feature(const cpu_info_t* info, const char* feature_name) {
    if (!info || !feature_name) return false;
    
    // This would need a more sophisticated lookup table
    // For now, just return false
    (void)feature_name;
    return false;
}

// Control register access
uint64_t cpu_read_cr0(void) {
    uint64_t value;
    __asm__ volatile ("mov %%cr0, %0" : "=r" (value));
    return value;
}

void cpu_write_cr0(uint64_t value) {
    __asm__ volatile ("mov %0, %%cr0" : : "r" (value));
}

uint64_t cpu_read_cr2(void) {
    uint64_t value;
    __asm__ volatile ("mov %%cr2, %0" : "=r" (value));
    return value;
}

uint64_t cpu_read_cr3(void) {
    uint64_t value;
    __asm__ volatile ("mov %%cr3, %0" : "=r" (value));
    return value;
}

void cpu_write_cr3(uint64_t value) {
    __asm__ volatile ("mov %0, %%cr3" : : "r" (value));
}

uint64_t cpu_read_cr4(void) {
    uint64_t value;
    __asm__ volatile ("mov %%cr4, %0" : "=r" (value));
    return value;
}

void cpu_write_cr4(uint64_t value) {
    __asm__ volatile ("mov %0, %%cr4" : : "r" (value));
}

// RFLAGS access
uint64_t cpu_read_rflags(void) {
    uint64_t value;
    __asm__ volatile ("pushfq; pop %0" : "=r" (value));
    return value;
}

void cpu_write_rflags(uint64_t value) {
    __asm__ volatile ("push %0; popfq" : : "r" (value));
}

// Interrupt control
bool cpu_interrupts_enabled(void) {
    return (cpu_read_rflags() & 0x200) != 0;
}

void cpu_enable_interrupts(void) {
    __asm__ volatile ("sti");
}

void cpu_disable_interrupts(void) {
    __asm__ volatile ("cli");
}

// CPU identification
uint32_t cpu_get_apic_id(void) {
    uint32_t eax, ebx, ecx, edx;
    cpuid(1, 0, &eax, &ebx, &ecx, &edx);
    return (ebx >> 24) & 0xFF;
}

uint32_t cpu_get_core_count(void) {
    // This is a simplified implementation
    // Real implementation would need to parse CPUID properly
    return 1;
}

uint32_t cpu_get_thread_count(void) {
    // This is a simplified implementation
    // Real implementation would need to parse CPUID properly
    return 1;
}

// Power management
void cpu_halt(void) {
    __asm__ volatile ("hlt");
}

void cpu_pause(void) {
    __asm__ volatile ("pause");
}

void cpu_nop(void) {
    __asm__ volatile ("nop");
}

// Timing
uint64_t cpu_read_tsc(void) {
    uint32_t low, high;
    __asm__ volatile ("rdtsc" : "=a" (low), "=d" (high));
    return ((uint64_t)high << 32) | low;
}

uint64_t cpu_read_tsc_aux(void) {
    uint32_t value;
    __asm__ volatile ("rdtscp" : "=a" (value) : : "rcx");
    return value;
}

void cpu_serialize(void) {
    __asm__ volatile ("serialize");
}

// Cache control
void cpu_invalidate_tlb(void) {
    __asm__ volatile ("mov %%cr3, %%rax; mov %%rax, %%cr3" : : : "rax");
}

void cpu_invalidate_tlb_page(uint64_t address) {
    __asm__ volatile ("invlpg (%0)" : : "r" (address) : "memory");
}

void cpu_wbinvd(void) {
    __asm__ volatile ("wbinvd");
}

void cpu_clflush(void* address) {
    __asm__ volatile ("clflush (%0)" : : "r" (address) : "memory");
}

void cpu_clflushopt(void* address) {
    __asm__ volatile ("clflushopt (%0)" : : "r" (address) : "memory");
}

void cpu_clwb(void* address) {
    __asm__ volatile ("clwb (%0)" : : "r" (address) : "memory");
}

// Memory barriers
void cpu_mfence(void) {
    __asm__ volatile ("mfence");
}

void cpu_sfence(void) {
    __asm__ volatile ("sfence");
}

void cpu_lfence(void) {
    __asm__ volatile ("lfence");
}

// Debugging
void cpu_breakpoint(void) {
    __asm__ volatile ("int3");
}

void cpu_debug_exception(void) {
    __asm__ volatile ("int $1");
}
