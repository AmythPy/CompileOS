; CompileOS 32-bit Multiboot Bootloader
; Proper GRUB-compatible multiboot header for i386

; Multiboot header constants
MAGIC       equ 0x1BADB002
FLAGS       equ 0x00000003  ; multiboot flags: page alignment + memory info
CHECKSUM    equ -(MAGIC + FLAGS)

; Multiboot header - MUST be first in the binary!
section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

; Stack allocation (32-bit)
section .bootstrap_stack
stack_bottom:
    resb 16384  ; 16 KiB stack
stack_top:

; Entry point
section .text
global _start
extern kernel_main
_start:
    ; Set up 32-bit stack
    mov esp, stack_top

    ; Call kernel main function
    call kernel_main

    ; Halt if kernel returns
    cli
    hlt
hang:
    jmp hang

; Add GNU stack note to avoid warning
section .note.GNU-stack noalloc noexec nowrite progbits
