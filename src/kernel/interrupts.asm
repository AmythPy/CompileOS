; CompileOS Interrupt Handler Stubs
; These are the low-level assembly stubs that get called when interrupts fire

[BITS 32]

; External C function
extern interrupt_handler_common

; Macro for ISRs that don't push error codes
%macro ISR_NOERRCODE 1
global isr%1
isr%1:
    cli
    push dword 0        ; Push dummy error code
    push dword %1       ; Push interrupt number
    jmp isr_common_stub
%endmacro

; Macro for ISRs that push error codes
%macro ISR_ERRCODE 1
global isr%1
isr%1:
    cli
    ; Error code already pushed by CPU
    push dword %1       ; Push interrupt number
    jmp isr_common_stub
%endmacro

; Macro for IRQs (hardware interrupts)
%macro IRQ 2
global irq%1
irq%1:
    cli
    push dword 0        ; Dummy error code
    push dword %2       ; Push interrupt number (32 + IRQ number)
    jmp isr_common_stub
%endmacro

; Exception handlers (0-31)
ISR_NOERRCODE 0     ; Divide by zero
ISR_NOERRCODE 1     ; Debug
ISR_NOERRCODE 2     ; NMI
ISR_NOERRCODE 3     ; Breakpoint
ISR_NOERRCODE 6     ; Invalid opcode
ISR_ERRCODE   13    ; General protection fault
ISR_ERRCODE   14    ; Page fault

; IRQ handlers (32-47)
IRQ 0, 32           ; Timer
IRQ 1, 33           ; Keyboard
IRQ 12, 44          ; Mouse

; Syscall handler (INT 0x80)
ISR_NOERRCODE 128

; Common ISR stub - saves state and calls C handler
isr_common_stub:
    ; Save all registers
    pusha               ; Pushes edi, esi, ebp, esp, ebx, edx, ecx, eax
    
    ; Save segment registers
    push ds
    push es
    push fs
    push gs
    
    ; Load kernel data segment
    mov ax, 0x10        ; Kernel data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Call C handler
    ; Arguments already on stack: int_no, err_code
    call interrupt_handler_common
    
    ; Restore segment registers
    pop gs
    pop fs
    pop es
    pop ds
    
    ; Restore all registers
    popa
    
    ; Clean up error code and interrupt number
    add esp, 8
    
    ; Return from interrupt
    sti
    iret
