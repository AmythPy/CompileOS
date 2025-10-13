/* CompileOS Kernel Linker Script - ELF with Multiboot Header */

OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386)
ENTRY(_start)

SECTIONS
{
    /* Multiboot header MUST be in the first 8192 bytes for QEMU */
    . = 0x1000;

    .multiboot :
    {
        *(.multiboot)
    }

    .bootstrap_stack :
    {
        . = ALIGN(16);
        *(.bootstrap_stack)
    }

    .text : ALIGN(4K)
    {
        *(.text)
        *(.text.*)
    }

    .rodata : ALIGN(4K)
    {
        *(.rodata)
        *(.rodata.*)
    }

    .data : ALIGN(4K)
    {
        *(.data)
        *(.data.*)
    }

    .bss : ALIGN(4K)
    {
        *(COMMON)
        *(.bss)
        *(.bss.*)
    }

    /DISCARD/ :
    {
        *(.comment)
        *(.note*)
    }
}










