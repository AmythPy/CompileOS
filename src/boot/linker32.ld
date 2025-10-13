/**
 * CompileOS 32-bit Linker Script
 * Simple linker script for i386 kernel
 */

OUTPUT_FORMAT(elf32-i386)
ENTRY(kernel_main)

SECTIONS
{
    . = 0x100000;  /* Kernel starts at 1MB */

    .text : ALIGN(4K)
    {
        *(.multiboot)
        *(.text*)
        *(.rodata*)
    }

    .data : ALIGN(4K)
    {
        *(.data*)
    }

    .bss : ALIGN(4K)
    {
        *(.bss*)
        *(COMMON)
    }

    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame)
        *(.note*)
    }
}
