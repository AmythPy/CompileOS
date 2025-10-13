@echo off
echo ========================================
echo CompileOS Build Test
echo ========================================
echo.

echo Checking for required tools...
where gcc >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: GCC not found in PATH
    exit /b 1
)

where nasm >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: NASM not found in PATH
    exit /b 1
)

echo Tools found: GCC and NASM
echo.

echo Creating build directories...
if not exist build mkdir build
if not exist build\obj mkdir build\obj
if not exist build\obj\kernel mkdir build\obj\kernel
if not exist build\obj\kernel\memory mkdir build\obj\kernel\memory
if not exist build\obj\kernel\process mkdir build\obj\kernel\process
if not exist build\obj\kernel\debugger mkdir build\obj\kernel\debugger
if not exist build\obj\kernel\terminal mkdir build\obj\kernel\terminal
if not exist build\obj\kernel\repl mkdir build\obj\kernel\repl
if not exist build\obj\kernel\desktop mkdir build\obj\kernel\desktop
if not exist build\obj\hal mkdir build\obj\hal
if not exist build\obj\hal\arch mkdir build\obj\hal\arch
if not exist build\obj\hal\arch\x86_64 mkdir build\obj\hal\arch\x86_64
if not exist build\obj\drivers mkdir build\obj\drivers
if not exist build\obj\drivers\vga mkdir build\obj\drivers\vga
if not exist build\obj\drivers\keyboard mkdir build\obj\drivers\keyboard
if not exist build\obj\kernel\fs mkdir build\obj\kernel\fs
if not exist build\obj\boot mkdir build\obj\boot

echo Directories created.
echo.

echo Compiling kernel sources...
gcc -Wall -Wextra -std=c99 -ffreestanding -fno-builtin -fno-stack-protector -mno-red-zone -mcmodel=kernel -fno-pic -fno-pie -Isrc -Isrc/hal -c src/kernel/main.c -o build/obj/kernel/main.o
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to compile main.c
    exit /b 1
)

gcc -Wall -Wextra -std=c99 -ffreestanding -fno-builtin -fno-stack-protector -mno-red-zone -mcmodel=kernel -fno-pic -fno-pie -Isrc -Isrc/hal -c src/kernel/util.c -o build/obj/kernel/util.o
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to compile util.c
    exit /b 1
)

gcc -Wall -Wextra -std=c99 -ffreestanding -fno-builtin -fno-stack-protector -mno-red-zone -mcmodel=kernel -fno-pic -fno-pie -Isrc -Isrc/hal -c src/kernel/memory/memory.c -o build/obj/kernel/memory/memory.o
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to compile memory.c
    exit /b 1
)

gcc -Wall -Wextra -std=c99 -ffreestanding -fno-builtin -fno-stack-protector -mno-red-zone -mcmodel=kernel -fno-pic -fno-pie -Isrc -Isrc/hal -c src/kernel/memory/multibit.c -o build/obj/kernel/memory/multibit.o
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to compile multibit.c
    exit /b 1
)

gcc -Wall -Wextra -std=c99 -ffreestanding -fno-builtin -fno-stack-protector -mno-red-zone -mcmodel=kernel -fno-pic -fno-pie -Isrc -Isrc/hal -c src/kernel/memory/memory_tools.c -o build/obj/kernel/memory/memory_tools.o
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to compile memory_tools.c
    exit /b 1
)

gcc -Wall -Wextra -std=c99 -ffreestanding -fno-builtin -fno-stack-protector -mno-red-zone -mcmodel=kernel -fno-pic -fno-pie -Isrc -Isrc/hal -c src/kernel/process/process.c -o build/obj/kernel/process/process.o
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to compile process.c
    exit /b 1
)

gcc -Wall -Wextra -std=c99 -ffreestanding -fno-builtin -fno-stack-protector -mno-red-zone -mcmodel=kernel -fno-pic -fno-pie -Isrc -Isrc/hal -c src/kernel/terminal/terminal.c -o build/obj/kernel/terminal/terminal.o
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to compile terminal.c
    exit /b 1
)

gcc -Wall -Wextra -std=c99 -ffreestanding -fno-builtin -fno-stack-protector -mno-red-zone -mcmodel=kernel -fno-pic -fno-pie -Isrc -Isrc/hal -c src/kernel/repl/repl.c -o build/obj/kernel/repl/repl.o
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to compile repl.c
    exit /b 1
)

