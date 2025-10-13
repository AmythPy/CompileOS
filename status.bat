@echo off
echo CompileOS Status Monitor
echo ========================
echo.

echo Checking QEMU process...
tasklist | findstr qemu-system-x86_64.exe
if %errorlevel% equ 0 (
    echo QEMU is running CompileOS
) else (
    echo QEMU is not running
)

echo.
echo Checking kernel binary...
if exist build\kernel.bin (
    echo Kernel binary exists: build\kernel.bin
    dir build\kernel.bin | findstr kernel.bin
) else (
    echo Kernel binary not found
)

echo.
echo Available scripts:
echo   run_qemu.bat    - Launch CompileOS in QEMU
echo   run_qemu.ps1    - PowerShell launcher with error checking
echo   build.bat       - Build CompileOS kernel
echo   status.bat      - This status monitor

echo.
pause
