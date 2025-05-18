@echo off
echo This script will help set up your development environment.
echo.
echo Prerequisites:
echo 1. MSYS2 should be installed at C:\msys64
echo 2. Internet connection for downloading packages
echo.

REM Check if MSYS2 is installed
if not exist "C:\msys64" (
    echo Error: MSYS2 not found!
    echo Please install MSYS2 from https://www.msys2.org/
    echo After installing, run this script again.
    pause
    exit /b 1
)

REM Add MSYS2 to PATH temporarily
set "PATH=C:\msys64\usr\bin;C:\msys64\mingw64\bin;%PATH%"

echo Installing required packages...
echo This may take a while...

REM Update package database
C:\msys64\usr\bin\pacman.exe -Syu --noconfirm

REM Install required packages
C:\msys64\usr\bin\pacman.exe -S --noconfirm ^
    mingw-w64-x86_64-gcc ^
    mingw-w64-x86_64-binutils ^
    nasm ^
    make ^
    grub ^
    xorriso ^
    qemu

echo.
echo Setup complete! You can now build the OS using:
echo   build.bat
echo.
echo Or if you prefer using make:
echo   make
echo.
pause 