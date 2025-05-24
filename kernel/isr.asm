; Interrupt Service Routines
[BITS 32]
global isr0, isr1, isr2, isr3, isr4, isr5, isr6, isr7
global isr8, isr9, isr10, isr11, isr12, isr13, isr14, isr15
global load_idt

extern isr_handler    ; Defined in C

section .text

; Macro to create ISR stub
%macro ISR_NOERRCODE 1
isr%1:
    push dword 0     ; Push dummy error code
    push dword %1    ; Push interrupt number
    jmp isr_common
%endmacro

; Macro for ISR with error code
%macro ISR_ERRCODE 1
isr%1:
    push dword %1    ; Push interrupt number
    jmp isr_common
%endmacro

; Common ISR handler
isr_common:
    pusha           ; Push all registers

    mov ax, ds      ; Save data segment
    push eax

    mov ax, 0x10    ; Load kernel data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call isr_handler

    pop eax         ; Restore data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa            ; Restore registers
    add esp, 8      ; Clean up error code and ISR number
    iret            ; Return from interrupt

; Create ISR stubs
ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE   8
ISR_NOERRCODE 9
ISR_ERRCODE   10
ISR_ERRCODE   11
ISR_ERRCODE   12
ISR_ERRCODE   13
ISR_ERRCODE   14
ISR_NOERRCODE 15

; Load IDT
load_idt:
    push ebp
    mov ebp, esp
    mov eax, [ebp + 4]  ; Get pointer to IDT
    lidt [eax]          ; Load IDT
    pop ebp
    ret

section .note.GNU-stack noalloc noexec nowrite progbits 