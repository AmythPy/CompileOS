/**
 * CompileOS x86_64 Interrupt Handling Implementation - Bare Metal
 * 
 * Interrupt Descriptor Table (IDT) and interrupt handling
 */

#include "interrupts.h"
#include "io.h"
#include <string.h>

// External keyboard handler
extern void keyboard_irq_handler(void);

// IDT and IDT descriptor
static idt_entry_t idt[256];
static idt_descriptor_t idt_desc;

// Interrupt handler table
static interrupt_handler_func_t interrupt_handlers[256] = {0};

// Initialize interrupts
void interrupts_init(void) {
    // Clear IDT
    memset(idt, 0, sizeof(idt));
    
    // Set up IDT descriptor
    idt_desc.limit = sizeof(idt) - 1;
    idt_desc.base = (uint64_t)idt;
    
    // Set up interrupt handlers
    interrupts_set_handler(0, (interrupt_handler_func_t)interrupt_handler_0, GATE_TYPE_INTERRUPT);
    interrupts_set_handler(1, (interrupt_handler_func_t)interrupt_handler_1, GATE_TYPE_INTERRUPT);
    interrupts_set_handler(2, (interrupt_handler_func_t)interrupt_handler_2, GATE_TYPE_INTERRUPT);
    interrupts_set_handler(3, (interrupt_handler_func_t)interrupt_handler_3, GATE_TYPE_INTERRUPT);
    interrupts_set_handler(4, (interrupt_handler_func_t)interrupt_handler_4, GATE_TYPE_INTERRUPT);
    interrupts_set_handler(5, (interrupt_handler_func_t)interrupt_handler_5, GATE_TYPE_INTERRUPT);
    interrupts_set_handler(6, (interrupt_handler_func_t)interrupt_handler_6, GATE_TYPE_INTERRUPT);
    interrupts_set_handler(7, (interrupt_handler_func_t)interrupt_handler_7, GATE_TYPE_INTERRUPT);
    interrupts_set_handler(8, (interrupt_handler_func_t)interrupt_handler_8, GATE_TYPE_INTERRUPT);
    interrupts_set_handler(9, (interrupt_handler_func_t)interrupt_handler_9, GATE_TYPE_INTERRUPT);
    interrupts_set_handler(10, (interrupt_handler_func_t)interrupt_handler_10, GATE_TYPE_INTERRUPT);
    interrupts_set_handler(11, (interrupt_handler_func_t)interrupt_handler_11, GATE_TYPE_INTERRUPT);
    interrupts_set_handler(12, (interrupt_handler_func_t)interrupt_handler_12, GATE_TYPE_INTERRUPT);
    interrupts_set_handler(13, (interrupt_handler_func_t)interrupt_handler_13, GATE_TYPE_INTERRUPT);
    interrupts_set_handler(14, (interrupt_handler_func_t)interrupt_handler_14, GATE_TYPE_INTERRUPT);
    interrupts_set_handler(15, (interrupt_handler_func_t)interrupt_handler_15, GATE_TYPE_INTERRUPT);
    interrupts_set_handler(16, (interrupt_handler_func_t)interrupt_handler_16, GATE_TYPE_INTERRUPT);
    interrupts_set_handler(17, (interrupt_handler_func_t)interrupt_handler_17, GATE_TYPE_INTERRUPT);
    interrupts_set_handler(18, (interrupt_handler_func_t)interrupt_handler_18, GATE_TYPE_INTERRUPT);
    interrupts_set_handler(19, (interrupt_handler_func_t)interrupt_handler_19, GATE_TYPE_INTERRUPT);
    interrupts_set_handler(20, (interrupt_handler_func_t)interrupt_handler_20, GATE_TYPE_INTERRUPT);
    interrupts_set_handler(21, (interrupt_handler_func_t)interrupt_handler_21, GATE_TYPE_INTERRUPT);
    interrupts_set_handler(22, (interrupt_handler_func_t)interrupt_handler_22, GATE_TYPE_INTERRUPT);
    interrupts_set_handler(23, (interrupt_handler_func_t)interrupt_handler_23, GATE_TYPE_INTERRUPT);
    interrupts_set_handler(24, (interrupt_handler_func_t)interrupt_handler_24, GATE_TYPE_INTERRUPT);
    interrupts_set_handler(25, (interrupt_handler_func_t)interrupt_handler_25, GATE_TYPE_INTERRUPT);
    interrupts_set_handler(26, (interrupt_handler_func_t)interrupt_handler_26, GATE_TYPE_INTERRUPT);
    interrupts_set_handler(27, (interrupt_handler_func_t)interrupt_handler_27, GATE_TYPE_INTERRUPT);
    interrupts_set_handler(28, (interrupt_handler_func_t)interrupt_handler_28, GATE_TYPE_INTERRUPT);
    interrupts_set_handler(29, (interrupt_handler_func_t)interrupt_handler_29, GATE_TYPE_INTERRUPT);
    interrupts_set_handler(30, (interrupt_handler_func_t)interrupt_handler_30, GATE_TYPE_INTERRUPT);
    interrupts_set_handler(31, (interrupt_handler_func_t)interrupt_handler_31, GATE_TYPE_INTERRUPT);
    
    // Set up IRQ handlers (32-47)
    interrupts_set_handler(32, (interrupt_handler_func_t)irq_handler_0, GATE_TYPE_INTERRUPT);
    interrupts_set_handler(33, (interrupt_handler_func_t)irq_handler_1, GATE_TYPE_INTERRUPT);
    interrupts_set_handler(34, (interrupt_handler_func_t)irq_handler_2, GATE_TYPE_INTERRUPT);
    interrupts_set_handler(35, (interrupt_handler_func_t)irq_handler_3, GATE_TYPE_INTERRUPT);
    interrupts_set_handler(36, (interrupt_handler_func_t)irq_handler_4, GATE_TYPE_INTERRUPT);
    interrupts_set_handler(37, (interrupt_handler_func_t)irq_handler_5, GATE_TYPE_INTERRUPT);
    interrupts_set_handler(38, (interrupt_handler_func_t)irq_handler_6, GATE_TYPE_INTERRUPT);
    interrupts_set_handler(39, (interrupt_handler_func_t)irq_handler_7, GATE_TYPE_INTERRUPT);
    interrupts_set_handler(40, (interrupt_handler_func_t)irq_handler_8, GATE_TYPE_INTERRUPT);
    interrupts_set_handler(41, (interrupt_handler_func_t)irq_handler_9, GATE_TYPE_INTERRUPT);
    interrupts_set_handler(42, (interrupt_handler_func_t)irq_handler_10, GATE_TYPE_INTERRUPT);
    interrupts_set_handler(43, (interrupt_handler_func_t)irq_handler_11, GATE_TYPE_INTERRUPT);
    interrupts_set_handler(44, (interrupt_handler_func_t)irq_handler_12, GATE_TYPE_INTERRUPT);
    interrupts_set_handler(45, (interrupt_handler_func_t)irq_handler_13, GATE_TYPE_INTERRUPT);
    interrupts_set_handler(46, (interrupt_handler_func_t)irq_handler_14, GATE_TYPE_INTERRUPT);
    interrupts_set_handler(47, (interrupt_handler_func_t)irq_handler_15, GATE_TYPE_INTERRUPT);
    
    // Initialize PIC
    pic_init();
    
    // Enable keyboard IRQ (IRQ 1)
    pic_enable_irq(1);
    
    // Load IDT
    interrupts_load_idt();
    
    // Enable interrupts
    interrupts_enable();
}

