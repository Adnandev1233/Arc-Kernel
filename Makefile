# Compiler and linker settings
CC = gcc
AS = nasm
LD = ld

# Directories
KERNEL_DIR = kernel
BOOT_DIR = boot
BUILD_DIR = build
ISO_DIR = iso
INCLUDE_DIR = include
LIB_DIR = lib

# Compiler flags
CFLAGS = -m32 -nostdlib -fno-builtin -fno-stack-protector \
         -nostartfiles -nodefaultlibs -Wall -Wextra -c \
         -I$(KERNEL_DIR) -I$(INCLUDE_DIR) -I$(KERNEL_DIR)/include \
         -ffreestanding -fno-pie -O2

# Assembler flags
ASFLAGS = -f elf32

# Linker flags
LDFLAGS = -m elf_i386 \
          -T linker.ld \
          --no-dynamic-linker \
          -nostdlib \
          -z max-page-size=0x1000 \
          -static

# Source files
KERNEL_C_SRCS = $(wildcard $(KERNEL_DIR)/*.c)
LIB_C_SRCS = $(wildcard $(LIB_DIR)/*.c)
KERNEL_ASM_SRCS = $(wildcard $(KERNEL_DIR)/*.asm)

# Object files
KERNEL_C_OBJS = $(KERNEL_C_SRCS:%.c=$(BUILD_DIR)/%.o)
LIB_C_OBJS = $(LIB_C_SRCS:%.c=$(BUILD_DIR)/%.o)
KERNEL_ASM_OBJS = $(KERNEL_ASM_SRCS:%.asm=$(BUILD_DIR)/%.o)

# Final kernel binary
KERNEL_BIN = $(BUILD_DIR)/kernel.bin

# ISO output
ISO_OUTPUT = $(BUILD_DIR)/arcos.iso

# Default target
all: directories $(ISO_OUTPUT)

# Create necessary directories
directories:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/$(KERNEL_DIR)
	mkdir -p $(BUILD_DIR)/$(LIB_DIR)
	mkdir -p $(ISO_DIR)/boot/grub

# Compile C source files
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@

# Compile assembly source files
$(BUILD_DIR)/%.o: %.asm
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

# Link kernel
$(KERNEL_BIN): $(KERNEL_C_OBJS) $(LIB_C_OBJS) $(KERNEL_ASM_OBJS)
	@echo "Linking kernel..."
	@echo "Objects: $(KERNEL_C_OBJS) $(LIB_C_OBJS) $(KERNEL_ASM_OBJS)"
	$(LD) $(LDFLAGS) -o $@ $^
	@echo "Kernel linked successfully!"

# Create GRUB config
$(ISO_DIR)/boot/grub/grub.cfg:
	echo 'set timeout=5' > $@
	echo 'set default=0' >> $@
	echo '' >> $@
	echo 'menuentry "ArcOS" {' >> $@
	echo '    insmod part_msdos' >> $@
	echo '    insmod ext2' >> $@
	echo '    insmod multiboot' >> $@
	echo '    multiboot /boot/kernel.bin' >> $@
	echo '    boot' >> $@
	echo '}' >> $@
	echo '' >> $@
	echo 'menuentry "ArcOS (Debug Mode)" {' >> $@
	echo '    insmod part_msdos' >> $@
	echo '    insmod ext2' >> $@
	echo '    insmod multiboot' >> $@
	echo '    multiboot /boot/kernel.bin debug=1' >> $@
	echo '    boot' >> $@
	echo '}' >> $@

# Create ISO
$(ISO_OUTPUT): $(KERNEL_BIN) $(ISO_DIR)/boot/grub/grub.cfg
	cp $(KERNEL_BIN) $(ISO_DIR)/boot/
	grub-mkrescue -o $(ISO_OUTPUT) $(ISO_DIR)

# Clean build files
clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(ISO_DIR)

# Run in QEMU
run: $(ISO_OUTPUT)
	qemu-system-i386 -cdrom $(ISO_OUTPUT)

# Run in QEMU with debug options
debug: $(ISO_OUTPUT)
	qemu-system-i386 -cdrom $(ISO_OUTPUT) -s -S

# Print variables for debugging
print-%:
	@echo '$*=$($*)'

.PHONY: all clean run debug directories print-% 