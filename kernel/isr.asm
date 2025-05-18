; Interrupt Service Routines
global isr_stub_table
global load_idt

extern isr_handler    ; Defined in C

section .text

; Macro to create ISR stub
%macro ISR_NOERRCODE 1
isr_stub_%1:
    push dword 0     ; Push dummy error code
    push dword %1    ; Push interrupt number
    jmp isr_common
%endmacro

; Macro for ISR with error code
%macro ISR_ERRCODE 1
isr_stub_%1:
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
ISR_NOERRCODE 16
ISR_ERRCODE   17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_ERRCODE   30
ISR_NOERRCODE 31

; Fill remaining ISRs (32-255) with a default handler
%assign i 32
%rep 224
ISR_NOERRCODE i
%assign i i+1
%endrep

; Load IDT
load_idt:
    push ebp
    mov ebp, esp
    mov eax, [ebp + 8]  ; Get pointer to IDT
    lidt [eax]          ; Load IDT
    pop ebp
    ret

; ISR stub table
section .data
isr_stub_table:
%assign i 0
%rep 256
    dd isr_stub_%+i     ; Add address of each ISR stub
%assign i i+1
%endrep

section .note.GNU-stack noalloc noexec nowrite progbits 