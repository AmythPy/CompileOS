/**
 * CompileOS x86_64 I/O Interface - Bare Metal
 * 
 * Direct port I/O and memory-mapped I/O access
 */

#ifndef X86_64_IO_H
#define X86_64_IO_H

#include <stdint.h>

// Port I/O functions
static inline uint8_t io_inb(uint16_t port) {
    uint8_t value;
    __asm__ volatile ("inb %1, %0" : "=a" (value) : "Nd" (port));
    return value;
}

static inline void io_outb(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" : : "a" (value), "Nd" (port));
}

static inline uint16_t io_inw(uint16_t port) {
    uint16_t value;
    __asm__ volatile ("inw %1, %0" : "=a" (value) : "Nd" (port));
    return value;
}

static inline void io_outw(uint16_t port, uint16_t value) {
    __asm__ volatile ("outw %0, %1" : : "a" (value), "Nd" (port));
}

static inline uint32_t io_inl(uint16_t port) {
    uint32_t value;
    __asm__ volatile ("inl %1, %0" : "=a" (value) : "Nd" (port));
    return value;
}

static inline void io_outl(uint16_t port, uint32_t value) {
    __asm__ volatile ("outl %0, %1" : : "a" (value), "Nd" (port));
}

// String I/O functions
static inline void io_insb(uint16_t port, void* buffer, uint32_t count) {
    __asm__ volatile ("rep insb" : "+D" (buffer), "+c" (count) : "d" (port) : "memory");
}

static inline void io_insw(uint16_t port, void* buffer, uint32_t count) {
    __asm__ volatile ("rep insw" : "+D" (buffer), "+c" (count) : "d" (port) : "memory");
}

static inline void io_insl(uint16_t port, void* buffer, uint32_t count) {
    __asm__ volatile ("rep insl" : "+D" (buffer), "+c" (count) : "d" (port) : "memory");
}

static inline void io_outsb(uint16_t port, const void* buffer, uint32_t count) {
    __asm__ volatile ("rep outsb" : "+S" (buffer), "+c" (count) : "d" (port) : "memory");
}

static inline void io_outsw(uint16_t port, const void* buffer, uint32_t count) {
    __asm__ volatile ("rep outsw" : "+S" (buffer), "+c" (count) : "d" (port) : "memory");
}

static inline void io_outsl(uint16_t port, const void* buffer, uint32_t count) {
    __asm__ volatile ("rep outsl" : "+S" (buffer), "+c" (count) : "d" (port) : "memory");
}

// Memory-mapped I/O
static inline uint8_t mmio_read8(volatile void* address) {
    return *(volatile uint8_t*)address;
}

static inline void mmio_write8(volatile void* address, uint8_t value) {
    *(volatile uint8_t*)address = value;
}

static inline uint16_t mmio_read16(volatile void* address) {
    return *(volatile uint16_t*)address;
}

static inline void mmio_write16(volatile void* address, uint16_t value) {
    *(volatile uint16_t*)address = value;
}

static inline uint32_t mmio_read32(volatile void* address) {
    return *(volatile uint32_t*)address;
}

static inline void mmio_write32(volatile void* address, uint32_t value) {
    *(volatile uint32_t*)address = value;
}

static inline uint64_t mmio_read64(volatile void* address) {
    return *(volatile uint64_t*)address;
}

static inline void mmio_write64(volatile void* address, uint64_t value) {
    *(volatile uint64_t*)address = value;
}

// Common I/O ports
#define COM1_PORT 0x3F8
#define COM2_PORT 0x2F8
#define COM3_PORT 0x3E8
#define COM4_PORT 0x2E8

#define VGA_CRTC_INDEX 0x3D4
#define VGA_CRTC_DATA 0x3D5
#define VGA_MISC_OUTPUT 0x3C2
#define VGA_SEQUENCER_INDEX 0x3C4
#define VGA_SEQUENCER_DATA 0x3C5
#define VGA_GRAPHICS_INDEX 0x3CE
#define VGA_GRAPHICS_DATA 0x3CF
#define VGA_ATTRIBUTE_INDEX 0x3C0
#define VGA_ATTRIBUTE_DATA 0x3C1

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1

#define PIT_CHANNEL0 0x40
#define PIT_CHANNEL1 0x41
#define PIT_CHANNEL2 0x42
#define PIT_COMMAND 0x43

#define CMOS_INDEX 0x70
#define CMOS_DATA 0x71

// I/O delay
static inline void io_delay(void) {
    __asm__ volatile ("jmp 1f; 1: jmp 1f; 1:");
}

#endif // X86_64_IO_H
