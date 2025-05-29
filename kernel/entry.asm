[BITS 32]

; Stack setup
section .bss
align 16
stack_bottom:
    resb 16384 ; 16 KB stack
stack_top:

; Kernel entry point
section .text
global _start
extern kernel_main

; Multiboot header must be in the first 8KB of the kernel
align 4
    dd 0x1BADB002    ; Magic number
    dd 0x03          ; Flags: align modules on page boundaries and provide memory map
    dd -(0x1BADB002 + 0x03)  ; Checksum

_start:
    ; Set up stack
    mov esp, stack_top

    ; Check multiboot magic number
    cmp eax, 0x2BADB002
    jne .no_multiboot

    ; Print boot message
    mov esi, boot_msg
    call early_print

    ; Push multiboot info pointer
    push ebx

    ; Call kernel main
    call kernel_main

    ; If kernel_main returns, halt
.hang:
    cli
    hlt
    jmp .hang

.no_multiboot:
    mov esi, no_multiboot_msg
    call early_print
    jmp .hang

; Early debug output function
; Input: ESI = string pointer
early_print:
    push eax
    push ebx
    mov ebx, VGA_MEMORY
.loop:
    lodsb
    test al, al
    jz .done
    mov ah, VGA_COLOR_LIGHT_GREY
    mov [ebx], ax
    add ebx, 2
    jmp .loop
.done:
    pop ebx
    pop eax
    ret

; Error handler
error:
    mov esi, error_msg
    call early_print
.hang:
    cli
    hlt
    jmp .hang

; Data
boot_msg db 'ArcOS Kernel Starting...', 13, 10, 0
init_msg db 'Initializing kernel...', 13, 10, 0
halt_msg db 'Kernel halted.', 13, 10, 0
error_msg db 'CRITICAL ERROR: System halted.', 13, 10, 0
no_multiboot_msg db 'Error: Not loaded by multiboot-compliant bootloader', 13, 10, 0

; VGA text mode constants
VGA_WIDTH equ 80
VGA_HEIGHT equ 25
VGA_MEMORY equ 0xB8000
VGA_COLOR_LIGHT_GREY equ 0x07
VGA_COLOR_RED equ 0x04 