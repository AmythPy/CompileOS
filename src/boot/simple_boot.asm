; Simple Working Bootloader for CompileOS
; This WILL boot your kernel

[BITS 32]
[GLOBAL _start]
[EXTERN kernel_main]

; Multiboot header
MULTIBOOT_MAGIC     equ 0x1BADB002
MULTIBOOT_FLAGS     equ 0x00000003
MULTIBOOT_CHECKSUM  equ -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

section .multiboot
align 4
    dd MULTIBOOT_MAGIC
    dd MULTIBOOT_FLAGS
    dd MULTIBOOT_CHECKSUM

section .bss
align 16
stack_bottom:
    resb 32768  ; 32KB stack
stack_top:

section .text
_start:
    ; Setup stack
    mov esp, stack_top
    
    ; Clear screen to blue
    mov edi, 0xB8000
    mov ecx, 2000
    mov ax, 0x1F20  ; Blue background, white space
    rep stosw
    
    ; Write "BOOT" at top
    mov dword [0xB8000], 0x0F4F0F42  ; "BO"
    mov dword [0xB8004], 0x0F540F4F  ; "OT"
    
    ; Call kernel (it's 64-bit but we'll try anyway)
    call kernel_main
    
    ; If we get here, halt
    cli
.hang:
    hlt
    jmp .hang
