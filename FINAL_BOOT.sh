#!/bin/bash
cd /mnt/c/Users/ccoll/OneDrive/Desktop/CompileOS

echo "🔥 BUILDING COMPILEOS WITH PROPER MULTIBOOT FORMAT 🔥"
echo "====================================================="

# Clean and build
make clean
make

if [ -f build/kernel.bin ]; then
    echo ""
    echo "✅ KERNEL COMPILED SUCCESSFULLY!"
    echo ""
    echo "📀 CREATING BOOTABLE ISO..."
    rm -rf iso
    mkdir -p iso/boot/grub
    cp build/kernel.bin iso/boot/

    # Create GRUB config
    cat > iso/boot/grub/grub.cfg << 'EOF'
set timeout=0
set default=0

menuentry "CompileOS - Interactive Terminal" {
    multiboot /boot/kernel.bin
    boot
}

menuentry "CompileOS - Desktop Environment" {
    multiboot /boot/kernel.bin
    boot
}
EOF

    # Create the ISO
    grub-mkrescue -o compileos_final.iso iso 2>&1 | grep -v warning || true

    if [ -f compileos_final.iso ]; then
        echo ""
        echo "🎉 ISO CREATED SUCCESSFULLY!"
        echo ""
        echo "🚀 BOOTING COMPILEOS IN QEMU..."
        echo ""
        echo "WHAT YOU SHOULD SEE:"
        echo "  ✅ GRUB bootloader menu"
        echo "  ✅ Boot progress: BB → CC → PP → 66 → KK"
        echo "  ✅ Full interactive terminal"
        echo "  ✅ Working keyboard input"
        echo "  ✅ Command execution"
        echo ""
        echo "COMMANDS TO TRY:"
        echo "  help    - Show all commands"
        echo "  version - OS version info"
        echo "  echo hello - Echo text"
        echo "  clear   - Clear screen"
        echo ""
        qemu-system-x86_64 -cdrom compileos_final.iso -m 512M
    else
        echo "❌ ISO CREATION FAILED"
    fi
else
    echo "❌ KERNEL BUILD FAILED"
fi
