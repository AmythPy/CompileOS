@echo off
echo Building CompileOS...
echo.

REM Check if make is available
where make >nul 2>&1
if %errorlevel% equ 0 (
    echo Using Makefile...
    make clean
    make all
) else (
    echo Make not found, using manual build...
    echo.
    
    REM Create build directories
    if not exist build mkdir build
    if not exist build\obj mkdir build\obj
    if not exist build\obj\kernel mkdir build\obj\kernel
    if not exist build\obj\kernel\memory mkdir build\obj\kernel\memory
    if not exist build\obj\kernel\terminal mkdir build\obj\kernel\terminal
    if not exist build\obj\kernel\repl mkdir build\obj\kernel\repl
    if not exist build\obj\kernel\debugger mkdir build\obj\kernel\debugger
    if not exist build\obj\kernel\desktop mkdir build\obj\kernel\desktop
    if not exist build\obj\hal mkdir build\obj\hal
    if not exist build\obj\hal\arch mkdir build\obj\hal\arch
    if not exist build\obj\hal\arch\x86_64 mkdir build\obj\hal\arch\x86_64
    if not exist build\obj\drivers mkdir build\obj\drivers
    if not exist build\obj\drivers\vga mkdir build\obj\drivers\vga
    if not exist build\obj\boot mkdir build\obj\boot
    
    echo Build directories created.
    echo.
    echo Manual build not implemented yet.
    echo Please install make or use the existing kernel.bin
)

echo.
if exist build\kernel.bin (
    echo Build successful! Kernel binary: build\kernel.bin
    echo Size: 
    dir build\kernel.bin | findstr kernel.bin
) else (
    echo Build failed or kernel.bin not found.
)

echo.
pause
