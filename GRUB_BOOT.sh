#!/bin/bash
cd /mnt/c/Users/ccoll/OneDrive/Desktop/CompileOS

echo "🔥 COMPILEOS - FINAL ATTEMPT WITH ISO 🔥"
echo "========================================"

# Clean and build
make clean
make

if [ -f build/kernel.bin ]; then
    echo ""
    echo "✅ KERNEL COMPILED!"
    echo ""
    echo "📀 CREATING GRUB ISO..."
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

menuentry "CompileOS - Boot Test" {
    multiboot /boot/kernel.bin
    boot
}
EOF

    # Create the ISO
    echo "Creating bootable ISO..."
    grub-mkrescue -o compileos.iso iso 2>&1 | grep -v warning || true

    if [ -f compileos.iso ]; then
        echo ""
        echo "🎉 ISO CREATED SUCCESSFULLY!"
        echo ""
        echo "🚀 BOOTING WITH GRUB..."
        echo ""
        echo "Watch for boot progress:"
        echo "  BB = Bootloader started"
        echo "  CC = CPU check"
        echo "  PP = Paging setup"
        echo "  66 = 64-bit mode"
        echo "  KK = Kernel called"
        echo ""
        echo "If you see these, CompileOS is working!"
        echo ""
        qemu-system-x86_64 -cdrom compileos.iso -m 512M
    else
        echo "❌ ISO CREATION FAILED"
    fi
else
    echo "❌ KERNEL BUILD FAILED"
fi