// Load IDT
void interrupts_load_idt(void) {
    __asm__ volatile ("lidt %0" : : "m" (idt_desc));
}

// Set interrupt handler
void interrupts_set_handler(uint8_t vector, interrupt_handler_func_t handler, uint8_t type) {
    uint64_t address = (uint64_t)handler;
    
    idt[vector].offset_low = address & 0xFFFF;
    idt[vector].offset_middle = (address >> 16) & 0xFFFF;
    idt[vector].offset_high = (address >> 32) & 0xFFFFFFFF;
    idt[vector].selector = 0x08; // Kernel code segment
    idt[vector].ist = 0;
    idt[vector].type_attr = type;
    idt[vector].reserved = 0;
    
    interrupt_handlers[vector] = handler;
}

// Enable interrupts
void interrupts_enable(void) {
    __asm__ volatile ("sti");
}

// Disable interrupts
void interrupts_disable(void) {
    __asm__ volatile ("cli");
}

// Check if interrupts are enabled
bool interrupts_are_enabled(void) {
    uint64_t rflags;
    __asm__ volatile ("pushfq; pop %0" : "=r" (rflags));
    return (rflags & 0x200) != 0;
}

// PIC initialization
void pic_init(void) {
    // ICW1: Initialize PIC
    io_outb(PIC1_COMMAND, 0x11);
    io_outb(PIC2_COMMAND, 0x11);
    
    // ICW2: Set interrupt vector offsets
    io_outb(PIC1_DATA, 0x20); // IRQ 0-7 -> INT 32-39
    io_outb(PIC2_DATA, 0x28); // IRQ 8-15 -> INT 40-47
    
    // ICW3: Set up cascading
    io_outb(PIC1_DATA, 0x04); // PIC1 has slave at IRQ 2
    io_outb(PIC2_DATA, 0x02); // PIC2 is slave
    
    // ICW4: Set up mode
    io_outb(PIC1_DATA, 0x01); // 8086 mode
    io_outb(PIC2_DATA, 0x01); // 8086 mode
    
    // Mask all interrupts initially
    pic_mask_all();
}

