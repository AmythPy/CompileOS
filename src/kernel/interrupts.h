/**
 * CompileOS Interrupt Management
 * IDT setup and interrupt handlers
 */

#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>

// IDT entry structure (32-bit)
typedef struct {
    uint16_t offset_low;    // Lower 16 bits of handler address
    uint16_t selector;      // Kernel segment selector (0x08)
    uint8_t  zero;          // Always 0
    uint8_t  type_attr;     // Type and attributes
    uint16_t offset_high;   // Upper 16 bits of handler address
} __attribute__((packed)) idt_entry_t;

// IDT pointer structure
typedef struct {
    uint16_t limit;         // Size of IDT - 1
    uint32_t base;          // Address of IDT
} __attribute__((packed)) idt_ptr_t;

// Number of IDT entries
#define IDT_ENTRIES 256

// Initialize IDT
void idt_init(void);

// Set an IDT gate
void idt_set_gate(uint8_t num, uint32_t handler, uint16_t selector, uint8_t flags);

// Interrupt handler type
typedef void (*interrupt_handler_t)(void);

// Register interrupt handler
void register_interrupt_handler(uint8_t num, interrupt_handler_t handler);

// Common interrupt handlers (defined in interrupts.asm)
extern void isr0(void);   // Divide by zero
extern void isr1(void);   // Debug
extern void isr2(void);   // NMI
extern void isr3(void);   // Breakpoint
extern void isr6(void);   // Invalid opcode
extern void isr13(void);  // General protection fault
extern void isr14(void);  // Page fault

// IRQ handlers (hardware interrupts)
extern void irq0(void);   // Timer
extern void irq1(void);   // Keyboard
extern void irq12(void);  // Mouse

// Syscall interrupt
extern void isr128(void); // INT 0x80 for syscalls

// PIC (Programmable Interrupt Controller) functions
void pic_remap(void);
void pic_send_eoi(uint8_t irq);

#endif // INTERRUPTS_H
