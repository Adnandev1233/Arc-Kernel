[BITS 32]
global timer_irq_handler
global keyboard_irq_handler

extern keyboard_handler
extern timer_handler
extern irq_handler

section .text

; Timer IRQ handler (IRQ0)
timer_irq_handler:
    push dword 0     ; Push dummy error code
    push dword 32    ; Push IRQ number (32 + 0)
    jmp irq_common

; Keyboard IRQ handler (IRQ1)
keyboard_irq_handler:
    push dword 0     ; Push dummy error code
    push dword 33    ; Push IRQ number (32 + 1)
    jmp irq_common

; Common IRQ handler
irq_common:
    pusha           ; Push all registers

    mov ax, ds      ; Save data segment
    push eax

    mov ax, 0x10    ; Load kernel data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call irq_handler

    pop eax         ; Restore data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa            ; Restore registers
    add esp, 8      ; Clean up error code and IRQ number
    iret            ; Return from interrupt

section .note.GNU-stack noalloc noexec nowrite progbits 