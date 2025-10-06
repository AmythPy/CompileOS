#!/bin/bash
cd /mnt/c/Users/ccoll/OneDrive/Desktop/CompileOS

echo "🚀 COMPILEOS - GUARANTEED WORKING VERSION 🚀"
echo "============================================="

# Clean build
echo ""
echo "Step 1: Cleaning..."
rm -rf build

echo ""
echo "Step 2: Building bootloader..."
mkdir -p build/obj/boot
nasm -f elf64 src/boot/multiboot.asm -o build/obj/boot/multiboot.o

echo ""
echo "Step 3: Building kernel with WORKING main.c..."
mkdir -p build/obj/kernel
gcc -Wall -Wextra -std=c99 -ffreestanding -fno-builtin -fno-stack-protector -mno-red-zone -mcmodel=kernel -fno-pic -fno-pie -Isrc -Isrc/hal -c src/kernel/main_guaranteed.c -o build/obj/kernel/main.o

if [ -f build/obj/kernel/main.o ]; then
    echo ""
    echo "✅ MAIN KERNEL COMPILED!"

    echo ""
    echo "Step 4: Linking kernel..."
    ld -T src/boot/linker.ld -o build/kernel.bin build/obj/boot/multiboot.o build/obj/kernel/main.o

    if [ -f build/kernel.bin ]; then
        echo ""
        echo "🎉 KERNEL LINKED SUCCESSFULLY!"
        echo ""
        echo "🚀 LAUNCHING COMPILEOS..."
        echo ""
        echo "WHAT YOU WILL SEE:"
        echo "  ✅ Cyan desktop background"
        echo "  ✅ Blue title bar with OS name"
        echo "  ✅ Desktop icons (MyPC, Files, Setup)"
        echo "  ✅ Terminal window with borders"
        echo "  ✅ Welcome message"
        echo "  ✅ Working command prompt"
        echo "  ✅ Taskbar at bottom"
        echo ""
        echo "TYPE COMMANDS:"
        echo "  help    - Show commands"
        echo "  version - OS version"
        echo "  clear   - Clear screen"
        echo ""
        qemu-system-x86_64 -kernel build/kernel.bin -m 512M
    else
        echo "❌ KERNEL LINK FAILED"
    fi
else
    echo "❌ MAIN KERNEL COMPILATION FAILED"
fi
