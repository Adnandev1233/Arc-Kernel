/* Linker script for kernel */
ENTRY(_start)
OUTPUT_FORMAT(elf32-i386)

SECTIONS
{
    /* Begin putting sections at 1MB, a conventional place for kernels to be
       loaded by the bootloader. */
    . = 0x100000;

    /* First put the text section, which contains the multiboot header */
    .text BLOCK(4K) : ALIGN(4K)
    {
        *(.text)       /* Code */
    }

    /* Read-only data. */
    .rodata BLOCK(4K) : ALIGN(4K)
    {
        *(.rodata)     /* Read-only data */
    }

    /* Read-write data (initialized) */
    .data BLOCK(4K) : ALIGN(4K)
    {
        *(.data)       /* Read-write data */
    }

    /* Read-write data (uninitialized) and stack */
    .bss BLOCK(4K) : ALIGN(4K)
    {
        *(COMMON)      /* Common symbols */
        *(.bss)        /* Zero-initialized data */
        *(.bootstrap_stack)  /* Stack for early boot */
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