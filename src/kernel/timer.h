/**
 * CompileOS Programmable Interval Timer (PIT)
 * Provides system timing and scheduling ticks
 */

#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

// Initialize timer with frequency in Hz
void timer_init(uint32_t frequency);

// Get current tick count
uint64_t timer_get_ticks(void);

// Sleep for specified milliseconds
void timer_sleep(uint32_t ms);

#endif // TIMER_H
