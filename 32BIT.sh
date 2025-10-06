#!/bin/bash
cd /mnt/c/Users/ccoll/OneDrive/Desktop/CompileOS

echo "🚀 COMPILEOS - 32-BIT WORKING VERSION 🚀"
echo "======================================="

# Clean build
echo ""
echo "Step 1: Cleaning..."
rm -rf build

echo ""
echo "Step 2: Building 32-bit bootloader..."
mkdir -p build/obj/boot
nasm -f elf32 src/boot/multiboot.asm -o build/obj/boot/multiboot.o

echo ""
echo "Step 3: Building 32-bit kernel with mouse support..."
mkdir -p build/obj/kernel
gcc -Wall -Wextra -std=c99 -ffreestanding -fno-builtin -fno-stack-protector -m32 -Isrc -Isrc/hal -c src/kernel/main_32bit.c -o build/obj/kernel/main.o
gcc -Wall -Wextra -std=c99 -ffreestanding -fno-builtin -fno-stack-protector -m32 -Isrc -Isrc/hal -c src/kernel/mouse.c -o build/obj/kernel/mouse.o

if [ -f build/obj/kernel/main.o ]; then
    echo ""
    echo "✅ 32-BIT KERNEL COMPILED!"

    echo ""
    echo "Step 4: Linking 32-bit kernel..."
    ld -m elf_i386 -T src/boot/linker32.ld -o build/kernel.bin build/obj/boot/multiboot.o build/obj/kernel/main.o build/obj/kernel/mouse.o

    if [ -f build/kernel.bin ]; then
        echo ""
        echo "🎉 32-BIT KERNEL LINKED SUCCESSFULLY!"
        echo ""
        echo "🚀 LAUNCHING COMPILEOS..."
        echo ""
        echo "WHAT YOU WILL SEE:"
        echo "  ✅ Cyan desktop background"
        echo "  ✅ Blue title bar with OS name"
        echo "  ✅ Desktop icons (MyPC, Files, Setup)"
        echo "  ✅ Terminal window with borders"
        echo "  ✅ Mouse cursor (horizontal/vertical lines)"
        echo "  ✅ WASD keyboard mouse control"
        echo "  ✅ Welcome message"
        echo "  ✅ Working command prompt"
        echo "  ✅ Taskbar at bottom"
        echo ""
        echo "MOUSE CONTROL:"
        echo "  Use WASD keys to move cursor"
        echo "  W = Up, S = Down, A = Left, D = Right"
        echo ""
        echo "TYPE COMMANDS:"
        echo "  help    - Show commands"
        echo "  version - OS version"
        echo "  clear   - Clear screen"
        echo ""
        qemu-system-i386 -kernel build/kernel.bin -m 512M
    else
        echo "❌ 32-BIT KERNEL LINK FAILED"
    fi
else
    echo "❌ 32-BIT KERNEL COMPILATION FAILED"
fi
