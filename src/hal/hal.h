/**
 * CompileOS Hardware Abstraction Layer (HAL)
 * 
 * Provides a hardware-agnostic interface for kernel components
 */

#ifndef HAL_H
#define HAL_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// HAL status codes
typedef enum {
    HAL_SUCCESS = 0,
    HAL_ERROR_INVALID_PARAM = -1,
    HAL_ERROR_NOT_IMPLEMENTED = -2,
    HAL_ERROR_HARDWARE_FAILURE = -3,
    HAL_ERROR_TIMEOUT = -4
} hal_status_t;

// CPU architecture detection
typedef enum {
    ARCH_UNKNOWN = 0,
    ARCH_X86_32,
    ARCH_X86_64,
    ARCH_ARM32,
    ARCH_ARM64,
    ARCH_RISCV32,
    ARCH_RISCV64
} cpu_arch_t;

// Memory region types
typedef enum {
    MEMORY_TYPE_RAM = 0,
    MEMORY_TYPE_ROM,
    MEMORY_TYPE_MMIO,
    MEMORY_TYPE_RESERVED
} memory_type_t;

// Memory region descriptor
typedef struct {
    uint64_t base_address;
    uint64_t size;
    memory_type_t type;
    bool is_available;
} memory_region_t;

// Interrupt handler type
typedef void (*interrupt_handler_t)(uint32_t interrupt_number, void* context);

// Timer callback type
typedef void (*timer_callback_t)(void* context);

// HAL initialization and cleanup
hal_status_t hal_init(void);
void hal_shutdown(void);

// CPU and architecture
cpu_arch_t hal_get_cpu_architecture(void);
const char* hal_get_cpu_architecture_string(void);
uint32_t hal_get_cpu_count(void);

// Memory management
hal_status_t hal_memory_map(memory_region_t* regions, size_t max_regions, size_t* actual_count);
hal_status_t hal_memory_protect(void* address, size_t size, bool read, bool write, bool execute);
hal_status_t hal_memory_flush_cache(void* address, size_t size);

// Interrupt handling
hal_status_t hal_interrupt_register(uint32_t interrupt_number, interrupt_handler_t handler, void* context);
hal_status_t hal_interrupt_enable(uint32_t interrupt_number);
hal_status_t hal_interrupt_disable(uint32_t interrupt_number);
hal_status_t hal_interrupt_acknowledge(uint32_t interrupt_number);

// Timer functions
hal_status_t hal_timer_init(uint32_t frequency_hz);
hal_status_t hal_timer_register_callback(timer_callback_t callback, void* context);
uint64_t hal_timer_get_ticks(void);
uint64_t hal_timer_ticks_to_ns(uint64_t ticks);

// I/O operations
hal_status_t hal_io_read8(uint16_t port, uint8_t* value);
hal_status_t hal_io_write8(uint16_t port, uint8_t value);
hal_status_t hal_io_read16(uint16_t port, uint16_t* value);
hal_status_t hal_io_write16(uint16_t port, uint16_t value);
hal_status_t hal_io_read32(uint16_t port, uint32_t* value);
hal_status_t hal_io_write32(uint16_t port, uint32_t value);

// System control
void hal_halt(void);
void hal_reboot(void);
hal_status_t hal_get_system_info(void* info_buffer, size_t buffer_size);

// Debugging and logging
hal_status_t hal_debug_putchar(char c);
hal_status_t hal_debug_puts(const char* str);
hal_status_t hal_log_init(void);

#endif // HAL_H
