; CompileOS Boot Assembly
; Multiboot 1 header - MUST be first 8192 bytes of file!

; Multiboot 1 header constants
MAGIC       equ 0x1BADB002
FLAGS       equ (1 << 0) | (1 << 1)  ; Page align and memory info
CHECKSUM    equ -(MAGIC + FLAGS)

; Multiboot header section - must be first in binary
section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

; Padding to ensure we're in the first 8192 bytes
times 4096 - ($ - $$) db 0

; Stack allocation
section .bss
align 16
stack_bottom:
    resb 16384
stack_top:

section .text
global _start
extern kernel_main

_start:
    ; Set up stack
    mov esp, stack_top

    ; Call kernel main function
    call kernel_main

    ; Hang if kernel returns
.hang:
    cli
    hlt
    jmp .hang
