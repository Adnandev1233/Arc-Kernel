@echo off
setlocal enabledelayedexpansion

echo Checking GRUB requirements...

REM Check if MSYS2 is installed
if exist "C:\msys64" (
    echo MSYS2 found at C:\msys64
) else (
    echo MSYS2 not found!
    echo Please download and install MSYS2 from https://www.msys2.org/
    echo After installation, run this script again.
    pause
    exit /b 1
)

REM Add MSYS2 to PATH if not already there
echo !PATH! | find "C:\msys64\usr\bin" > nul
if errorlevel 1 (
    echo Adding MSYS2 to PATH...
    set "PATH=C:\msys64\usr\bin;!PATH!"
    setx PATH "C:\msys64\usr\bin;%PATH%"
)

REM Check for required tools
where grub-mkrescue >nul 2>&1
if errorlevel 1 (
    echo grub-mkrescue not found!
    echo Installing GRUB...
    C:\msys64\usr\bin\bash.exe -lc "pacman -S --noconfirm grub"
)

where xorriso >nul 2>&1
if errorlevel 1 (
    echo xorriso not found!
    echo Installing xorriso...
    C:\msys64\usr\bin\bash.exe -lc "pacman -S --noconfirm xorriso"
)

where mtools >nul 2>&1
if errorlevel 1 (
    echo mtools not found!
    echo Installing mtools...
    C:\msys64\usr\bin\bash.exe -lc "pacman -S --noconfirm mtools"
)

echo.
echo Checking if GRUB modules are present...
if not exist "C:\msys64\usr\lib\grub\i386-pc" (
    echo GRUB modules not found!
    echo Reinstalling GRUB...
    C:\msys64\usr\bin\bash.exe -lc "pacman -S --noconfirm grub"
)

echo.
echo Setup complete! Please close and reopen your terminal/IDE to ensure PATH changes take effect.
echo You can now try building your OS again.
pause 