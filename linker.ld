/* Linker script for ArcOS kernel */
ENTRY(_start)
OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386)

SECTIONS
{
    . = 0x1000;    /* Kernel is loaded at 0x1000 by bootloader */

    .text : {
        *(.multiboot)  /* Multiboot header first */
        *(.text)       /* Code */
    }

    .rodata ALIGN(4K) : {
        *(.rodata)     /* Read-only data */
    }

    .data ALIGN(4K) : {
        *(.data)       /* Read-write data */
    }

    .bss ALIGN(4K) : {
        *(COMMON)      /* Common symbols */
        *(.bss)        /* Zero-initialized data */
    }

    /* End of kernel */
    end = .;

    /* Discard unused sections */
    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
        *(.note.gnu.build-id)
    }
} 