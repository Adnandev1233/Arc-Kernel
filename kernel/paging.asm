[BITS 32]
global enable_paging

section .text
enable_paging:
    push ebp
    mov ebp, esp
    
    ; Get page directory address
    mov eax, [ebp + 8]
    
    ; Set CR3 to page directory
    mov cr3, eax
    
    ; Enable paging
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
    
    pop ebp
    ret

align 4
page_fault:
    ; Simple page fault handler
    cli
    hlt
    jmp page_fault
 