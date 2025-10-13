/**
 * CompileOS x86_64 Interrupt Handling - Bare Metal
 * 
 * Interrupt Descriptor Table (IDT) and interrupt handling
 */

#ifndef X86_64_INTERRUPTS_H
#define X86_64_INTERRUPTS_H

#include <stdint.h>
#include <stdbool.h>

// Interrupt vector numbers
#define INT_DIVIDE_ERROR 0
#define INT_DEBUG 1
#define INT_NMI 2
#define INT_BREAKPOINT 3
#define INT_OVERFLOW 4
#define INT_BOUND_RANGE 5
#define INT_INVALID_OPCODE 6
#define INT_DEVICE_NOT_AVAILABLE 7
#define INT_DOUBLE_FAULT 8
#define INT_COPROCESSOR_SEGMENT 9
#define INT_INVALID_TSS 10
#define INT_SEGMENT_NOT_PRESENT 11
#define INT_STACK_FAULT 12
#define INT_GENERAL_PROTECTION 13
#define INT_PAGE_FAULT 14
#define INT_FPU_ERROR 16
#define INT_ALIGNMENT_CHECK 17
#define INT_MACHINE_CHECK 18
#define INT_SIMD_FPU_ERROR 19
#define INT_VIRTUALIZATION 20
#define INT_SECURITY 30

// IRQ numbers (mapped to interrupt vectors 32-47)
#define IRQ_TIMER 0
#define IRQ_KEYBOARD 1
#define IRQ_CASCADE 2
#define IRQ_COM2 3
#define IRQ_COM1 4
#define IRQ_LPT2 5
#define IRQ_FLOPPY 6
#define IRQ_LPT1 7
#define IRQ_CMOS 8
#define IRQ_FREE1 9
#define IRQ_FREE2 10
#define IRQ_FREE3 11
#define IRQ_PS2 12
#define IRQ_FPU 13
#define IRQ_ATA_PRIMARY 14
#define IRQ_ATA_SECONDARY 15

// Interrupt gate types
#define GATE_TYPE_INTERRUPT 0x8E
#define GATE_TYPE_TRAP 0x8F
#define GATE_TYPE_TASK 0x85

// IDT entry structure
typedef struct {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t type_attr;
    uint16_t offset_middle;
    uint32_t offset_high;
    uint32_t reserved;
} __attribute__((packed)) idt_entry_t;

// IDT descriptor structure
typedef struct {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) idt_descriptor_t;

// Interrupt handler function type
typedef void (*interrupt_handler_func_t)(uint32_t interrupt_number, uint32_t error_code, void* context);

// Interrupt context structure
typedef struct {
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t rbp;
    uint64_t r8;
    uint64_t r9;
    uint64_t r10;
    uint64_t r11;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
    uint64_t interrupt_number;
    uint64_t error_code;
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
} interrupt_context_t;

// Interrupt management functions
void interrupts_init(void);
void interrupts_load_idt(void);
void interrupts_set_handler(uint8_t vector, interrupt_handler_func_t handler, uint8_t type);
void interrupts_enable(void);
void interrupts_disable(void);
bool interrupts_are_enabled(void);

// PIC (Programmable Interrupt Controller) functions
void pic_init(void);
void pic_enable_irq(uint8_t irq);
void pic_disable_irq(uint8_t irq);
void pic_send_eoi(uint8_t irq);
void pic_mask_all(void);
void pic_unmask_all(void);

// Interrupt handler stubs (defined in assembly)
void interrupt_handler_0(void);
void interrupt_handler_1(void);
void interrupt_handler_2(void);
void interrupt_handler_3(void);
void interrupt_handler_4(void);
void interrupt_handler_5(void);
void interrupt_handler_6(void);
void interrupt_handler_7(void);
void interrupt_handler_8(void);
void interrupt_handler_9(void);
void interrupt_handler_10(void);
void interrupt_handler_11(void);
void interrupt_handler_12(void);
void interrupt_handler_13(void);
void interrupt_handler_14(void);
void interrupt_handler_15(void);
void interrupt_handler_16(void);
void interrupt_handler_17(void);
void interrupt_handler_18(void);
void interrupt_handler_19(void);
void interrupt_handler_20(void);
void interrupt_handler_21(void);
void interrupt_handler_22(void);
void interrupt_handler_23(void);
void interrupt_handler_24(void);
void interrupt_handler_25(void);
void interrupt_handler_26(void);
void interrupt_handler_27(void);
void interrupt_handler_28(void);
void interrupt_handler_29(void);
void interrupt_handler_30(void);
void interrupt_handler_31(void);

// IRQ handlers
void irq_handler_0(void);
void irq_handler_1(void);
void irq_handler_2(void);
void irq_handler_3(void);
void irq_handler_4(void);
void irq_handler_5(void);
void irq_handler_6(void);
void irq_handler_7(void);
void irq_handler_8(void);
void irq_handler_9(void);
void irq_handler_10(void);
void irq_handler_11(void);
void irq_handler_12(void);
void irq_handler_13(void);
void irq_handler_14(void);
void irq_handler_15(void);

// Common interrupt handler
void interrupt_handler_common(interrupt_context_t* context);

// Exception handlers
void exception_divide_error(interrupt_context_t* context);
void exception_debug(interrupt_context_t* context);
void exception_nmi(interrupt_context_t* context);
void exception_breakpoint(interrupt_context_t* context);
void exception_overflow(interrupt_context_t* context);
void exception_bound_range(interrupt_context_t* context);
void exception_invalid_opcode(interrupt_context_t* context);
void exception_device_not_available(interrupt_context_t* context);
void exception_double_fault(interrupt_context_t* context);
void exception_coprocessor_segment(interrupt_context_t* context);
void exception_invalid_tss(interrupt_context_t* context);
void exception_segment_not_present(interrupt_context_t* context);
void exception_stack_fault(interrupt_context_t* context);
void exception_general_protection(interrupt_context_t* context);
void exception_page_fault(interrupt_context_t* context);
void exception_fpu_error(interrupt_context_t* context);
void exception_alignment_check(interrupt_context_t* context);
void exception_machine_check(interrupt_context_t* context);
void exception_simd_fpu_error(interrupt_context_t* context);
void exception_virtualization(interrupt_context_t* context);
void exception_security(interrupt_context_t* context);

#endif // X86_64_INTERRUPTS_H










