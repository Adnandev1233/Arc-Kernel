section .text
global setup_idt_entry

; void setup_idt_entry(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags)
setup_idt_entry:
    push ebp
    mov ebp, esp
    mov eax, [ebp + 8]   ; num
    mov edx, [ebp + 12]  ; base
    mov cx, [ebp + 16]   ; selector
    mov bl, [ebp + 20]   ; flags
    
    ; Calculate IDT entry address
    shl eax, 3           ; multiply by 8 (size of IDT entry)
    add eax, [idt_base]  ; add IDT base address
    
    ; Set up IDT entry
    mov word [eax], dx        ; base low
    mov word [eax + 2], cx    ; selector
    mov byte [eax + 4], 0     ; zero
    mov byte [eax + 5], bl    ; flags
    shr edx, 16
    mov word [eax + 6], dx    ; base high
    
    pop ebp
    ret

section .data
idt_base: dd 0          ; Will be set by C code

section .note.GNU-stack noalloc noexec nowrite progbits 