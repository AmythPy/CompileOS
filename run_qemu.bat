@echo off
echo Starting CompileOS in QEMU...
echo.
echo CompileOS v0.1.0 - Hardware Agnostic Development Platform
echo Architecture: x86_64
echo Memory: 512MB
echo.
echo Press Ctrl+Alt+G to release mouse from QEMU window
echo Press Ctrl+Alt+F to toggle fullscreen
echo Press Ctrl+Alt+Q to quit QEMU
echo.
pause

"C:\Program Files\qemu\qemu-system-x86_64.exe" -kernel build\kernel.bin -m 512M -serial stdio -monitor stdio

echo.
echo CompileOS session ended.
pause
