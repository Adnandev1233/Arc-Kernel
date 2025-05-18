; I/O port functions
global port_out_byte
global port_in_byte
global io_wait

section .text

; void port_out_byte(uint16_t port, uint8_t data)
port_out_byte:
    push ebp
    mov ebp, esp
    mov dx, [ebp + 8]    ; port
    mov al, [ebp + 12]   ; data
    out dx, al
    pop ebp
    ret

; uint8_t port_in_byte(uint16_t port)
port_in_byte:
    push ebp
    mov ebp, esp
    mov dx, [ebp + 8]    ; port
    in al, dx
    pop ebp
    ret

; Small delay for I/O operations
io_wait:
    out 0x80, al
    ret

section .note.GNU-stack noalloc noexec nowrite progbits 