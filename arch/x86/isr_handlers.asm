[BITS 32]
section .text

; Export our ISR handlers
global isr0
global isr1
global isr2
global isr3
global isr4
global isr5
global isr6
global isr7
global isr8
global isr9
global isr10
global isr11
global isr12
global isr13
global isr14
global isr15

; External C function
extern isr_handler

; Common ISR stub that calls C handler
isr_common_stub:
    pusha                   ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax

    mov ax, ds             ; Save data segment
    push eax

    mov ax, 0x10           ; Load kernel data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp              ; Push pointer to stack as argument to handler

    call isr_handler      ; Call C handler

    add esp, 4            ; Clean up pushed stack pointer

    pop eax               ; Restore data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa                  ; Restore registers
    add esp, 8            ; Clean up error code and ISR number
    iret                  ; Return from interrupt

; CPU Exception handlers
isr0:
    cli                   ; Disable interrupts
    push byte 0          ; Push dummy error code
    push byte 0          ; Push interrupt number
    jmp isr_common_stub

isr1:
    cli
    push byte 0
    push byte 1
    jmp isr_common_stub

isr2:
    cli
    push byte 0
    push byte 2
    jmp isr_common_stub

isr3:
    cli
    push byte 0
    push byte 3
    jmp isr_common_stub

isr4:
    cli
    push byte 0
    push byte 4
    jmp isr_common_stub

isr5:
    cli
    push byte 0
    push byte 5
    jmp isr_common_stub

isr6:
    cli
    push byte 0
    push byte 6
    jmp isr_common_stub

isr7:
    cli
    push byte 0
    push byte 7
    jmp isr_common_stub

isr8:
    cli
    ; Error code already pushed
    push byte 8
    jmp isr_common_stub

isr9:
    cli
    push byte 0
    push byte 9
    jmp isr_common_stub

isr10:
    cli
    ; Error code already pushed
    push byte 10
    jmp isr_common_stub

isr11:
    cli
    ; Error code already pushed
    push byte 11
    jmp isr_common_stub

isr12:
    cli
    ; Error code already pushed
    push byte 12
    jmp isr_common_stub

isr13:
    cli
    ; Error code already pushed
    push byte 13
    jmp isr_common_stub

isr14:
    cli
    ; Error code already pushed
    push byte 14
    jmp isr_common_stub

isr15:
    cli
    push byte 0
    push byte 15
    jmp isr_common_stub

section .note.GNU-stack noalloc noexec nowrite progbits 