// Enable IRQ
void pic_enable_irq(uint8_t irq) {
    uint16_t port;
    uint8_t mask;
    
    if (irq < 8) {
        port = PIC1_DATA;
        mask = ~(1 << irq);
    } else {
        port = PIC2_DATA;
        mask = ~(1 << (irq - 8));
    }
    
    uint8_t current = io_inb(port);
    io_outb(port, current & mask);
}

// Disable IRQ
void pic_disable_irq(uint8_t irq) {
    uint16_t port;
    uint8_t mask;
    
    if (irq < 8) {
        port = PIC1_DATA;
        mask = 1 << irq;
    } else {
        port = PIC2_DATA;
        mask = 1 << (irq - 8);
    }
    
    uint8_t current = io_inb(port);
    io_outb(port, current | mask);
}

// Send EOI (End of Interrupt)
void pic_send_eoi(uint8_t irq) {
    if (irq >= 8) {
        io_outb(PIC2_COMMAND, 0x20); // EOI to slave
    }
    io_outb(PIC1_COMMAND, 0x20); // EOI to master
}

// Mask all interrupts
void pic_mask_all(void) {
    io_outb(PIC1_DATA, 0xFF);
    io_outb(PIC2_DATA, 0xFF);
}

// Unmask all interrupts
void pic_unmask_all(void) {
    io_outb(PIC1_DATA, 0x00);
    io_outb(PIC2_DATA, 0x00);
}

// First interrupt_handler_common removed - using merged version below

// Exception handlers (simplified implementations)
void exception_divide_error(interrupt_context_t* context) {
    volatile uint16_t* v = (uint16_t*)0xB8000;
    for (int i = 0; i < 80*25; i++) v[i] = 0x4F20; // red bg, white text
    const char* msg = "EXCEPTION: DIVIDE ERROR";
    for (int i = 0; msg[i]; i++) v[i] = (0x4F00 | msg[i]);
    (void)context;
}

void exception_debug(interrupt_context_t* context) {
    // TODO: Implement debug exception handling
    (void)context;
}

void exception_nmi(interrupt_context_t* context) {
    // TODO: Implement NMI handling
    (void)context;
}

