; Multiboot header for kernel
section .multiboot
align 4

; Multiboot header fields
MULTIBOOT_HEADER_MAGIC equ 0x1BADB002
MULTIBOOT_PAGE_ALIGN equ 1<<0
MULTIBOOT_MEMORY_INFO equ 1<<1
MULTIBOOT_FLAGS equ (MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO)
MULTIBOOT_CHECKSUM equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_FLAGS)

; Multiboot header structure
multiboot_header:
    dd MULTIBOOT_HEADER_MAGIC     ; Magic number
    dd MULTIBOOT_FLAGS            ; Flags
    dd MULTIBOOT_CHECKSUM         ; Checksum
    dd 0                          ; Header address (0 = not used)
    dd 0                          ; Load address (0 = not used)
    dd 0                          ; Load end address (0 = not used)
    dd 0                          ; BSS end address (0 = not used)
    dd 0                          ; Entry address (0 = not used)
    dd 0                          ; Mode type (0 = not used)
    dd 0                          ; Width (0 = not used)
    dd 0                          ; Height (0 = not used)
    dd 0                          ; Depth (0 = not used)

section .note.GNU-stack noalloc noexec nowrite progbits 