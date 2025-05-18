section .text
global enable_interrupts
global halt_cpu
global enable_paging

; void enable_interrupts(void)
enable_interrupts:
    sti
    ret

; void halt_cpu(void)
halt_cpu:
    hlt
    ret

; void enable_paging(uint32_t* directory)
enable_paging:
    push ebp
    mov ebp, esp
    
    ; Load page directory
    mov eax, [ebp + 8]    ; Get directory parameter
    mov cr3, eax          ; Load directory into CR3
    
    ; Enable paging (set PG bit in CR0)
    mov eax, cr0
    or eax, 0x80000000    ; Set bit 31 (PG bit)
    mov cr0, eax
    
    pop ebp
    ret

section .note.GNU-stack noalloc noexec nowrite progbits 