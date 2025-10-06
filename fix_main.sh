#!/bin/bash
cd /mnt/c/Users/ccoll/OneDrive/Desktop/CompileOS

# Keep only first 212 lines of main.c
head -n 212 src/kernel/main.c > src/kernel/main.c.tmp
mv src/kernel/main.c.tmp src/kernel/main.c

echo "Fixed! File now has 212 lines."
