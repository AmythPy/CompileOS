#!/bin/bash
cd /mnt/c/Users/ccoll/OneDrive/Desktop/CompileOS

echo "Removing placeholder files..."

rm -f placeholder.asm
rm -f placeholder.cfg
rm -f placeholder.dat
rm -f placeholder.dll
rm -f placeholder.exe
rm -f placeholder.img
rm -f placeholder.iso
rm -f placeholder.json
rm -f placeholder.lua
rm -f placeholder.md
rm -f placeholder.o
rm -f placeholder.py
rm -f placeholder.sh
rm -f placeholder.so
rm -f placeholder.xml

echo "✓ All placeholder files removed!"
echo ""
echo "Remaining files:"
ls -lh placeholder* 2>/dev/null || echo "  (none - all cleaned up!)"
