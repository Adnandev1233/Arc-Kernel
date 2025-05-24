# Compiler and linker settings
AS = nasm
CC = gcc
LD = ld

# Colors for output
GREEN = \033[0;32m
BLUE = \033[0;34m
YELLOW = \033[0;33m
RED = \033[0;31m
NC = \033[0m

# Flags
ASFLAGS = -f elf32
CFLAGS = -m32 -ffreestanding -fno-pie -fno-stack-protector -nostdlib -c -I$(INCLUDE_DIR) -Wall -Wextra -masm=intel
LDFLAGS = -m elf_i386 -T kernel/linker.ld -nostdlib

# Directories
BOOT_DIR = boot
KERNEL_DIR = kernel
DRIVERS_DIR = kernel/drivers
DE_DIR = kernel/de
INCLUDE_DIR = kernel/include

# Source files
BOOT_SRC = $(BOOT_DIR)/boot.asm
KERNEL_SRC = $(KERNEL_DIR)/kernel.c
CPU_SRC = $(KERNEL_DIR)/cpu.c
MEMORY_SRC = $(KERNEL_DIR)/memory.c
STRING_SRC = $(KERNEL_DIR)/string.c
TERMINAL_SRC = $(KERNEL_DIR)/terminal.c
DRIVERS_SRC = $(wildcard $(DRIVERS_DIR)/*.c)
DE_SRC = $(DE_DIR)/desktop.c $(DE_DIR)/dock.c $(DE_DIR)/menu_bar.c $(DE_DIR)/window_manager.c

# Object files
BOOT_OBJ = $(BOOT_SRC:.asm=.o)
KERNEL_OBJ = $(KERNEL_SRC:.c=.o)
CPU_OBJ = $(CPU_SRC:.c=.o)
MEMORY_OBJ = $(MEMORY_SRC:.c=.o)
STRING_OBJ = $(STRING_SRC:.c=.o)
TERMINAL_OBJ = $(TERMINAL_SRC:.c=.o)
DRIVERS_OBJ = $(DRIVERS_SRC:.c=.o)
DE_OBJ = $(DE_SRC:.c=.o)

# Output files
KERNEL_BIN = kernel.bin
ISO_DIR = iso
ISO_BIN = os.iso

# Default target
all: $(ISO_BIN)
	@echo "$(GREEN)Build completed successfully!$(NC)"
	@echo "$(BLUE)"
	@echo "    _    ____   ____  _____  ____  "
	@echo "   / \  |  _ \ / ___||  ___|/ ___| "
	@echo "  / _ \ | |_) | |    | |_  \___ \ "
	@echo " / ___ \|  _ <| |___ |  _|  ___) |"
	@echo "/_/   \_\_| \_\\____||_|   |____/ "
	@echo "$(NC)"

# Build bootloader
$(BOOT_OBJ): $(BOOT_SRC)
	@echo "  AS      $<"
	@$(AS) $(ASFLAGS) -o $@ $<

# Build kernel
$(KERNEL_OBJ): $(KERNEL_SRC)
	@echo "  CC      $<"
	@$(CC) $(CFLAGS) -o $@ $<

# Build CPU
$(CPU_OBJ): $(CPU_SRC)
	@echo "  CC      $<"
	@$(CC) $(CFLAGS) -o $@ $<

# Build memory
$(MEMORY_OBJ): $(MEMORY_SRC)
	@echo "  CC      $<"
	@$(CC) $(CFLAGS) -o $@ $<

# Build string
$(STRING_OBJ): $(STRING_SRC)
	@echo "  CC      $<"
	@$(CC) $(CFLAGS) -o $@ $<

# Build terminal
$(TERMINAL_OBJ): $(TERMINAL_SRC)
	@echo "  CC      $<"
	@$(CC) $(CFLAGS) -o $@ $<

# Build drivers
$(DRIVERS_OBJ): $(DRIVERS_SRC)
	@echo "  CC      $<"
	@$(CC) $(CFLAGS) -o $@ $<

# Build desktop environment
$(DE_DIR)/%.o: $(DE_DIR)/%.c
	@echo "  CC      $<"
	@$(CC) $(CFLAGS) -o $@ $<

# Link kernel
$(KERNEL_BIN): $(BOOT_OBJ) $(KERNEL_OBJ) $(CPU_OBJ) $(MEMORY_OBJ) $(STRING_OBJ) $(TERMINAL_OBJ) $(DRIVERS_OBJ) $(DE_OBJ)
	@echo "  LD      $@"
	@$(LD) $(LDFLAGS) $^ -o $@

# Create ISO
$(ISO_BIN): $(KERNEL_BIN)
	@echo "  MKDIR   $(ISO_DIR)/boot/grub"
	@mkdir -p $(ISO_DIR)/boot/grub
	@echo "  CP      $(KERNEL_BIN) -> $(ISO_DIR)/boot/"
	@cp $(KERNEL_BIN) $(ISO_DIR)/boot/
	@echo "  CP      grub.cfg -> $(ISO_DIR)/boot/grub/"
	@cp grub.cfg $(ISO_DIR)/boot/grub/
	@echo "  GRUB    Creating bootable ISO"
	@grub-mkrescue -o $@ $(ISO_DIR)

# Clean
clean:
	@echo "  CLEAN   Removing build artifacts"
	@rm -f $(BOOT_OBJ) $(KERNEL_OBJ) $(CPU_OBJ) $(MEMORY_OBJ) $(STRING_OBJ) $(TERMINAL_OBJ) $(DRIVERS_OBJ) $(DE_OBJ) $(KERNEL_BIN) $(ISO_BIN)
	@rm -rf $(ISO_DIR)
	@echo "Clean completed successfully!"

.PHONY: all clean 