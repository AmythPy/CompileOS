; CompileOS Multiboot Header
; Simple and reliable bootloader entry

[BITS 32]

; Multiboot1 header (more compatible)
MULTIBOOT_MAGIC        equ 0x1BADB002
MULTIBOOT_FLAGS        equ 0x00000003  ; Page align + memory info
MULTIBOOT_CHECKSUM     equ -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

; Multiboot header - MUST be in first 8KB
section .multiboot
align 4
    dd MULTIBOOT_MAGIC
    dd MULTIBOOT_FLAGS
    dd MULTIBOOT_CHECKSUM

; Stack
section .bss
align 16
stack_bottom:
    resb 16384  ; 16 KB stack
stack_top:

; Page tables for long mode
align 4096
pml4:
    resb 4096
pdpt:
    resb 4096
pd:
    resb 4096

; Entry point (32-bit)
section .text
global _start
extern kernel_main

_start:
    ; Set up stack
    mov esp, stack_top
    
    ; Print "B" to show we booted
    mov dword [0xB8000], 0x0F420F42  ; "BB" in white on black
    
    ; Save multiboot info
    mov edi, eax    ; Multiboot magic
    mov esi, ebx    ; Multiboot info
    
    ; Print "C" to show we're checking CPU
    mov dword [0xB8004], 0x0F430F43  ; "CC" in white on black
    
    ; Check for long mode support
    call check_long_mode
    
    ; Print "P" to show paging setup
    mov dword [0xB8008], 0x0F500F50  ; "PP" in white on black
    
    ; Set up paging for long mode
    call setup_page_tables
    call enable_paging
    
    ; Load 64-bit GDT
    lgdt [gdt64.pointer]
    
    ; Jump to 64-bit code
    jmp gdt64.code:long_mode_start

check_long_mode:
    ; Check if CPUID is supported
    pushfd
    pop eax
    mov ecx, eax
    xor eax, 1 << 21
    push eax
    popfd
    pushfd
    pop eax
    push ecx
    popfd
    cmp eax, ecx
    je .no_long_mode
    
    ; Check for long mode
    mov eax, 0x80000000
    cpuid
    cmp eax, 0x80000001
    jb .no_long_mode
    
    mov eax, 0x80000001
    cpuid
    test edx, 1 << 29
    jz .no_long_mode
    ret

.no_long_mode:
    hlt
    jmp .no_long_mode

setup_page_tables:
    ; Map first 2MB identity-mapped
    mov eax, pdpt
    or eax, 0b11  ; present + writable
    mov [pml4], eax
    
    mov eax, pd
    or eax, 0b11
    mov [pdpt], eax
    
    ; Map first 2MB as huge page
    mov eax, 0x83  ; huge page + present + writable
    mov [pd], eax
    
    ret

enable_paging:
    ; Load PML4
    mov eax, pml4
    mov cr3, eax
    
    ; Enable PAE
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax
    
    ; Enable long mode
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr
    
    ; Enable paging
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax
    
    ret

; 64-bit GDT
section .rodata
gdt64:
    dq 0  ; null descriptor
.code: equ $ - gdt64
    dq (1<<43) | (1<<44) | (1<<47) | (1<<53)  ; code segment
.pointer:
    dw $ - gdt64 - 1
    dq gdt64

; 64-bit entry point
[BITS 64]
section .text
long_mode_start:
    ; Print "6" to show we're in 64-bit mode
    mov rax, 0x0F360F36  ; "66" in white
    mov [0xB800C], rax
    
    ; Clear segment registers
    mov ax, 0
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Print "K" before calling kernel
    mov rax, 0x0F4B0F4B  ; "KK" in white
    mov [0xB8010], rax
    
    ; Call kernel main
    call kernel_main
    
    ; Print "E" if kernel returns (error)
    mov rax, 0x0F450F45  ; "EE" in white
    mov [0xB8014], rax
    
    ; If kernel returns, halt
    cli
.hang:
    hlt
    jmp .hang
