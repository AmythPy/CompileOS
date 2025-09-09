; CompileOS x86_64 Interrupt Handlers - Bare Metal Assembly
; 
; Interrupt handler stubs and common interrupt handler

[BITS 64]

; External symbols
extern interrupt_handler_common

; Macro to create interrupt handler stub
%macro INTERRUPT_HANDLER 1
global interrupt_handler_%1
interrupt_handler_%1:
    push qword 0          ; Push dummy error code
    push qword %1         ; Push interrupt number
    jmp interrupt_handler_common_stub
%endmacro

; Macro to create interrupt handler stub with error code
%macro INTERRUPT_HANDLER_ERROR 1
global interrupt_handler_%1
interrupt_handler_%1:
    push qword %1         ; Push interrupt number
    jmp interrupt_handler_common_stub
%endmacro

; Macro to create IRQ handler stub
%macro IRQ_HANDLER 1
global irq_handler_%1
irq_handler_%1:
    push qword 0          ; Push dummy error code
    push qword %1         ; Push IRQ number
    jmp interrupt_handler_common_stub
%endmacro

; Interrupt handlers (0-31)
INTERRUPT_HANDLER 0
INTERRUPT_HANDLER 1
INTERRUPT_HANDLER 2
INTERRUPT_HANDLER 3
INTERRUPT_HANDLER 4
INTERRUPT_HANDLER 5
INTERRUPT_HANDLER 6
INTERRUPT_HANDLER 7
INTERRUPT_HANDLER_ERROR 8
INTERRUPT_HANDLER 9
INTERRUPT_HANDLER_ERROR 10
INTERRUPT_HANDLER_ERROR 11
INTERRUPT_HANDLER_ERROR 12
INTERRUPT_HANDLER_ERROR 13
INTERRUPT_HANDLER_ERROR 14
INTERRUPT_HANDLER 15
INTERRUPT_HANDLER 16
INTERRUPT_HANDLER_ERROR 17
INTERRUPT_HANDLER 18
INTERRUPT_HANDLER 19
INTERRUPT_HANDLER 20
INTERRUPT_HANDLER 21
INTERRUPT_HANDLER 22
INTERRUPT_HANDLER 23
INTERRUPT_HANDLER 24
INTERRUPT_HANDLER 25
INTERRUPT_HANDLER 26
INTERRUPT_HANDLER 27
INTERRUPT_HANDLER 28
INTERRUPT_HANDLER 29
INTERRUPT_HANDLER_ERROR 30
INTERRUPT_HANDLER 31

; IRQ handlers (32-47)
IRQ_HANDLER 32
IRQ_HANDLER 33
IRQ_HANDLER 34
IRQ_HANDLER 35
IRQ_HANDLER 36
IRQ_HANDLER 37
IRQ_HANDLER 38
IRQ_HANDLER 39
IRQ_HANDLER 40
IRQ_HANDLER 41
IRQ_HANDLER 42
IRQ_HANDLER 43
IRQ_HANDLER 44
IRQ_HANDLER 45
IRQ_HANDLER 46
IRQ_HANDLER 47

; Common interrupt handler stub
interrupt_handler_common_stub:
    ; Save all general purpose registers
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    
    ; Save segment registers
    push ds
    push es
    push fs
    push gs
    
    ; Set up kernel data segments
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Call C interrupt handler
    mov rdi, rsp    ; Pass stack pointer as context
    call interrupt_handler_common
    
    ; Restore segment registers
    pop gs
    pop fs
    pop es
    pop ds
    
    ; Restore all general purpose registers
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    
    ; Remove error code and interrupt number from stack
    add rsp, 16
    
    ; Return from interrupt
    iretq

; IDT loading function
global interrupts_load_idt
interrupts_load_idt:
    lidt [rdi]
    ret

; Interrupt enable/disable functions
global interrupts_enable
interrupts_enable:
    sti
    ret

global interrupts_disable
interrupts_disable:
    cli
    ret

; Halt function
global cpu_halt
cpu_halt:
    hlt
    ret

; NOP function
global cpu_nop
cpu_nop:
    nop
    ret
