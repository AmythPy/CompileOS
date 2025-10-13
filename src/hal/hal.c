/**
 * CompileOS Hardware Abstraction Layer (HAL) - Implementation
 * 
 * Provides hardware-agnostic implementations for different architectures
 */

#include "hal.h"
#include "arch/x86_64/cpu.h"
#include "arch/x86_64/io.h"
#include "arch/x86_64/interrupts.h"
#include <stdarg.h>

// Global HAL state
static struct {
    bool initialized;
    cpu_arch_t cpu_arch;
    uint32_t cpu_count;
    uint64_t timer_frequency;
    volatile uint64_t timer_ticks;
    timer_callback_t timer_callback;
    void* timer_callback_ctx;
    interrupt_handler_t interrupt_handlers[256];
    void* interrupt_contexts[256];
} g_hal_state = {0};

// Serial COM1 (0x3F8) simple init/output for logging
static void serial_init_com1(void) {
    // Disable interrupts
    io_outb(0x3F8 + 1, 0x00);
    // Enable DLAB
    io_outb(0x3F8 + 3, 0x80);
    // Set baud rate divisor to 3 (38400 baud)
    io_outb(0x3F8 + 0, 0x03);
    io_outb(0x3F8 + 1, 0x00);
    // 8 bits, no parity, one stop bit
    io_outb(0x3F8 + 3, 0x03);
    // Enable FIFO, clear, 14-byte threshold
    io_outb(0x3F8 + 2, 0xC7);
    // IRQs enabled, RTS/DSR set
    io_outb(0x3F8 + 4, 0x0B);
}

static int serial_is_transmit_empty(void) {
    return io_inb(0x3F8 + 5) & 0x20;
}

static void serial_write_char(char c) {
    while (!serial_is_transmit_empty()) {}
    io_outb(0x3F8, (uint8_t)c);
}

/**
 * Detect CPU architecture
 */
static cpu_arch_t hal_detect_cpu_architecture(void) {
    cpu_info_t cpu_info;
    cpu_detect(&cpu_info);
    
    // Check for x86_64 features
    if (cpu_info.features.sse2 && cpu_info.features.tm) {
        return ARCH_X86_64;
    }
    
    return ARCH_UNKNOWN;
}

/**
 * Initialize HAL
 */
hal_status_t hal_init(void) {
    if (g_hal_state.initialized) {
        return HAL_SUCCESS;
    }
    
    // Detect CPU architecture
    g_hal_state.cpu_arch = hal_detect_cpu_architecture();
    g_hal_state.cpu_count = 1; // TODO: Implement proper CPU count detection
    
    // Initialize architecture-specific components
    switch (g_hal_state.cpu_arch) {
        case ARCH_X86_64:
            // Initialize x86_64 specific components
            interrupts_init();
            // Initialize system timer at 100 Hz by default
            hal_timer_init(100);
            break;
        case ARCH_ARM64:
            // TODO: Initialize ARM64 specific components
            break;
        default:
            return HAL_ERROR_NOT_IMPLEMENTED;
    }
    
    // Initialize debug/logging
    hal_log_init();
    
    g_hal_state.initialized = true;
    return HAL_SUCCESS;
}

/**
 * Shutdown HAL
 */
void hal_shutdown(void) {
    if (!g_hal_state.initialized) {
        return;
    }
    
    // TODO: Implement proper shutdown sequence
    g_hal_state.initialized = false;
}

/**
 * Get CPU architecture
 */
cpu_arch_t hal_get_cpu_architecture(void) {
    return g_hal_state.cpu_arch;
}

/**
 * Get CPU architecture string
 */
const char* hal_get_cpu_architecture_string(void) {
    switch (g_hal_state.cpu_arch) {
        case ARCH_X86_32: return "x86_32";
        case ARCH_X86_64: return "x86_64";
        case ARCH_ARM32: return "ARM32";
        case ARCH_ARM64: return "ARM64";
        case ARCH_RISCV32: return "RISC-V 32";
        case ARCH_RISCV64: return "RISC-V 64";
        default: return "Unknown";
    }
}

/**
 * Get CPU count
 */
uint32_t hal_get_cpu_count(void) {
    return g_hal_state.cpu_count;
}

/**
 * Memory map (placeholder implementation)
 */
hal_status_t hal_memory_map(memory_region_t* regions, size_t max_regions, size_t* actual_count) {
    if (!regions || !actual_count) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    // TODO: Implement proper memory mapping
    // For now, return a dummy region
    if (max_regions > 0) {
        regions[0].base_address = 0x100000; // 1MB
        regions[0].size = 0x10000000; // 256MB
        regions[0].type = MEMORY_TYPE_RAM;
        regions[0].is_available = true;
        *actual_count = 1;
    } else {
        *actual_count = 0;
    }
    
    return HAL_SUCCESS;
}

/**
 * Memory protection (placeholder implementation)
 */
hal_status_t hal_memory_protect(void* address, size_t size, bool read, bool write, bool execute) {
    // TODO: Implement proper memory protection
    (void)address;
    (void)size;
    (void)read;
    (void)write;
    (void)execute;
    return HAL_SUCCESS;
}

/**
 * Memory cache flush (placeholder implementation)
 */
hal_status_t hal_memory_flush_cache(void* address, size_t size) {
    // TODO: Implement proper cache flushing
    (void)address;
    (void)size;
    return HAL_SUCCESS;
}

