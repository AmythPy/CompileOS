# CompileOS Userland & Tools Guide 🚀

## Overview

CompileOS now has a complete **runtime library**, **userland applications**, and **development tools**!

---

## 📚 Runtime Library

**Location:** `src/runtime/`

The runtime library provides standard functions for userland applications.

### Features

#### System Calls
```c
int syscall_exit(int code);
int syscall_write(const char* str);
int syscall_read(char* buffer, size_t size);
```

#### Standard I/O
```c
int print(const char* str);
int println(const char* str);
int printf_rt(const char* format, ...);
char* readline(void);
int getchar_rt(void);
```

#### Memory Management
```c
void* rt_malloc(size_t size);
void rt_free(void* ptr);
void* rt_realloc(void* ptr, size_t size);
```

#### String Utilities
```c
char* rt_strdup(const char* str);
int rt_atoi(const char* str);
char* rt_itoa(int value, char* buffer);
```

#### File Operations
```c
int rt_file_exists(const char* path);
int rt_file_read(const char* path, char* buffer, size_t size);
int rt_file_write(const char* path, const char* data, size_t size);
int rt_file_append(const char* path, const char* data, size_t size);
```

#### Process Management
```c
int rt_spawn(const char* program, char** args);
int rt_wait(int pid);
int rt_kill(int pid);
int rt_getpid(void);
```

---

## 🖥️ Userland Applications

### 1. Shell (`shell.c`)

An interactive command shell for running commands and programs.

**Features:**
- Built-in commands: `cd`, `pwd`, `exit`
- External program execution
- Command parsing
- Current directory tracking

**Usage:**
```c
shell_main();
```

**Example Session:**
```
CompileOS Shell v1.0
Type 'exit' to quit

/> pwd
/
/> cd /home
/home> ls
/home> exit
Goodbye!
```

---

### 2. Calculator (`calculator.c`)

An interactive calculator using the REPL engine.

**Features:**
- Expression evaluation
- Variable support
- Clear and exit commands
- Nice UI with borders

**Usage:**
```c
calculator_main();
```

**Example Session:**
```
╔════════════════════════════════╗
║   CompileOS Calculator v1.0    ║
╠════════════════════════════════╣
║ 0                              ║
╠════════════════════════════════╣
║  Commands:                     ║
║  - Type expression and Enter   ║
║  - 'clear' to clear            ║
║  - 'exit' to quit              ║
╚════════════════════════════════╝

Expression: 2 + 3
Result: 5

Expression: x = 100
Result: 100

Expression: x * 2
Result: 200
```

---

### 3. Text Editor (`texteditor.c`)

A simple text editor for creating and editing files.

**Features:**
- Multiple lines (up to 100)
- Save/load files
- Line numbers
- Command mode
- Modified indicator

**Usage:**
```c
texteditor_main("myfile.txt");
```

**Commands:**
- `:w` - Save file
- `:q` - Quit (warns if modified)
- `:wq` - Save and quit
- `:q!` - Quit without saving
- `i` - Insert mode (type text)
- `ESC` - Command mode

**Example Session:**
```
╔════════════════════════════════════════════════════════════════════════════╗
║ CompileOS Text Editor - myfile.txt *                                       ║
╠════════════════════════════════════════════════════════════════════════════╣
║  1 │ Hello, World!                                                         ║
║  2 │ This is a test file.                                                  ║
║  3 │                                                                        ║
╠════════════════════════════════════════════════════════════════════════════╣
║ Commands: :w (save) :q (quit) :wq (save & quit) i (insert) ESC (command)  ║
╚════════════════════════════════════════════════════════════════════════════╝

Line 1, Col 1 > :w
File saved!
```

---

### 4. File Manager (`filemanager.c`)

An interactive file browser and manager.

**Features:**
- File listing
- Directory navigation
- File operations (copy, move, delete)
- Create files and directories
- View file contents
- Nice UI with file types and sizes

