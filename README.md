
# ArcOS v0.8k
-------------
# ArcOS

A 32-bit operating system written in C and assembly.

## Building

```bash
make menuconfig  # Configure build options
make            # Build the OS
make run        # Run in QEMU
```

## Features

- 32-bit protected mode
- VESA/VBE graphics support
- Basic GUI framework
- File system support
- Disk I/O
- Memory management
- Interrupt handling
- Keyboard and mouse input

## Requirements

- GCC (32-bit)
- NASM
- Make
- QEMU (for running)

## License

MIT License

## Prerequisites

You need the following tools installed:

- NASM (Netwide Assembler)
- GCC (GNU Compiler Collection)
- Make
- GRUB
- QEMU (for testing)
- xorriso (for creating the ISO)

### Windows Users
Install these tools through MSYS2:
```bash
pacman -S make
pacman -S gcc
pacman -S nasm
pacman -S grub
pacman -S qemu
pacman -S xorriso
```

### Linux Users
Install these tools through your package manager:
```bash
# Ubuntu/Debian
sudo apt-get install make gcc nasm grub-pc-bin xorriso qemu-system-x86

# Fedora
sudo dnf install make gcc nasm grub2 xorriso qemu-system-x86

# Arch Linux
sudo pacman -S make gcc nasm grub xorriso qemu
```

## Building

To build the operating system:

```bash
make clean  # Clean any previous
make menuconfig
make        # Build everything
```

This will:
1. Compile the bootloader stages
2. Compile the kernel
3. Create a disk image
4. Create a bootable ISO

## Running

To run the OS in QEMU:

```bash
make run
```
## Project Structure

- `bootloader_stage1.asm`: First stage bootloader (loads stage 2)
- `bootloader_stage2.asm`: Second stage bootloader (loads kernel and enters protected mode)
- `kernel.c` and related files: Kernel implementation
- `Makefile`: Build configuration
- `grub.cfg`: GRUB bootloader configuration

## Troubleshooting

1. If you get "command not found" errors, make sure all required tools are installed and in your PATH.
2. If the build fails with "Error: grub-mkrescue not found", install GRUB tools.
3. If QEMU fails to start, make sure QEMU is installed and in your PATH.

## Development

The bootloader is implemented in two stages:
1. Stage 1 (`bootloader_stage1.asm`): Loads from the first sector and bootstraps stage 2
2. Stage 2 (`bootloader_stage2.asm`): Enables A20 line, enters protected mode, and loads the kernel

The kernel is loaded at the specified memory location and takes control from there. 

## build-support

1.Linux(best option)
2.MacOS(second best option)
3.Windows(hard but possible)

## Known Issues

### Multiboot Error (0x1000)
The kernel is currently experiencing a multiboot error with code 0x1000. This error occurs during the boot process and indicates that the kernel is not being properly recognized as a multiboot-compliant kernel. The error appears in white text on the screen.

Current investigation points to potential issues with:
1. Multiboot header placement
2. Kernel loading address
3. GRUB configuration

Work in progress to resolve this issue.

## Contributing

Feel free to submit issues and pull requests.

- `1.Linux(best option)`
- `2.MacOS(second best option)`
- `3.Windows(hard but possible)`
