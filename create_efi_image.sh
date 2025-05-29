#!/bin/bash

# Create a temporary directory
TEMP_DIR=$(mktemp -d)

# Create EFI directory structure
mkdir -p $TEMP_DIR/EFI/BOOT

# Copy EFI bootloader
cp boot/BOOTX64.EFI $TEMP_DIR/EFI/BOOT/

# Copy kernel
cp kernel/kernel.bin $TEMP_DIR/

# Create FAT32 filesystem
dd if=/dev/zero of=efi.img bs=1M count=100
mkfs.fat -F 32 efi.img

# Create mount point
MOUNT_POINT=$(mktemp -d)

# Mount the image
sudo mount -o loop efi.img $MOUNT_POINT

# Copy files
sudo cp -r $TEMP_DIR/* $MOUNT_POINT/

# Unmount
sudo umount $MOUNT_POINT

# Clean up
rm -rf $TEMP_DIR $MOUNT_POINT

echo "EFI image created as efi.img" 