**Usage:**
```c
filemanager_main();
```

**Commands:**
- `ls` - List files
- `cd <dir>` - Change directory
- `cat <file>` - View file contents
- `rm <file>` - Delete file
- `mkdir <dir>` - Create directory
- `touch <file>` - Create empty file
- `cp <src> <dst>` - Copy file
- `mv <src> <dst>` - Move/rename file
- `exit` - Quit

**Example Session:**
```
╔════════════════════════════════════════════════════════════════════════════╗
║ CompileOS File Manager - /                                                 ║
╠════════════════════════════════════════════════════════════════════════════╣
║ Type  │ Name                                    │ Size                      ║
╠════════════════════════════════════════════════════════════════════════════╣
║ > [D] │ home                                    │        0 bytes            ║
║   [F] │ test.txt                                │      128 bytes            ║
║   [F] │ readme.md                               │      512 bytes            ║
╠════════════════════════════════════════════════════════════════════════════╣
║ Commands: ls, cd <dir>, cat <file>, rm <file>, mkdir <dir>, touch <file>  ║
║           cp <src> <dst>, mv <src> <dst>, exit                             ║
╚════════════════════════════════════════════════════════════════════════════╝

Command: cat test.txt
--- File Contents ---
Hello from CompileOS!
--- End ---
```

---

## 🛠️ Development Tools

### 1. Hex Dump (`hexdump.c`)

Display file contents in hexadecimal format.

**Usage:**
```c
hexdump_main("myfile.bin");
```

**Output:**
```
Hex dump of 'myfile.bin' (256 bytes):

00000000  48 65 6c 6c 6f 2c 20 57  6f 72 6c 64 21 0a 00 00  |Hello, World!...|
00000010  54 68 69 73 20 69 73 20  61 20 74 65 73 74 2e 00  |This is a test..|
00000020  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
```

---

### 2. Memory Test (`memtest.c`)

Test memory allocation and performance.

**Usage:**
```c
memtest_main();
```

**Tests:**
- Allocation test (various sizes)
- Fragmentation test
- Memory statistics

**Output:**
```
╔════════════════════════════════════════╗
║  CompileOS Memory Test Tool v1.0       ║
╚════════════════════════════════════════╝

=== Memory Allocation Test ===
✓ Allocated 16 bytes at 0x100000
  ✓ Read/write test passed
  ✓ Freed
✓ Allocated 32 bytes at 0x100020
  ✓ Read/write test passed
  ✓ Freed
...

=== Memory Fragmentation Test ===
Allocated block 0 at 0x100000
Allocated block 1 at 0x100080
...
✓ Large allocation succeeded despite fragmentation

=== Memory Statistics ===
Total Memory:  268435456 bytes
Used Memory:   2048 bytes
Free Memory:   268433408 bytes
```

---

### 3. System Info (`sysinfo.c`)

Display comprehensive system information.

**Usage:**
```c
sysinfo_main();
```

**Output:**
```
╔════════════════════════════════════════════════════════════════════════════╗
║                    CompileOS System Information v1.0                       ║
╚════════════════════════════════════════════════════════════════════════════╝

┌─ Operating System ─────────────────────────────────────────────────────────┐
│ Name:           CompileOS
│ Version:        0.1.0 (V0.01a)
│ Architecture:   x86_64
│ Build Date:     Oct  5 2025 15:00:00
│ Description:    Hardware Agnostic Development Platform
└────────────────────────────────────────────────────────────────────────────┘

┌─ CPU Information ──────────────────────────────────────────────────────────┐
│ Architecture:   x86_64
│ Mode:           64-bit
│ Features:       SSE, SSE2, x87 FPU
│ Cores:          1 (detected)
└────────────────────────────────────────────────────────────────────────────┘

┌─ Memory Information ───────────────────────────────────────────────────────┐
│ Total Memory:   268435456 bytes (256.00 MB)
│ Used Memory:    2048 bytes (0.00 MB)
│ Free Memory:    268433408 bytes (256.00 MB)
│ Heap Range:     0x100000 - 0x10000000
│ Allocations:    10
│ Frees:          5
└────────────────────────────────────────────────────────────────────────────┘

┌─ System Features ──────────────────────────────────────────────────────────┐
│ ✓ PS/2 Keyboard Driver
│ ✓ VGA Text Mode (80x25)
│ ✓ Interactive Terminal
│ ✓ REPL Expression Evaluator
│ ✓ In-Memory Filesystem
│ ✓ Desktop Environment
│ ✓ Memory Management (Heap Allocator)
│ ✓ Process Scheduler (Round-Robin)
│ ✓ Interrupt Handling (IDT, PIC)
│ ✓ Multi-bit Memory Support (16/32/64-bit)
└────────────────────────────────────────────────────────────────────────────┘
```

