/* CompileOS 64-bit Kernel Linker Script - ELF with Multiboot Header */

OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)
ENTRY(_start)

SECTIONS
{
    /* Multiboot header MUST be in the first 8192 bytes for QEMU */
    . = 0x100000;  /* Load at 1MB */

    .multiboot :
    {
        *(.multiboot)
    }

    .bootstrap_stack :
    {
        _stack_bottom = .;
        *(.bootstrap_stack)
        _stack_top = .;
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
