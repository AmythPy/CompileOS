#!/bin/bash
cd /mnt/c/Users/ccoll/OneDrive/Desktop/CompileOS

echo "🚀 COMPILEOS - FINAL EXECUTION 🚀"
echo "================================="

# Clean and build
echo ""
echo "Step 1: Cleaning build..."
rm -rf build

echo ""
echo "Step 2: Compiling complete OS..."
make

if [ -f build/kernel.bin ]; then
    echo ""
    echo "✅ COMPILATION SUCCESSFUL!"
    echo ""
    echo "📀 CREATING BOOTABLE ISO..."
    rm -rf iso
    mkdir -p iso/boot/grub
    cp build/kernel.bin iso/boot/

    # Create GRUB config
    cat > iso/boot/grub/grub.cfg << 'EOF'
set timeout=0
set default=0

menuentry "CompileOS - Complete Desktop Environment" {
    multiboot /boot/kernel.bin
    boot
}

menuentry "CompileOS - Hardware Agnostic Platform" {
    multiboot /boot/kernel.bin
    boot
}
EOF

    # Create the ISO
    grub-mkrescue -o compileos_ready.iso iso 2>&1 | grep -v warning || true

    if [ -f compileos_ready.iso ]; then
        echo ""
        echo "🎉 COMPILEOS ISO CREATED!"
        echo ""
        echo "🌟 LAUNCHING COMPILEOS..."
        echo ""
        echo "WHAT YOU WILL SEE:"
        echo "  ✅ Desktop environment with windows"
        echo "  ✅ Interactive terminal"
        echo "  ✅ Working keyboard input"
        echo "  ✅ Desktop applications"
        echo "  ✅ Complete command system"
        echo ""
        echo "🎊 EXPERIENCE YOUR CREATION! 🎊"
        echo ""
        qemu-system-x86_64 -cdrom compileos_ready.iso -m 512M
    else
        echo "❌ ISO CREATION FAILED"
    fi
else
    echo "❌ COMPILATION FAILED"
fi
