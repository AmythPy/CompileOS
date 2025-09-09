; CompileOS Embedded Bootloader - x86_64 Assembly
; Boots from a splash screen BMP with embedded OS

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
    
    ; Load embedded OS from splash screen
    call load_embedded_os
    
    ; Jump to embedded OS
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

; Load embedded OS from splash screen
load_embedded_os:
    pusha
    
    ; Reset disk system
    mov ah, 0x00
    mov dl, 0x80
    int 0x13
    jc .error
    
    ; Load splash screen (first few sectors to get BMP header)
    mov ah, 0x02
    mov al, 2          ; Load 2 sectors to get BMP headers
    mov ch, 0          ; Cylinder
    mov cl, 2          ; Sector (starting from 2, since 1 is boot sector)
    mov dh, 0          ; Head
    mov dl, 0x80       ; Drive
    mov bx, 0x1000     ; Buffer address (segment)
    mov es, bx
    mov bx, 0x0000     ; Buffer address (offset)
    int 0x13
    jc .error
    
    ; Parse BMP header to find embedded OS
    call parse_bmp_header
    
    ; Load the embedded OS
    call load_os_data
    
    popa
    ret
    
.error:
    mov si, error_message
    call print_string
    jmp halt

; Parse BMP header to find embedded OS
parse_bmp_header:
    pusha
    
    ; BMP header is at 0x1000:0x0000
    mov ax, 0x1000
    mov es, ax
    mov di, 0x0000
    
    ; Check BMP signature
    cmp word [es:di], 0x4D42  ; "BM"
    jne .invalid_bmp
    
    ; Get file size
    mov ax, [es:di + 2]       ; File size (low word)
    mov dx, [es:di + 4]       ; File size (high word)
    
    ; Calculate sectors needed
    mov cx, 512
    div cx
    inc ax  ; Round up
    
    ; Store sector count
    mov [sectors_to_load], ax
    
    ; Get data offset
    mov ax, [es:di + 10]      ; Data offset
    mov [bmp_data_offset], ax
    
    ; Parse BMP info header
    add di, 14
    mov ax, [es:di + 4]       ; Image width
    mov [bmp_width], ax
    mov ax, [es:di + 8]       ; Image height
    mov [bmp_height], ax
    mov ax, [es:di + 14]      ; Bits per pixel
    mov [bmp_bpp], ax
    
    ; Calculate image data size
    mov ax, [bmp_width]
    mul word [bmp_height]
    mov bx, [bmp_bpp]
    shr bx, 3  ; Convert bits to bytes
    mul bx
    mov [bmp_image_size], ax
    
    ; Calculate OS offset
    mov ax, [bmp_data_offset]
    add ax, [bmp_image_size]
    mov [os_offset], ax
    
    popa
    ret
    
.invalid_bmp:
    mov si, invalid_bmp_message
    call print_string
    jmp halt

; Load embedded OS data
load_os_data:
    pusha
    
    ; Calculate starting sector for OS data
    mov ax, [os_offset]
    mov bx, 512
    div bx
    add al, 2  ; Add 2 for boot sector and first data sector
    mov [os_start_sector], al
    
    ; Calculate sectors to load for OS
    mov ax, [sectors_to_load]
    sub ax, [os_start_sector]
    mov [os_sectors], ax
    
    ; Load OS data
    mov ah, 0x02
    mov al, [os_sectors]
    mov ch, 0
    mov cl, [os_start_sector]
    mov dh, 0
    mov dl, 0x80
    mov bx, 0x2000  ; Load OS to 0x2000:0x0000
    mov es, bx
    mov bx, 0x0000
    int 0x13
    jc .error
    
    ; Verify OS magic
    call verify_os_magic
    
    popa
    ret
    
.error:
    mov si, os_load_error_message
    call print_string
    jmp halt

; Verify OS magic number
verify_os_magic:
    pusha
    
    mov ax, 0x2000
    mov es, ax
    mov di, 0x0000
    
    ; Check for OS embed header
    cmp dword [es:di], 0x31534F43  ; "COS1"
    jne .invalid_os
    
    ; Get OS size
    mov ax, [es:di + 4]
    mov [os_size], ax
    
    ; Get OS checksum
    mov ax, [es:di + 12]
    mov [os_checksum], ax
    
    ; Get OS version
    mov si, di
    add si, 16
    mov di, os_version
    mov cx, 16
    rep movsb
    
    popa
    ret
    
.invalid_os:
    mov si, invalid_os_message
    call print_string
    jmp halt

; Halt system
halt:
    cli
    hlt
    jmp halt

; Data section
boot_message db 'CompileOS Embedded Bootloader v0.1.0', 0x0D, 0x0A, 'Loading embedded OS...', 0x0D, 0x0A, 0
error_message db 'Error loading embedded OS!', 0x0D, 0x0A, 0
invalid_bmp_message db 'Invalid BMP file!', 0x0D, 0x0A, 0
invalid_os_message db 'Invalid embedded OS!', 0x0D, 0x0A, 0
os_load_error_message db 'Error loading OS data!', 0x0D, 0x0A, 0

; Variables
sectors_to_load dw 0
bmp_data_offset dw 0
bmp_width dw 0
bmp_height dw 0
bmp_bpp dw 0
bmp_image_size dw 0
os_offset dw 0
os_start_sector db 0
os_sectors db 0
os_size dw 0
os_checksum dw 0
os_version times 16 db 0

; Boot sector signature
times 510-($-$$) db 0
dw 0xAA55
