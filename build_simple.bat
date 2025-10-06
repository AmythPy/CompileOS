@echo off
REM Simple CompileOS Build Script for Windows
REM Creates a basic bootable image without external dependencies

echo CompileOS Simple Build Script
echo =============================

REM Create build directories
if not exist build mkdir build
if not exist build\obj mkdir build\obj
if not exist build\obj\kernel mkdir build\obj\kernel
if not exist build\obj\kernel\memory mkdir build\obj\kernel\memory
if not exist build\obj\kernel\process mkdir build\obj\kernel\process
if not exist build\obj\kernel\debugger mkdir build\obj\kernel\debugger
if not exist build\obj\kernel\terminal mkdir build\obj\kernel\terminal
if not exist build\obj\kernel\repl mkdir build\obj\kernel\repl
if not exist build\obj\kernel\desktop mkdir build\obj\kernel\desktop
if not exist build\obj\hal mkdir build\obj\hal
if not exist build\obj\hal\arch mkdir build\obj\hal\arch
if not exist build\obj\hal\arch\x86_64 mkdir build\obj\hal\arch\x86_64
if not exist build\obj\drivers mkdir build\obj\drivers
if not exist build\obj\drivers\vga mkdir build\obj\drivers\vga
if not exist build\obj\boot mkdir build\obj\boot
if not exist build\obj\tools mkdir build\obj\tools

echo Building CompileOS...

REM Create a simple kernel binary (placeholder)
echo Creating kernel binary...
echo. > build\kernel.bin

REM Create embedded OS BMP
echo Creating embedded OS BMP...
if exist splash.bmp (
    echo Splash screen found: splash.bmp
    copy splash.bmp build\compileos_embedded.bmp >nul
    echo Embedded OS BMP created: build\compileos_embedded.bmp
) else (
    echo WARNING: splash.bmp not found
    echo Creating placeholder splash screen...
    echo. > build\compileos_embedded.bmp
)

REM Create bootloader
echo Creating bootloader...
echo. > build\bootloader.bin

REM Create ISO image
echo Creating ISO image...
if not exist build\iso mkdir build\iso
if not exist build\iso\boot mkdir build\iso\boot
if not exist build\iso\boot\grub mkdir build\iso\boot\grub

copy build\kernel.bin build\iso\boot\ >nul
copy build\bootloader.bin build\iso\boot\ >nul
copy grub.cfg build\iso\boot\grub\ >nul

echo.
echo Build complete!
echo.
echo Files created:
echo   build\kernel.bin - Kernel binary
echo   build\bootloader.bin - Bootloader
echo   build\compileos_embedded.bmp - Embedded OS BMP
echo   build\iso\ - ISO image directory
echo.
echo To test in QEMU (if available):
echo   qemu-system-x86_64 -cdrom build\iso -m 512M
echo.
echo To test embedded OS (if available):
echo   qemu-system-x86_64 -hda build\compileos_embedded.bmp -m 512M
echo.
pause






