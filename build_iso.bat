@echo off
REM CompileOS ISO Builder for Windows
REM Creates a bootable ISO with GRUB

echo ==========================================
echo CompileOS ISO Builder
echo ==========================================
echo.

REM Check if running in WSL
wsl --version >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: WSL not found!
    echo This script requires WSL to run.
    echo.
    pause
    exit /b 1
)

echo Running build in WSL...
echo.

wsl bash -c "cd /mnt/c/Users/ccoll/OneDrive/Desktop/CompileOS && chmod +x build_iso.sh && ./build_iso.sh"

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ==========================================
    echo Success!
    echo ==========================================
    echo.
    echo ISO created: compileos.iso
    echo.
    echo To run:
    echo   qemu-system-x86_64 -cdrom compileos.iso -m 512M
    echo.
) else (
    echo.
    echo Build failed!
    echo.
)

pause