/**
 * Register interrupt handler
 */
hal_status_t hal_interrupt_register(uint32_t interrupt_number, interrupt_handler_t handler, void* context) {
    if (interrupt_number >= 256 || !handler) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    g_hal_state.interrupt_handlers[interrupt_number] = handler;
    g_hal_state.interrupt_contexts[interrupt_number] = context;
    
    return HAL_SUCCESS;
}

/**
 * Enable interrupt
 */
hal_status_t hal_interrupt_enable(uint32_t interrupt_number) {
    if (interrupt_number >= 256) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    // TODO: Implement proper interrupt enabling
    return HAL_SUCCESS;
}

/**
 * Disable interrupt
 */
hal_status_t hal_interrupt_disable(uint32_t interrupt_number) {
    if (interrupt_number >= 256) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    // TODO: Implement proper interrupt disabling
    return HAL_SUCCESS;
}

/**
 * Acknowledge interrupt
 */
hal_status_t hal_interrupt_acknowledge(uint32_t interrupt_number) {
    if (interrupt_number >= 256) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    // TODO: Implement proper interrupt acknowledgment
    return HAL_SUCCESS;
}

/**
 * Initialize timer
 */
hal_status_t hal_timer_init(uint32_t frequency_hz) {
    if (frequency_hz == 0) {
        return HAL_ERROR_INVALID_PARAM;
    }
    g_hal_state.timer_frequency = frequency_hz;
    g_hal_state.timer_ticks = 0;
    g_hal_state.timer_callback = 0;
    g_hal_state.timer_callback_ctx = 0;

    // Program PIT (channel 0) to given frequency
    // PIT base frequency is 1193182 Hz
    uint32_t divisor = 1193182u / frequency_hz;
    if (divisor == 0) divisor = 1;
    // Command port 0x43: channel 0, lobyte/hibyte, mode 3 (square wave)
    io_outb(0x43, 0x36);
    io_outb(0x40, (uint8_t)(divisor & 0xFF));
    io_outb(0x40, (uint8_t)((divisor >> 8) & 0xFF));

    // Enable IRQ0 (timer)
    pic_enable_irq(0);
    return HAL_SUCCESS;
}

/**
 * Register timer callback
 */
hal_status_t hal_timer_register_callback(timer_callback_t callback, void* context) {
    g_hal_state.timer_callback = callback;
    g_hal_state.timer_callback_ctx = context;
    return HAL_SUCCESS;
}

/**
 * Get timer ticks
 */
uint64_t hal_timer_get_ticks(void) {
    return g_hal_state.timer_ticks;
}

/**
 * Convert ticks to nanoseconds
 */
uint64_t hal_timer_ticks_to_ns(uint64_t ticks) {
    if (g_hal_state.timer_frequency == 0) {
        return 0;
    }
    return (ticks * 1000000000ULL) / g_hal_state.timer_frequency;
}

void hal_timer_on_interrupt(void) {
    g_hal_state.timer_ticks++;
    if (g_hal_state.timer_callback) {
        g_hal_state.timer_callback(g_hal_state.timer_callback_ctx);
    }
}

/**
 * I/O operations (bare metal implementations)
 */
hal_status_t hal_io_read8(uint16_t port, uint8_t* value) {
    if (!value) {
        return HAL_ERROR_INVALID_PARAM;
    }
    *value = io_inb(port);
    return HAL_SUCCESS;
}

hal_status_t hal_io_write8(uint16_t port, uint8_t value) {
    io_outb(port, value);
    return HAL_SUCCESS;
}

hal_status_t hal_io_read16(uint16_t port, uint16_t* value) {
    if (!value) {
        return HAL_ERROR_INVALID_PARAM;
    }
    *value = io_inw(port);
    return HAL_SUCCESS;
}

hal_status_t hal_io_write16(uint16_t port, uint16_t value) {
    io_outw(port, value);
    return HAL_SUCCESS;
}

hal_status_t hal_io_read32(uint16_t port, uint32_t* value) {
    if (!value) {
        return HAL_ERROR_INVALID_PARAM;
    }
    *value = io_inl(port);
    return HAL_SUCCESS;
}

hal_status_t hal_io_write32(uint16_t port, uint32_t value) {
    io_outl(port, value);
    return HAL_SUCCESS;
}

/**
 * System control
 */
void hal_halt(void) {
    cpu_halt();
}

void hal_reboot(void) {
    // Try to reboot via keyboard controller
    uint8_t value;
    do {
        value = io_inb(0x64);
        if ((value & 0x02) == 0) break;
        io_delay();
    } while (1);
    
    io_outb(0x64, 0xFE); // Send reboot command
    cpu_halt(); // Fallback to halt
}

hal_status_t hal_get_system_info(void* info_buffer, size_t buffer_size) {
    // TODO: Implement system info gathering
    (void)info_buffer;
    (void)buffer_size;
    return HAL_ERROR_NOT_IMPLEMENTED;
}

/**
 * Debug and logging
 */
hal_status_t hal_debug_putchar(char c) {
    serial_write_char(c);
    return HAL_SUCCESS;
}

hal_status_t hal_debug_puts(const char* str) {
    if (!str) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    while (*str) {
        hal_debug_putchar(*str++);
    }
    return HAL_SUCCESS;
}

hal_status_t hal_log_init(void) {
    serial_init_com1();
    return HAL_SUCCESS;
}
