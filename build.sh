#!/bin/bash

# Create build directory if it doesn't exist
mkdir -p build

# Compiler and flags
CC=gcc
AS=nasm
CFLAGS="-m32 -ffreestanding -Wall -Wextra -I./kernel/include"
LDFLAGS="-m32 -ffreestanding -nostdlib -lgcc"
ASFLAGS="-f elf32"

# Compile assembly files
echo "Compiling assembly files..."
$AS $ASFLAGS kernel/boot.asm -o build/boot.o
$AS $ASFLAGS kernel/interrupt.asm -o build/interrupt_asm.o

# Compile C files
echo "Compiling C files..."
$CC $CFLAGS -c kernel/kernel.c -o build/kernel.o
$CC $CFLAGS -c kernel/interrupt_handlers.c -o build/interrupt_handlers.o
$CC $CFLAGS -c kernel/terminal.c -o build/terminal.o
$CC $CFLAGS -c kernel/keyboard.c -o build/keyboard.o
$CC $CFLAGS -c kernel/installer.c -o build/installer.o
$CC $CFLAGS -c kernel/disk.c -o build/disk.o

# Link everything together
echo "Linking..."
$CC $LDFLAGS -T kernel/linker.ld -o build/kernel.bin \
    build/boot.o \
    build/interrupt_asm.o \
    build/kernel.o \
    build/interrupt_handlers.o \
    build/terminal.o \
    build/keyboard.o \
    build/installer.o \
    build/disk.o

# Create ISO image
echo "Creating ISO image..."
mkdir -p build/iso/boot/grub
cp build/kernel.bin build/iso/boot/
cp grub.cfg build/iso/boot/grub/
grub-mkrescue -o build/os.iso build/iso

echo "Build complete!" 