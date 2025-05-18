[BITS 32]
global enable_paging

section .text
enable_paging:
    ; Save registers
    push ebp
    mov ebp, esp
    push ebx
    push ecx
    push edx
    
    ; Load page directory address from parameter
    mov eax, [ebp + 8]
    
    ; Disable interrupts
    cli
    
    ; Save current CR0
    mov ebx, cr0
    
    ; Disable paging and write protect
    and ebx, 0x7fffffff    ; Clear PG bit
    mov cr0, ebx
    
    ; Invalidate TLB by reloading CR3
    mov cr3, eax
    
    ; Enable paging
    or ebx, 0x80000000     ; Set PG bit
    mov cr0, ebx
    
    ; Restore registers
    pop edx
    pop ecx
    pop ebx
    pop ebp
    
    ; Return
    ret

align 4
page_fault:
    ; Simple page fault handler
    cli
    hlt
    jmp page_fault
 