---

## 🎯 Integration

### Add to Terminal Commands

To make these applications accessible from the terminal, add commands:

```c
// In terminal.c

static int cmd_calc(int argc, char** argv) {
    (void)argc; (void)argv;
    return calculator_main();
}

static int cmd_edit(int argc, char** argv) {
    const char* filename = (argc > 1) ? argv[1] : NULL;
    return texteditor_main(filename);
}

static int cmd_fm(int argc, char** argv) {
    (void)argc; (void)argv;
    return filemanager_main();
}

static int cmd_hexdump(int argc, char** argv) {
    if (argc < 2) {
        terminal_printf("Usage: hexdump <filename>\n");
        return -1;
    }
    return hexdump_main(argv[1]);
}

static int cmd_memtest(int argc, char** argv) {
    (void)argc; (void)argv;
    return memtest_main();
}

static int cmd_sysinfo(int argc, char** argv) {
    (void)argc; (void)argv;
    return sysinfo_main();
}

// Add to command table:
{"calc", "Interactive calculator", cmd_calc},
{"edit", "Text editor", cmd_edit},
{"fm", "File manager", cmd_fm},
{"hexdump", "Hex dump utility", cmd_hexdump},
{"memtest", "Memory test tool", cmd_memtest},
{"sysinfo", "System information", cmd_sysinfo},
```

---

## 📦 Build Integration

Add to `build_test.bat`:

```batch
REM Runtime library
gcc %CFLAGS% -c src/runtime/runtime.c -o build/obj/runtime/runtime.o

REM Userland applications
gcc %CFLAGS% -c src/userland/shell.c -o build/obj/userland/shell.o
gcc %CFLAGS% -c src/userland/calculator.c -o build/obj/userland/calculator.o
gcc %CFLAGS% -c src/userland/texteditor.c -o build/obj/userland/texteditor.o
gcc %CFLAGS% -c src/userland/filemanager.c -o build/obj/userland/filemanager.o

REM Tools
gcc %CFLAGS% -c src/tools/hexdump.c -o build/obj/tools/hexdump.o
gcc %CFLAGS% -c src/tools/memtest.c -o build/obj/tools/memtest.o
gcc %CFLAGS% -c src/tools/sysinfo.c -o build/obj/tools/sysinfo.o
```

---

## 🎉 Summary

You now have:

### Runtime Library
- ✅ System call wrappers
- ✅ Standard I/O functions
- ✅ Memory management
- ✅ String utilities
- ✅ File operations
- ✅ Process management

### Userland Applications
- ✅ **Shell** - Command line interface
- ✅ **Calculator** - Interactive calculator
- ✅ **Text Editor** - File editing
- ✅ **File Manager** - File browsing and management

### Development Tools
- ✅ **Hex Dump** - Binary file viewer
- ✅ **Memory Test** - Memory testing and diagnostics
- ✅ **System Info** - System information display

**Total:** 10 new components, ~2,000+ lines of code!

---

**CompileOS now has a complete userland environment!** 🚀
