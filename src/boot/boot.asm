; CompileOS Bootloader - x86_64 Assembly
; This is a basic bootloader that loads the kernel

[BITS 16]
[ORG 0x7C00]

; Boot sector entry point
start:
    ; Set up segment registers
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    sti
    
    ; Clear screen
    mov ax, 0x0003
    int 0x10
    
    ; Display boot message
    mov si, boot_message
    call print_string
    
    ; Load kernel from disk
    call load_kernel
    
    ; Jump to kernel
    jmp 0x1000:0x0000

; Print string function
print_string:
    pusha
    mov ah, 0x0E
    mov bh, 0x00
    mov bl, 0x07
.loop:
    lodsb
    cmp al, 0
    je .done
    int 0x10
    jmp .loop
.done:
    popa
    ret

; Load kernel from disk
load_kernel:
    pusha
    
    ; Reset disk system
    mov ah, 0x00
    mov dl, 0x80
    int 0x13
    jc .error
    
    ; Load kernel sectors
    mov ah, 0x02
    mov al, 10          ; Number of sectors to read
    mov ch, 0           ; Cylinder
    mov cl, 2           ; Sector (starting from 2, since 1 is boot sector)
    mov dh, 0           ; Head
    mov dl, 0x80        ; Drive
    mov bx, 0x1000      ; Buffer address (segment)
    mov es, bx
    mov bx, 0x0000      ; Buffer address (offset)
    int 0x13
    jc .error
    
    popa
    ret
    
.error:
    mov si, error_message
    call print_string
    jmp halt

; Halt system
halt:
    cli
    hlt
    jmp halt

; Messages
boot_message db 'CompileOS Bootloader v0.1.0', 0x0D, 0x0A, 'Loading kernel...', 0x0D, 0x0A, 0
error_message db 'Error loading kernel!', 0x0D, 0x0A, 0

; Boot sector signature
times 510-($-$$) db 0
dw 0xAA55










