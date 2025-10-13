@echo off
REM CompileOS Build Script for Windows
REM Builds the embedded OS system

echo CompileOS Build Script
echo ====================

REM Check if we have the required tools
where gcc >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: GCC not found. Please install MinGW-w64 or MSYS2
    echo Download from: https://www.msys2.org/
    pause
    exit /b 1
)

where nasm >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: NASM not found. Please install NASM
    echo Download from: https://www.nasm.us/
    pause
    exit /b 1
)

echo Building CompileOS...

REM Create build directories
if not exist build mkdir build
if not exist build\obj mkdir build\obj
if not exist build\obj\kernel mkdir build\obj\kernel
if not exist build\obj\kernel\memory mkdir build\obj\kernel\memory
if not exist build\obj\kernel\process mkdir build\obj\kernel\process
if not exist build\obj\kernel\debugger mkdir build\obj\kernel\debugger
if not exist build\obj\kernel\terminal mkdir build\obj\kernel\terminal
if not exist build\obj\kernel\repl mkdir build\obj\kernel\repl
if not exist build\obj\hal mkdir build\obj\hal
if not exist build\obj\hal\arch mkdir build\obj\hal\arch
if not exist build\obj\hal\arch\x86_64 mkdir build\obj\hal\arch\x86_64
if not exist build\obj\drivers mkdir build\obj\drivers
if not exist build\obj\drivers\vga mkdir build\obj\drivers\vga
if not exist build\obj\boot mkdir build\obj\boot
if not exist build\obj\tools mkdir build\obj\tools

REM Compiler flags
set CFLAGS=-Wall -Wextra -std=c99 -ffreestanding -fno-stack-protector -fno-builtin -mno-red-zone -mcmodel=kernel
set ASFLAGS=-f win64
set LDFLAGS=-T linker.ld -nostdlib

echo Compiling C sources...
gcc %CFLAGS% -c src\kernel\main.c -o build\obj\kernel\main.o
gcc %CFLAGS% -c src\kernel\kernel.c -o build\obj\kernel\kernel.o
gcc %CFLAGS% -c src\hal\hal.c -o build\obj\hal\hal.o
gcc %CFLAGS% -c src\hal\arch\x86_64\cpu.c -o build\obj\hal\arch\x86_64\cpu.o
gcc %CFLAGS% -c src\hal\arch\x86_64\interrupts.c -o build\obj\hal\arch\x86_64\interrupts.o
gcc %CFLAGS% -c src\kernel\memory\memory.c -o build\obj\kernel\memory\memory.o
gcc %CFLAGS% -c src\kernel\memory\multibit.c -o build\obj\kernel\memory\multibit.c
gcc %CFLAGS% -c src\kernel\process\process.c -o build\obj\kernel\process\process.o
gcc %CFLAGS% -c src\tools\embed_os.c -o build\obj\tools\embed_os.o

echo Assembling ASM sources...
nasm %ASFLAGS% src\hal\arch\x86_64\interrupts.asm -o build\obj\hal\arch\x86_64\interrupts_asm.o
nasm %ASFLAGS% src\boot\boot.asm -o build\obj\boot\boot.o
nasm %ASFLAGS% src\boot\embedded_boot.asm -o build\obj\boot\embedded_boot.o

echo Linking kernel...
gcc %LDFLAGS% -o build\kernel.bin build\obj\kernel\main.o build\obj\kernel\kernel.o build\obj\hal\hal.o build\obj\hal\arch\x86_64\cpu.o build\obj\hal\arch\x86_64\interrupts.o build\obj\hal\arch\x86_64\interrupts_asm.o build\obj\kernel\memory\memory.o build\obj\kernel\memory\multibit.c build\obj\kernel\process\process.o

echo Creating bootloader binaries...
objcopy -O binary build\obj\boot\boot.o build\bootloader.bin
objcopy -O binary build\obj\boot\embedded_boot.o build\embedded_bootloader.bin

echo Building embed tool...
gcc -o build\embed_os.exe build\obj\tools\embed_os.o

echo Testing splash screen...
if exist splash.bmp (
    echo Splash screen found: splash.bmp
    echo File size: 
    dir splash.bmp | findstr splash.bmp
) else (
    echo WARNING: splash.bmp not found
)

echo.
echo Build complete!
echo.
echo Available commands:
echo   build.bat embedded    - Create embedded OS BMP
echo   build.bat run         - Run in QEMU (if available)
echo   build.bat clean       - Clean build files
echo.
pause









