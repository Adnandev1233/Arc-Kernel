section .text
global port_in_byte
global port_out_byte

; uint8_t port_in_byte(uint16_t port)
port_in_byte:
    mov dx, [esp + 4]    ; Get the port number
    in al, dx            ; Read from port
    ret

; void port_out_byte(uint16_t port, uint8_t data)
port_out_byte:
    mov dx, [esp + 4]    ; Get the port number
    mov al, [esp + 8]    ; Get the data
    out dx, al           ; Write to port
    ret

section .note.GNU-stack noalloc noexec nowrite progbits 