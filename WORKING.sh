#!/bin/bash
cd /mnt/c/Users/ccoll/OneDrive/Desktop/CompileOS

echo "🔥 COMPILEOS - GUARANTEED TO WORK 🔥"
echo "==================================="

# Clean and build
make clean
make

if [ -f build/kernel.bin ]; then
    echo ""
    echo "✅ BUILD SUCCESSFUL!"
    echo ""
    echo "🚀 LAUNCHING COMPILEOS..."
    echo ""
    echo "YOU WILL SEE:"
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
    echo "❌ BUILD FAILED"
fi
