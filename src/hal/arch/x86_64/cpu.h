/**
 * CompileOS x86_64 CPU Interface - Bare Metal
 * 
 * Direct CPU register access and feature detection
 */

#ifndef X86_64_CPU_H
#define X86_64_CPU_H

#include <stdint.h>
#include <stdbool.h>

// CPU feature flags (CPUID)
typedef struct {
    // Basic features (EDX)
    bool fpu;
    bool vme;
    bool de;
    bool pse;
    bool tsc;
    bool msr;
    bool pae;
    bool mce;
    bool cx8;
    bool apic;
    bool sep;
    bool mtrr;
    bool pge;
    bool mca;
    bool cmov;
    bool pat;
    bool pse36;
    bool psn;
    bool clfsh;
    bool ds;
    bool acpi;
    bool mmx;
    bool fxsr;
    bool sse;
    bool sse2;
    bool ss;
    bool htt;
    bool tm;
    bool ia64;
    bool pbe;
    
    // Extended features (ECX)
    bool sse3;
    bool pclmulqdq;
    bool dtes64;
    bool monitor;
    bool ds_cpl;
    bool vmx;
    bool smx;
    bool est;
    bool tm2;
    bool ssse3;
    bool cnxt_id;
    bool sdbg;
    bool fma;
    bool cx16;
    bool xtpr;
    bool pdcm;
    bool pcid;
    bool dca;
    bool sse4_1;
    bool sse4_2;
    bool x2apic;
    bool movbe;
    bool popcnt;
    bool tsc_deadline;
    bool aes;
    bool xsave;
    bool osxsave;
    bool avx;
    bool f16c;
    bool rdrand;
    bool hypervisor;
} cpu_features_t;

// CPU information
typedef struct {
    uint32_t vendor_id[4];
    uint32_t processor_brand[12];
    uint32_t max_cpuid;
    uint32_t max_ext_cpuid;
    uint8_t stepping;
    uint8_t model;
    uint8_t family;
    uint8_t type;
    uint8_t ext_model;
    uint8_t ext_family;
    uint8_t brand_index;
    uint8_t clflush_size;
    uint8_t max_cores;
    uint8_t max_threads;
    uint8_t apic_id;
    cpu_features_t features;
} cpu_info_t;

// CPU detection and information
void cpu_detect(cpu_info_t* info);
const char* cpu_get_vendor_string(const cpu_info_t* info);
const char* cpu_get_brand_string(const cpu_info_t* info);
bool cpu_has_feature(const cpu_info_t* info, const char* feature_name);

// CPU control registers
uint64_t cpu_read_cr0(void);
void cpu_write_cr0(uint64_t value);
uint64_t cpu_read_cr2(void);
uint64_t cpu_read_cr3(void);
void cpu_write_cr3(uint64_t value);
uint64_t cpu_read_cr4(void);
void cpu_write_cr4(uint64_t value);

// CPU flags
uint64_t cpu_read_rflags(void);
void cpu_write_rflags(uint64_t value);
bool cpu_interrupts_enabled(void);
void cpu_enable_interrupts(void);
void cpu_disable_interrupts(void);

// CPU identification
uint32_t cpu_get_apic_id(void);
uint32_t cpu_get_core_count(void);
uint32_t cpu_get_thread_count(void);

// CPU power management
void cpu_halt(void);
void cpu_pause(void);
void cpu_nop(void);

// CPU timing
uint64_t cpu_read_tsc(void);
uint64_t cpu_read_tsc_aux(void);
void cpu_serialize(void);

// CPU cache control
void cpu_invalidate_tlb(void);
void cpu_invalidate_tlb_page(uint64_t address);
void cpu_wbinvd(void);
void cpu_clflush(void* address);
void cpu_clflushopt(void* address);
void cpu_clwb(void* address);

// CPU memory barriers
void cpu_mfence(void);
void cpu_sfence(void);
void cpu_lfence(void);

// CPU debugging
void cpu_breakpoint(void);
void cpu_debug_exception(void);

#endif // X86_64_CPU_H










