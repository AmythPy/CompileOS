/**
 * CompileOS Timer Implementation
 */

#include "timer.h"
#include "interrupts.h"
#include <stdint.h>

// PIT I/O ports
#define PIT_CHANNEL0    0x40
#define PIT_COMMAND     0x43

// PIT frequency (1.193182 MHz)
#define PIT_FREQUENCY   1193182

// Global tick counter
static volatile uint64_t timer_ticks = 0;

// Port I/O
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

/**
 * Timer interrupt handler
 */
static void timer_handler(void) {
    timer_ticks++;
}

/**
 * Initialize timer
 */
void timer_init(uint32_t frequency) {
    // Register timer interrupt handler
    register_interrupt_handler(32, timer_handler);

    // Calculate divisor for desired frequency
    uint32_t divisor = PIT_FREQUENCY / frequency;

    // Send command byte: Channel 0, lobyte/hibyte, rate generator
    outb(PIT_COMMAND, 0x36);

    // Send divisor
    outb(PIT_CHANNEL0, divisor & 0xFF);         // Low byte
    outb(PIT_CHANNEL0, (divisor >> 8) & 0xFF);  // High byte
}

/**
 * Get current tick count
 */
uint64_t timer_get_ticks(void) {
    return timer_ticks;
}

/**
 * Sleep for specified milliseconds
 */
void timer_sleep(uint32_t ms) {
    uint64_t end_ticks = timer_ticks + ms;
    while (timer_ticks < end_ticks) {
        __asm__ volatile("hlt");  // Halt until next interrupt
    }
}
