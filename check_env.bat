@echo off
echo Checking development environment...

where i686-elf-gcc >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo ERROR: i686-elf-gcc not found! Please install the cross-compiler
    exit /b 1
)

where nasm >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo ERROR: NASM not found! Please install NASM assembler
    exit /b 1
)

where grub-mkrescue >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo ERROR: GRUB tools not found! Please install GRUB
    exit /b 1
)

where xorriso >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo ERROR: xorriso not found! Please install xorriso
    exit /b 1
)

echo All required tools are present!
exit /b 0 