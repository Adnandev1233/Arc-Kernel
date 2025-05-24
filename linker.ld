/* Linker script for ArcOS kernel */
ENTRY(_start)
OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386)

SECTIONS
{
    . = 1M;    /* Kernel is loaded at 1MB */

    .text BLOCK(4K) : ALIGN(4K)
    {
        *(.multiboot)  /* Multiboot header must be first */
        *(.text)       /* Code */
    }

    .rodata BLOCK(4K) : ALIGN(4K)
    {
        *(.rodata)     /* Read-only data */
    }

    .data BLOCK(4K) : ALIGN(4K)
    {
        *(.data)       /* Read-write data */
    }

    .bss BLOCK(4K) : ALIGN(4K)
    {
        *(COMMON)      /* Common symbols */
        *(.bss)        /* Zero-initialized data */
        *(.bootstrap_stack)  /* Stack for early boot */
    }

    .note.GNU-stack : { *(.note.GNU-stack) }

    /* End of kernel */
    end = .;

    /* Discard unused sections */
    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
        *(.note.gnu.build-id)
    }
} 