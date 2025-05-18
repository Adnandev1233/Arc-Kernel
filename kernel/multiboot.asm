; Multiboot header for kernel
section .multiboot
align 4

; Multiboot header fields
MULTIBOOT_HEADER_MAGIC equ 0x1BADB002
MULTIBOOT_PAGE_ALIGN equ 1<<0
MULTIBOOT_MEMORY_INFO equ 1<<1
MULTIBOOT_FLAGS equ (MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO)
MULTIBOOT_CHECKSUM equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_FLAGS)

; Minimal Multiboot header structure
multiboot_header:
    dd MULTIBOOT_HEADER_MAGIC     ; Magic number
    dd MULTIBOOT_FLAGS            ; Flags
    dd MULTIBOOT_CHECKSUM         ; Checksum

section .text
global _start
extern kernel_main

_start:
    ; Don't modify eax/ebx as they contain multiboot info
    cli                         ; Disable interrupts
    cmp eax, MULTIBOOT_HEADER_MAGIC
    jne .invalid_magic
    
    ; Set up stack
    mov esp, stack_top
    
    ; Preserve multiboot info
    push ebx                    ; Multiboot info structure pointer
    push eax                    ; Magic number
    
    ; Call kernel
    call kernel_main
    
    ; If we return, halt
    cli
.halt:
    hlt
    jmp .halt

.invalid_magic:
    ; Print error message if possible
    mov dword [0xB8000], 0x4F524F45 ; "ER"
    mov dword [0xB8004], 0x4F3A4F52 ; "R:"
    mov dword [0xB8008], 0x4F204F20 ; "  "
    mov dword [0xB800C], 0x4F474F42 ; "BG"
    mov dword [0xB8010], 0x4F4D4F54 ; "TM"
    jmp .halt

section .bss
align 16
stack_bottom:
    resb 32768  ; 32 KB stack
stack_top:

section .note.GNU-stack noalloc noexec nowrite progbits 