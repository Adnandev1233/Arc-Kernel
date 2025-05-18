#!/bin/bash

echo "Installing GRUB and required tools..."

# Detect package manager
if command -v apt-get >/dev/null 2>&1; then
    # Debian/Ubuntu
    sudo apt-get update
    sudo apt-get install -y grub-pc-bin grub-common xorriso mtools
elif command -v dnf >/dev/null 2>&1; then
    # Fedora
    sudo dnf install -y grub2 grub2-tools xorriso mtools
elif command -v pacman >/dev/null 2>&1; then
    # Arch Linux
    sudo pacman -Sy grub xorriso mtools
else
    echo "Could not detect package manager. Please install manually:"
    echo "- grub"
    echo "- xorriso"
    echo "- mtools"
    exit 1
fi

echo "Checking if GRUB modules are present..."
if [ ! -d "/usr/lib/grub/i386-pc" ]; then
    echo "GRUB modules not found in /usr/lib/grub/i386-pc"
    echo "Installing GRUB modules..."
    if command -v apt-get >/dev/null 2>&1; then
        sudo apt-get install -y grub-pc-bin
    elif command -v dnf >/dev/null 2>&1; then
        sudo dnf install -y grub2-pc
    elif command -v pacman >/dev/null 2>&1; then
        sudo pacman -S grub
    fi
fi

echo "Setup complete! You can now try building your OS." 