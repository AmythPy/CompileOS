/* CompileOS Kernel Linker Script */

ENTRY(kernel_main)

SECTIONS
{
    /* Kernel starts at 1MB */
    . = 0x100000;
    
    /* Kernel code section */
    .text : ALIGN(4K)
    {
        *(.text)
        *(.text.*)
    }
    
    /* Read-only data */
    .rodata : ALIGN(4K)
    {
        *(.rodata)
        *(.rodata.*)
    }
    
    /* Read-write data */
    .data : ALIGN(4K)
    {
        *(.data)
        *(.data.*)
    }
    
    /* Uninitialized data */
    .bss : ALIGN(4K)
    {
        *(COMMON)
        *(.bss)
        *(.bss.*)
    }
    
    /* Discard other sections */
    /DISCARD/ :
    {
        *(.comment)
        *(.note*)
    }
}
