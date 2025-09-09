BITS 32
global _start
align 4
multiboot_header:
    dd 0xe85250d6
    dd 0x0
    dd 0x100
    dd -(0xe85250d6 + 0x0 + 0x100)
BITS 64
_start:
    cli
.loop:
    hlt
    jmp .loop