gcc -Wall -Wextra -std=c99 -ffreestanding -fno-builtin -fno-stack-protector -mno-red-zone -mcmodel=kernel -fno-pic -fno-pie -Isrc -Isrc/hal -c src/kernel/debugger/debugger.c -o build/obj/kernel/debugger/debugger.o
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to compile debugger.c
    exit /b 1
)

gcc -Wall -Wextra -std=c99 -ffreestanding -fno-builtin -fno-stack-protector -mno-red-zone -mcmodel=kernel -fno-pic -fno-pie -Isrc -Isrc/hal -c src/kernel/desktop/desktop.c -o build/obj/kernel/desktop/desktop.o
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to compile desktop.c
    exit /b 1
)

gcc -Wall -Wextra -std=c99 -ffreestanding -fno-builtin -fno-stack-protector -mno-red-zone -mcmodel=kernel -fno-pic -fno-pie -Isrc -Isrc/hal -c src/hal/hal.c -o build/obj/hal/hal.o
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to compile hal.c
    exit /b 1
)

gcc -Wall -Wextra -std=c99 -ffreestanding -fno-builtin -fno-stack-protector -mno-red-zone -mcmodel=kernel -fno-pic -fno-pie -Isrc -Isrc/hal -c src/hal/arch/x86_64/cpu.c -o build/obj/hal/arch/x86_64/cpu.o
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to compile cpu.c
    exit /b 1
)

gcc -Wall -Wextra -std=c99 -ffreestanding -fno-builtin -fno-stack-protector -mno-red-zone -mcmodel=kernel -fno-pic -fno-pie -Isrc -Isrc/hal -c src/hal/arch/x86_64/interrupts.c -o build/obj/hal/arch/x86_64/interrupts.o
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to compile interrupts.c
    exit /b 1
)

gcc -Wall -Wextra -std=c99 -ffreestanding -fno-builtin -fno-stack-protector -mno-red-zone -mcmodel=kernel -fno-pic -fno-pie -Isrc -Isrc/hal -c src/drivers/vga/vga.c -o build/obj/drivers/vga/vga.o
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to compile vga.c
    exit /b 1
)

gcc -Wall -Wextra -std=c99 -ffreestanding -fno-builtin -fno-stack-protector -mno-red-zone -mcmodel=kernel -fno-pic -fno-pie -Isrc -Isrc/hal -c src/drivers/keyboard/keyboard.c -o build/obj/drivers/keyboard/keyboard.o
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to compile keyboard.c
    exit /b 1
)

gcc -Wall -Wextra -std=c99 -ffreestanding -fno-builtin -fno-stack-protector -mno-red-zone -mcmodel=kernel -fno-pic -fno-pie -Isrc -Isrc/hal -c src/kernel/fs/fs.c -o build/obj/kernel/fs/fs.o
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to compile fs.c
    exit /b 1
)

echo Compiling assembly sources...
nasm -f elf64 src/hal/arch/x86_64/interrupts.asm -o build/obj/hal/arch/x86_64/interrupts_asm.o
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to assemble interrupts.asm
    exit /b 1
)

nasm -f elf64 src/boot/boot.asm -o build/obj/boot/boot.o
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to assemble boot.asm
    exit /b 1
)

echo.
echo Linking kernel...
ld -T linker.ld -nostdlib -o build/kernel.bin build/obj/kernel/main.o build/obj/kernel/util.o build/obj/kernel/memory/memory.o build/obj/kernel/memory/multibit.o build/obj/kernel/memory/memory_tools.o build/obj/kernel/process/process.o build/obj/kernel/terminal/terminal.o build/obj/kernel/repl/repl.o build/obj/kernel/debugger/debugger.o build/obj/kernel/desktop/desktop.o build/obj/kernel/fs/fs.o build/obj/hal/hal.o build/obj/hal/arch/x86_64/cpu.o build/obj/hal/arch/x86_64/interrupts.o build/obj/hal/arch/x86_64/interrupts_asm.o build/obj/drivers/vga/vga.o build/obj/drivers/keyboard/keyboard.o
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to link kernel
    exit /b 1
)

echo.
echo ========================================
echo Build completed successfully!
echo ========================================
echo Kernel binary: build\kernel.bin
echo.
