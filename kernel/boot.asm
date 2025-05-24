[BITS 16]
[ORG 0x7C00]

; GDT
gdt_start:
    ; Null descriptor
    dd 0x0
    dd 0x0

    ; Code segment descriptor
    dw 0xFFFF    ; Limit (bits 0-15)
    dw 0x0000    ; Base (bits 0-15)
    db 0x00      ; Base (bits 16-23)
    db 10011010b ; Access byte
    db 11001111b ; Flags and Limit (bits 16-19)
    db 0x00      ; Base (bits 24-31)

    ; Data segment descriptor
    dw 0xFFFF    ; Limit (bits 0-15)
    dw 0x0000    ; Base (bits 0-15)
    db 0x00      ; Base (bits 16-23)
    db 10010010b ; Access byte
    db 11001111b ; Flags and Limit (bits 16-19)
    db 0x00      ; Base (bits 24-31)

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; Size of GDT
    dd gdt_start                ; Address of GDT

; Constants
CODE_SEG equ 0x08
DATA_SEG equ 0x10

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    sti

    ; Save boot drive number
    mov [boot_drive], dl

    ; Print boot message
    mov si, boot_msg
    call print_string

    ; Print drive number for debugging
    mov si, drive_msg
    call print_string
    mov al, [boot_drive]
    add al, '0'
    mov ah, 0x0E
    int 0x10
    mov al, 13
    int 0x10
    mov al, 10
    int 0x10

    ; Reset disk system
    mov ah, 0x00
    mov dl, [boot_drive]
    int 0x13
    jc disk_error

    ; Print loading message
    mov si, loading_msg
    call print_string

    ; Try to read kernel with retries
    mov cx, 3           ; Number of retries
read_retry:
    push cx            ; Save retry count
    
    ; Print retry count
    mov si, retry_count_msg
    call print_string
    mov al, cl
    add al, '0'
    mov ah, 0x0E
    int 0x10
    mov al, 13
    int 0x10
    mov al, 10
    int 0x10
    
    ; Load kernel (20 sectors) to 0x1000
    mov ah, 0x02       ; BIOS read sector function
    mov al, 20         ; Number of sectors to read
    mov ch, 0          ; Cylinder 0
    mov cl, 1          ; Start from sector 1 (after bootloader)
    mov dh, 0          ; Head 0
    mov dl, [boot_drive]; Drive number
    mov bx, 0x1000     ; Load address
    int 0x13
    jnc read_success   ; If no error, continue
    
    ; Print error message
    mov si, disk_error_msg
    call print_string
    
    ; Print error code
    mov al, ah
    add al, '0'
    mov ah, 0x0E
    int 0x10
    mov al, 13
    int 0x10
    mov al, 10
    int 0x10
    
    ; Reset disk system
    mov ah, 0x00
    mov dl, [boot_drive]
    int 0x13
    
    pop cx             ; Restore retry count
    loop read_retry    ; Try again if retries left
    
    ; If we get here, all retries failed
    jmp disk_error

read_success:
    pop cx             ; Clean up stack
    
    ; Print success message
    mov si, success_msg
    call print_string

    ; Enable A20 line
    call enable_a20

    ; Load GDT
    lgdt [gdt_descriptor]

    ; Switch to protected mode
    cli
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ; Far jump to 32-bit code
    jmp CODE_SEG:protected_mode

[BITS 32]
protected_mode:
    ; Set up segment registers
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Set up stack
    mov esp, 0x90000

    ; Jump to kernel
    jmp 0x1000

[BITS 16]
enable_a20:
    ; Try BIOS method first
    mov ax, 0x2401
    int 0x15
    ret

print_string:
    mov ah, 0x0E
.next:
    lodsb
    test al, al
    jz .done
    int 0x10
    jmp .next
.done:
    ret

disk_error:
    mov si, disk_error_msg
    call print_string
    mov al, ah
    add al, '0'
    mov ah, 0x0E
    int 0x10
    mov al, 13
    int 0x10
    mov al, 10
    int 0x10
    jmp $

boot_msg: db 'Loading ArcOS...', 13, 10, 0
drive_msg: db 'Boot drive: ', 0
loading_msg: db 'Reading kernel from disk...', 13, 10, 0
success_msg: db 'Kernel loaded successfully!', 13, 10, 0
disk_error_msg: db 'Disk read error! Code: ', 0
retry_count_msg: db 'Retry attempt: ', 0
boot_drive: db 0

times 510-($-$$) db 0
dw 0xAA55 