void exception_breakpoint(interrupt_context_t* context) {
    // TODO: Implement breakpoint handling
    (void)context;
}

void exception_overflow(interrupt_context_t* context) {
    // TODO: Implement overflow handling
    (void)context;
}

void exception_bound_range(interrupt_context_t* context) {
    // TODO: Implement bound range handling
    (void)context;
}

void exception_invalid_opcode(interrupt_context_t* context) {
    volatile uint16_t* v = (uint16_t*)0xB8000;
    for (int i = 0; i < 80*25; i++) v[i] = 0x4F20;
    const char* msg = "EXCEPTION: INVALID OPCODE";
    for (int i = 0; msg[i]; i++) v[i] = (0x4F00 | msg[i]);
    (void)context;
}

void exception_device_not_available(interrupt_context_t* context) {
    // TODO: Implement device not available handling
    (void)context;
}

void exception_double_fault(interrupt_context_t* context) {
    // TODO: Implement double fault handling
    (void)context;
}

void exception_coprocessor_segment(interrupt_context_t* context) {
    // TODO: Implement coprocessor segment handling
    (void)context;
}

void exception_invalid_tss(interrupt_context_t* context) {
    // TODO: Implement invalid TSS handling
    (void)context;
}

void exception_segment_not_present(interrupt_context_t* context) {
    // TODO: Implement segment not present handling
    (void)context;
}

void exception_stack_fault(interrupt_context_t* context) {
    // TODO: Implement stack fault handling
    (void)context;
}

void exception_general_protection(interrupt_context_t* context) {
    volatile uint16_t* v = (uint16_t*)0xB8000;
    for (int i = 0; i < 80*25; i++) v[i] = 0x4F20;
    const char* msg = "EXCEPTION: GENERAL PROTECTION";
    for (int i = 0; msg[i]; i++) v[i] = (0x4F00 | msg[i]);
    (void)context;
}

void exception_page_fault(interrupt_context_t* context) {
    volatile uint16_t* v = (uint16_t*)0xB8000;
    for (int i = 0; i < 80*25; i++) v[i] = 0x4F20;
    const char* msg = "EXCEPTION: PAGE FAULT";
    for (int i = 0; msg[i]; i++) v[i] = (0x4F00 | msg[i]);
    (void)context;
}

void exception_fpu_error(interrupt_context_t* context) {
    // TODO: Implement FPU error handling
    (void)context;
}

void exception_alignment_check(interrupt_context_t* context) {
    // TODO: Implement alignment check handling
    (void)context;
}

void exception_machine_check(interrupt_context_t* context) {
    // TODO: Implement machine check handling
    (void)context;
}

void exception_simd_fpu_error(interrupt_context_t* context) {
    // TODO: Implement SIMD FPU error handling
    (void)context;
}

void exception_virtualization(interrupt_context_t* context) {
    // TODO: Implement virtualization handling
    (void)context;
}

void exception_security(interrupt_context_t* context) {
    // TODO: Implement security handling
    (void)context;
}

// Common interrupt handler
void interrupt_handler_common(interrupt_context_t* context) {
    uint32_t int_num = context->interrupt_number;
    
    // Handle timer interrupt (IRQ 0 = interrupt 32)
    if (int_num == 32) {
        // Notify HAL timer and send EOI
        extern void hal_timer_on_interrupt(void);
        hal_timer_on_interrupt();
        pic_send_eoi(0);
        return;
    }

    // Handle keyboard interrupt (IRQ 1 = interrupt 33)
    if (int_num == 33) {
        keyboard_irq_handler();
        pic_send_eoi(1);
        return;
    }
    
    // Handle other interrupts
    if (int_num < 32) {
        // Exception handler
        // TODO: Implement proper exception handling
    } else if (int_num >= 32 && int_num < 48) {
        // IRQ handler
        uint8_t irq = int_num - 32;
        pic_send_eoi(irq);
    }
}





