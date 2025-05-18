section .text
global keyboard_irq_handler
global timer_irq_handler
extern keyboard_handler
extern timer_handler
extern send_eoi

; Timer IRQ handler
timer_irq_handler:
    pusha                   ; Save all registers
    cld                     ; Clear direction flag
    call timer_handler      ; Call our C handler
    call send_eoi          ; Send End of Interrupt
    popa                   ; Restore all registers
    iret                   ; Return from interrupt

; Keyboard IRQ handler
keyboard_irq_handler:
    pusha                   ; Save all registers
    cld                     ; Clear direction flag
    call keyboard_handler   ; Call our C handler
    call send_eoi          ; Send End of Interrupt
    popa                   ; Restore all registers
    iret                   ; Return from interrupt

section .note.GNU-stack noalloc noexec nowrite progbits 