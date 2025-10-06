#!/bin/bash
set -e

echo "🔥 BUILDING COMPILEOS - THE WORLD DEPENDS ON THIS 🔥"
echo "=================================================="

cd /mnt/c/Users/ccoll/OneDrive/Desktop/CompileOS

echo ""
echo "Step 1: Cleaning build..."
make clean

echo ""
echo "Step 2: Compiling kernel..."
make

if [ ! -f build/kernel.bin ]; then
    echo "❌ BUILD FAILED!"
    exit 1
fi

echo ""
echo "Step 3: Creating bootable ISO..."
rm -rf iso
mkdir -p iso/boot/grub
cp build/kernel.bin iso/boot/

cat > iso/boot/grub/grub.cfg << 'EOF'
set timeout=0
menuentry "CompileOS - Interactive Desktop" {
    multiboot /boot/kernel.bin
    boot
}
EOF

grub-mkrescue -o compileos.iso iso 2>&1 | grep -v "warning:" || true

echo ""
echo "✅ BUILD COMPLETE!"
echo ""
echo "🚀 LAUNCHING COMPILEOS..."
echo ""
echo "YOU WILL SEE:"
echo "  ✓ Cyan desktop background"
echo "  ✓ Blue title bar with system info"
echo "  ✓ Desktop icons (MyPC, Files, Setup)"
echo "  ✓ Terminal window with borders"
echo "  ✓ Welcome message"
echo "  ✓ Working command prompt"
echo "  ✓ Taskbar at bottom"
echo ""
echo "TYPE COMMANDS:"
echo "  help    - Show all commands"
echo "  version - OS version"
echo "  echo    - Echo text"
echo "  calc    - Calculator"
echo "  clear   - Clear screen"
echo ""
echo "Press Ctrl+Alt+G to release mouse from QEMU"
echo ""

qemu-system-x86_64 -cdrom compileos.iso -m 512M
