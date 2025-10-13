/**
 * CompileOS Interrupt Management Implementation
 */

#include "interrupts.h"
#include <stdint.h>
#include <string.h>

// IDT table
static idt_entry_t idt[IDT_ENTRIES];
static idt_ptr_t idt_ptr;

// Interrupt handler table
static interrupt_handler_t interrupt_handlers[IDT_ENTRIES];

// Port I/O functions
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

/**
 * Set an IDT gate
 */
void idt_set_gate(uint8_t num, uint32_t handler, uint16_t selector, uint8_t flags) {
    idt[num].offset_low = handler & 0xFFFF;
    idt[num].offset_high = (handler >> 16) & 0xFFFF;
    idt[num].selector = selector;
    idt[num].zero = 0;
    idt[num].type_attr = flags;
}

/**
 * Load IDT
 */
static void idt_load(void) {
    __asm__ volatile("lidt %0" : : "m"(idt_ptr));
}

/**
 * Remap PIC (Programmable Interrupt Controller)
 * Default IRQ mappings conflict with CPU exceptions
 * Remap IRQ 0-15 to interrupts 32-47
 */
void pic_remap(void) {
    // Save masks
    uint8_t mask1 = inb(0x21);
    uint8_t mask2 = inb(0xA1);

    // Start initialization sequence
    outb(0x20, 0x11);  // ICW1: Initialize + ICW4
    outb(0xA0, 0x11);

    // ICW2: Set interrupt vector offsets
    outb(0x21, 0x20);  // Master PIC: IRQ 0-7 -> INT 32-39
    outb(0xA1, 0x28);  // Slave PIC: IRQ 8-15 -> INT 40-47

    // ICW3: Tell PICs about each other
    outb(0x21, 0x04);  // Master: slave on IRQ2
    outb(0xA1, 0x02);  // Slave: cascade identity

    // ICW4: 8086 mode
    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    // Restore masks
    outb(0x21, mask1);
    outb(0xA1, mask2);
}

/**
 * Send End of Interrupt to PIC
 */
void pic_send_eoi(uint8_t irq) {
    if (irq >= 8) {
        outb(0xA0, 0x20);  // Send EOI to slave
    }
    outb(0x20, 0x20);      // Send EOI to master
}

/**
 * Register an interrupt handler
 */
void register_interrupt_handler(uint8_t num, interrupt_handler_t handler) {
    interrupt_handlers[num] = handler;
}

/**
 * Common interrupt handler (called from ASM stubs)
 */
void interrupt_handler_common(uint32_t int_no, uint32_t err_code) {
    // Call registered handler if exists
    if (interrupt_handlers[int_no]) {
        interrupt_handlers[int_no]();
    }

    // Send EOI for hardware interrupts (IRQs)
    if (int_no >= 32 && int_no < 48) {
        pic_send_eoi(int_no - 32);
    }

    // Suppress unused parameter warnings
    (void)err_code;
}

/**
 * Initialize IDT
 */
void idt_init(void) {
    // Clear IDT and handlers
    memset(&idt, 0, sizeof(idt));
    memset(&interrupt_handlers, 0, sizeof(interrupt_handlers));

    // Set up IDT pointer
    idt_ptr.limit = sizeof(idt) - 1;
    idt_ptr.base = (uint32_t)&idt;

    // Remap PIC before setting up IRQs
    pic_remap();

    // Set up exception handlers (0-31)
    // 0x8E = Present, Ring 0, 32-bit Interrupt Gate
    idt_set_gate(0, (uint32_t)isr0, 0x08, 0x8E);    // Divide by zero
    idt_set_gate(1, (uint32_t)isr1, 0x08, 0x8E);    // Debug
    idt_set_gate(2, (uint32_t)isr2, 0x08, 0x8E);    // NMI
    idt_set_gate(3, (uint32_t)isr3, 0x08, 0x8E);    // Breakpoint
    idt_set_gate(6, (uint32_t)isr6, 0x08, 0x8E);    // Invalid opcode
    idt_set_gate(13, (uint32_t)isr13, 0x08, 0x8E);  // General protection
    idt_set_gate(14, (uint32_t)isr14, 0x08, 0x8E);  // Page fault

    // Set up IRQ handlers (32-47)
    idt_set_gate(32, (uint32_t)irq0, 0x08, 0x8E);   // Timer
    idt_set_gate(33, (uint32_t)irq1, 0x08, 0x8E);   // Keyboard
    idt_set_gate(44, (uint32_t)irq12, 0x08, 0x8E);  // Mouse

    // Set up syscall handler (128 = 0x80)
    idt_set_gate(128, (uint32_t)isr128, 0x08, 0x8E);

    // Load IDT
    idt_load();

    // Enable interrupts
    __asm__ volatile("sti");
}
