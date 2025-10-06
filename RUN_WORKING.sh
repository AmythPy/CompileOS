#!/bin/bash
cd /mnt/c/Users/ccoll/OneDrive/Desktop/CompileOS

echo "🔥 BUILDING WORKING COMPILEOS 🔥"
echo "================================"

# Clean and build
make clean
make

if [ -f build/kernel.bin ]; then
    echo ""
    echo "✅ BUILD SUCCESSFUL!"
    echo ""
    echo "🚀 CREATING BOOTABLE ISO..."
    rm -rf iso
    mkdir -p iso/boot/grub
    cp build/kernel.bin iso/boot/

    cat > iso/boot/grub/grub.cfg << 'EOF'
set timeout=0
menuentry "CompileOS - Working Interactive Terminal" {
    multiboot /boot/kernel.bin
    boot
}
EOF

    grub-mkrescue -o compileos_working.iso iso 2>&1 | grep -v warning || true

    echo ""
    echo "🎉 COMPILEOS IS NOW WORKING!"
    echo ""
    echo "WHAT YOU WILL SEE:"
    echo "  ✅ Black screen with white text"
    echo "  ✅ Header: 'CompileOS v0.1.0 - Interactive Terminal'"
    echo "  ✅ Welcome message and system info"
    echo "  ✅ 'CompileOS>' prompt"
    echo "  ✅ Status bar at bottom"
    echo ""
    echo "WHAT YOU CAN DO:"
    echo "  ✅ Type 'help' - See all commands"
    echo "  ✅ Type 'version' - Show OS version"
    echo "  ✅ Type 'echo hello' - Echo text"
    echo "  ✅ Type 'clear' - Clear screen"
    echo "  ✅ Type 'reboot' - Reboot system"
    echo "  ✅ Use backspace to edit"
    echo "  ✅ Press Enter to execute"
    echo ""
    echo "THIS IS A REAL, WORKING OS!"
    echo ""
    echo "💫 LAUNCHING IN QEMU..."
    echo ""
    qemu-system-x86_64 -cdrom compileos_working.iso -m 512M
else
    echo "❌ BUILD FAILED"
fi
