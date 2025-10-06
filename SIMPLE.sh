#!/bin/bash
cd /mnt/c/Users/ccoll/OneDrive/Desktop/CompileOS

echo "🚀 COMPILEOS - SIMPLE & WORKING 🚀"
echo "=================================="

# Clean build
echo ""
echo "Step 1: Cleaning..."
rm -rf build

echo ""
echo "Step 2: Building bootloader..."
mkdir -p build/obj/boot
nasm -f elf32 src/boot/multiboot.asm -o build/obj/boot/multiboot.o

echo ""
echo "Step 3: Building simple kernel (everything in one file)..."
mkdir -p build/obj/kernel
gcc -Wall -Wextra -std=c99 -ffreestanding -fno-builtin -fno-stack-protector -m32 -Isrc -Isrc/hal -c src/kernel/main_simple.c -o build/obj/kernel/main.o

if [ -f build/obj/kernel/main.o ]; then
    echo ""
    echo "✅ SIMPLE KERNEL COMPILED!"

    echo ""
    echo "Step 4: Linking simple kernel..."
    ld -m elf_i386 -T src/boot/linker32.ld -o build/kernel.bin build/obj/boot/multiboot.o build/obj/kernel/main.o

    if [ -f build/kernel.bin ]; then
        echo ""
        echo "🎉 SIMPLE KERNEL LINKED SUCCESSFULLY!"
        echo ""
        echo "🚀 LAUNCHING COMPILEOS..."
        echo ""
        echo "WHAT YOU WILL SEE:"
        echo "  ✅ Cyan desktop background"
        echo "  ✅ Blue title bar with OS name"
        echo "  ✅ Desktop icons (MyPC, Files)"
        echo "  ✅ Terminal window with borders"
        echo "  ✅ MOUSE CURSOR (yellow lines)"
        echo "  ✅ KEYBOARD INPUT (guaranteed)"
        echo "  ✅ WASD cursor control"
        echo ""
        echo "CONTROLS:"
        echo "  WASD keys = Move cursor"
        echo "  Type commands in terminal"
        echo "  help, version, clear work"
        echo ""
        qemu-system-i386 -kernel build/kernel.bin -m 512M
    else
        echo "❌ SIMPLE KERNEL LINK FAILED"
    fi
else
    echo "❌ SIMPLE KERNEL COMPILATION FAILED"
fi
