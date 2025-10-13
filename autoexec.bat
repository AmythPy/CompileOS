@echo off
REM CompileOS Autoexec - Automatic Build and Run Script
REM This script automates the build and execution of CompileOS

echo ========================================
echo CompileOS Autoexec v1.0
echo ========================================
echo.

REM Check if build tools are installed
echo Checking for build tools...
where gcc >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ERROR: GCC not found!
    echo Please install MinGW-w64 first.
    echo See SETUP_INSTRUCTIONS.md for details.
    echo.
    pause
    exit /b 1
)

where nasm >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ERROR: NASM not found!
    echo Please install NASM first.
    echo See SETUP_INSTRUCTIONS.md for details.
    echo.
    pause
    exit /b 1
)

echo ✓ GCC found
echo ✓ NASM found
echo.

REM Build CompileOS
echo Building CompileOS...
call build_test.bat
if %ERRORLEVEL% NEQ 0 (
    echo.
    echo Build failed! Check errors above.
    pause
    exit /b 1
)

echo.
echo ========================================
echo Build completed successfully!
echo ========================================
echo.

REM Check if QEMU is available
where qemu-system-x86_64 >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo QEMU found! Starting CompileOS...
    echo.
    echo Press Ctrl+C to stop QEMU
    echo.
    timeout /t 2 >nul
    qemu-system-x86_64 -kernel build\kernel.bin -m 512M
) else (
    echo.
    echo QEMU not found. Kernel built successfully at:
    echo   build\kernel.bin
    echo.
    echo To run CompileOS:
    echo   1. Install QEMU (see SETUP_INSTRUCTIONS.md)
    echo   2. Run: qemu-system-x86_64 -kernel build\kernel.bin -m 512M
    echo.
    pause
)
