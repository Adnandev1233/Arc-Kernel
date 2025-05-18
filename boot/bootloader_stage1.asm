[BITS 16]
[ORG 0x7C00]

; Constants
STAGE2_SEGMENT equ 0x1000
STAGE2_OFFSET  equ 0x0000
STAGE2_SECTORS equ 8      ; Number of sectors for stage 2
KERNEL_SEGMENT equ 0x1000 ; Changed to align with multiboot spec
KERNEL_OFFSET  equ 0x0000
KERNEL_SECTORS equ 64     ; Size of kernel in sectors
MAX_RETRIES    equ 3      ; Maximum number of disk reset attempts

start:
    ; Initialize segment registers
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00    ; Set up stack just below bootloader

    ; Save boot drive number
    mov [boot_drive], dl

    ; Print boot message
    mov si, msg_boot
    call print_string

    ; Initialize retry counter
    mov byte [retry_count], MAX_RETRIES

reset_disk:
    ; Reset disk system
    xor ax, ax        ; AH = 0 (reset disk)
    mov dl, [boot_drive]
    int 0x13
    jnc check_disk_status  ; If successful, check disk status
    
    ; Retry logic for reset
    dec byte [retry_count]
    jz disk_error         ; If retries exhausted, report error
    mov si, msg_retry
    call print_string
    jmp reset_disk

check_disk_status:
    mov ah, 0x01        ; Get disk status
    mov dl, [boot_drive]
    int 0x13
    test ah, ah         ; Check if status is 0 (no error)
    jnz disk_error

load_stage2:
    mov si, msg_loading_stage2
    call print_string

    ; Load Stage 2 with proper alignment
    mov ax, STAGE2_SEGMENT
    mov es, ax
    xor bx, bx      ; ES:BX = STAGE2_SEGMENT:0

    ; Read sectors
    mov ah, 0x02    ; BIOS read sectors
    mov al, STAGE2_SECTORS
    mov ch, 0       ; Cylinder 0
    mov cl, 2       ; Start from sector 2
    mov dh, 0       ; Head 0
    mov dl, [boot_drive]
    int 0x13
    jc disk_error   ; If carry set, error occurred

    ; Verify sectors read
    cmp al, STAGE2_SECTORS
    jne disk_error

    ; Print success message
    mov si, msg_stage2_loaded
    call print_string

    ; Jump to stage 2
    jmp STAGE2_SEGMENT:STAGE2_OFFSET

disk_error:
    mov si, msg_disk_error
    call print_string
    
    ; Print specific error code
    mov al, ah          ; Error code is in AH
    call print_hex      ; New function to print hex value
    
    jmp $           ; Infinite loop

; Function: print_hex
; Input: AL = value to print
print_hex:
    push ax
    push cx
    mov ah, 0x0E
    mov cl, al
    shr al, 4
    call .print_digit
    mov al, cl
    and al, 0x0F
    call .print_digit
    pop cx
    pop ax
    ret
.print_digit:
    cmp al, 10
    jb .is_digit
    add al, 'A' - 10
    jmp .print_it
.is_digit:
    add al, '0'
.print_it:
    int 0x10
    ret

; Function: print_string
; Input: SI = pointer to string
print_string:
    push ax
    mov ah, 0x0E    ; BIOS teletype
.loop:
    lodsb           ; Load byte from SI into AL
    test al, al     ; Check if character is null
    jz .done        ; If null, we're done
    int 0x10        ; Print character
    jmp .loop
.done:
    pop ax
    ret

; Data
msg_boot db 'ArcOS Bootloader v1.0', 13, 10, 0
msg_loading_stage2 db 'Loading Stage 2...', 13, 10, 0
msg_stage2_loaded db 'Stage 2 loaded successfully!', 13, 10, 0
msg_disk_error db 'Disk error! Code: ', 0
msg_retry db 'Retrying disk reset...', 13, 10, 0
boot_drive db 0
retry_count db MAX_RETRIES

; Padding and boot signature
times 510-($-$$) db 0
dw 0xAA55 