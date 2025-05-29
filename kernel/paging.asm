[BITS 32]

section .text
align 4
page_fault:
    ; Simple page fault handler
    cli
    hlt
    jmp page_fault
 