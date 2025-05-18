@echo off
echo This script will help set up your development environment.
echo.
echo Prerequisites:
echo 1. MSYS2 should be installed at C:\msys64
echo 2. i686-elf cross-compiler should be installed at C:\i686-elf-tools
echo.
echo Please install MSYS2 from https://www.msys2.org/ if you haven't already.
echo.
pause

:: Add tools to PATH
setx PATH "%PATH%;C:\msys64\usr\bin;C:\i686-elf-tools\bin"

echo.
echo Environment variables have been set.
echo Please run the following commands in MSYS2:
echo.
echo pacman -Syu
echo pacman -S mingw-w64-x86_64-gcc nasm grub xorriso
echo.
echo After installing the packages, you can run build.bat to compile the kernel.
echo.
pause 