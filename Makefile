# Compiler and flags
CC = gcc
AS = nasm
LD = ld
CFLAGS = -m32 -ffreestanding -fno-pie -fno-stack-protector -fno-pic -fno-builtin -Wall -Wextra -Werror -std=gnu99 -I./kernel/include
ASFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T kernel/linker.ld -nostdlib

# Host compiler flags (for tools)
HOST_CFLAGS = -Wall -Wextra -std=gnu99

# Directories
KERNEL_DIR = kernel
BUILD_DIR = build
CONFIG_DIR = config
ISO_DIR = $(BUILD_DIR)/iso
BOOT_DIR = $(ISO_DIR)/boot/grub

# Source files
KERNEL_SRC = $(filter-out $(KERNEL_DIR)/menuconfig.c,$(wildcard $(KERNEL_DIR)/*.c))
KERNEL_ASM = $(filter-out $(KERNEL_DIR)/boot.asm,$(wildcard $(KERNEL_DIR)/*.asm))
KERNEL_OBJ = $(KERNEL_SRC:.c=.o) $(KERNEL_ASM:.asm=.o)

# Default target
all: $(BUILD_DIR)/os.iso

# Create build directories
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(ISO_DIR):
	mkdir -p $(ISO_DIR)

$(BOOT_DIR):
	mkdir -p $(BOOT_DIR)

# Configuration
menuconfig: $(BUILD_DIR)/menuconfig
	@if [ ! -d $(CONFIG_DIR) ]; then \
		mkdir -p $(CONFIG_DIR); \
	fi
	@cd $(CONFIG_DIR) && ../$(BUILD_DIR)/menuconfig

$(BUILD_DIR)/menuconfig: kernel/menuconfig.c | $(BUILD_DIR)
	$(CC) $(HOST_CFLAGS) -o $@ $< -lncurses

# Compile kernel
$(BUILD_DIR)/kernel.bin: $(KERNEL_OBJ) | $(BUILD_DIR)
	$(LD) $(LDFLAGS) -o $@ $(sort $(KERNEL_OBJ))

# Create ISO image
$(BUILD_DIR)/os.iso: $(BUILD_DIR)/kernel.bin kernel/grub.cfg | $(ISO_DIR) $(BOOT_DIR)
	@echo "Creating ISO directory structure..."
	@mkdir -p $(BOOT_DIR)
	@cp $(BUILD_DIR)/kernel.bin $(ISO_DIR)/boot/
	@cp kernel/grub.cfg $(BOOT_DIR)/
	@echo "Creating bootable ISO..."
	@grub-mkrescue -o $@ $(ISO_DIR) --compress=xz

# Compile C files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile assembly files
%.o: %.asm
	$(AS) $(ASFLAGS) $< -o $@

# Run the kernel
run: $(BUILD_DIR)/os.iso
	qemu-system-i386 -cdrom $(BUILD_DIR)/os.iso -boot d -m 512M -serial stdio

# Clean build files
clean:
	rm -f $(KERNEL_OBJ) $(BUILD_DIR)/kernel.bin $(BUILD_DIR)/os.iso $(BUILD_DIR)/menuconfig
	rm -rf $(ISO_DIR)

.PHONY: all menuconfig run clean distclean 
