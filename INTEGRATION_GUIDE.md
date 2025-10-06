# CompileOS Integration Guide

## Quick Integration Steps

Follow these steps to integrate the new features into your kernel:

---

## Step 1: Update kernel_main.c

Add the new includes and initialization:

```c
// Add these includes at the top
#include "drivers/keyboard/keyboard.h"
#include "kernel/fs/fs.h"

// In kernel_init() function, add after desktop_init():
if (keyboard_init() != 0) {
    return -1;
}

if (fs_init() != 0) {
    return -1;
}

// In kernel_main() after terminal_start(), add:
// Display initial prompt
terminal_printf("\nWelcome to CompileOS!\n");
terminal_printf("Type 'help' for available commands.\n\n");

// Main kernel loop - replace the existing loop with:
while (g_kernel_state.status == KERNEL_STATUS_RUNNING) {
    // Process terminal input (handles keyboard automatically)
    terminal_run();
    
    // Yield to other processes
    process_schedule();
    
    // Halt CPU until next interrupt
    hal_halt();
}
```

---

## Step 2: Wire Keyboard IRQ Handler

### Option A: Modify interrupts.c

In `src/hal/arch/x86_64/interrupts.c`, find the `interrupt_handler_common()` function and add:

```c
extern void keyboard_irq_handler(void);

void interrupt_handler_common(interrupt_context_t* context) {
    uint32_t int_num = context->interrupt_number;
    
    // Handle keyboard interrupt (IRQ 1 = interrupt 33)
    if (int_num == 33) {
        keyboard_irq_handler();
        pic_send_eoi(1);
        return;
    }
    
    // Handle other interrupts...
    // (existing code)
}
```

### Option B: Add dedicated IRQ handler

Or add a dedicated handler in `interrupts.c`:

```c
extern void keyboard_irq_handler(void);

// IRQ 1 handler (keyboard)
void irq1_keyboard_handler(void) {
    keyboard_irq_handler();
    pic_send_eoi(1);
}
```

Then register it in `interrupts_init()`:

```c
// In interrupts_init(), after setting up IRQ handlers:
interrupts_set_handler(33, (interrupt_handler_func_t)irq1_keyboard_handler, GATE_TYPE_INTERRUPT);
```

---

## Step 3: Enable Keyboard IRQ

In `src/hal/hal.c` or `interrupts.c`, after initializing the PIC:

```c
// Enable keyboard IRQ (IRQ 1)
pic_enable_irq(1);
```

---

## Step 4: Build the Project

Run the updated build script:

```cmd
build_test.bat
```

This will now compile:
- Keyboard driver (`keyboard.c`)
- Filesystem (`fs.c`)
- Updated terminal (`terminal.c`)
- Updated REPL (`repl.c`)

---

## Step 5: Test in QEMU

```cmd
qemu-system-x86_64 -kernel build/kernel.bin -m 512M
```

You should see:
1. CompileOS boots
2. Terminal displays welcome message
3. Prompt appears: `CompileOS> `
4. You can type commands!

---

## Testing Checklist

### Keyboard Test
```
CompileOS> echo Hello World
Hello World
```

### Memory Test
```
CompileOS> memory

Memory Statistics:
  Total:  268435456 bytes
  Used:   2048 bytes
  Free:   268433408 bytes
```

### REPL Test
Create a terminal command to test REPL:

```c
static int cmd_calc(int argc, char** argv) {
    if (argc < 2) {
        terminal_printf("Usage: calc <expression>\n");
        return -1;
    }
    
    // Build expression from arguments
    char expr[256] = {0};
    for (int i = 1; i < argc; i++) {
        strcat(expr, argv[i]);
        if (i < argc - 1) strcat(expr, " ");
    }
    
    repl_value_t result;
    if (repl_evaluate(expr, &result) == 0) {
        terminal_printf("Result: %lld\n", (long long)result.value.int64_val);
    } else {
        terminal_printf("Error evaluating expression\n");
    }
    
    return 0;
}

// Add to command table:
{"calc", "Evaluate math expression", cmd_calc},
```

Test it:
```
CompileOS> calc 2 + 3
Result: 5

CompileOS> calc (10 + 5) * 2
Result: 30

CompileOS> calc x = 100
Result: 100

CompileOS> calc x * 2
Result: 200
```

### Filesystem Test
Add file commands:

```c
static int cmd_touch(int argc, char** argv) {
    if (argc < 2) {
        terminal_printf("Usage: touch <filename>\n");
        return -1;
    }
    
    if (fs_create(argv[1]) == 0) {
        terminal_printf("Created: %s\n", argv[1]);
    } else {
        terminal_printf("Error creating file\n");
    }
    
    return 0;
}

static int cmd_ls(int argc, char** argv) {
    (void)argc; (void)argv;
    
    fs_file_info_t files[64];
    size_t count;
    
    if (fs_list("/", files, 64, &count) == 0) {
        terminal_printf("\nFiles (%u):\n", (unsigned int)count);
        for (size_t i = 0; i < count; i++) {
            char type = (files[i].type == FS_TYPE_DIRECTORY) ? 'd' : '-';
            terminal_printf("  %c %8u  %s\n", 
                          type,
                          (unsigned int)files[i].size, 
                          files[i].name);
        }
        terminal_printf("\n");
    }
    
    return 0;
}

static int cmd_write(int argc, char** argv) {
    if (argc < 3) {
        terminal_printf("Usage: write <filename> <text>\n");
        return -1;
    }
    
    fs_fd_t fd = fs_open(argv[1], FS_O_WRONLY | FS_O_CREATE | FS_O_TRUNC);
    if (fd < 0) {
        terminal_printf("Error opening file\n");
        return -1;
    }
    
    // Write all remaining arguments
    for (int i = 2; i < argc; i++) {
        fs_write(fd, argv[i], strlen(argv[i]));
        if (i < argc - 1) {
            fs_write(fd, " ", 1);
        }
    }
    
    fs_close(fd);
    terminal_printf("Wrote to: %s\n", argv[1]);
    
    return 0;
}

static int cmd_cat(int argc, char** argv) {
    if (argc < 2) {
        terminal_printf("Usage: cat <filename>\n");
        return -1;
    }
    
    fs_fd_t fd = fs_open(argv[1], FS_O_RDONLY);
    if (fd < 0) {
        terminal_printf("Error: File not found\n");
        return -1;
    }
    
    char buffer[512];
    int bytes_read;
    terminal_printf("\n");
    while ((bytes_read = fs_read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0';
        terminal_printf("%s", buffer);
    }
    terminal_printf("\n\n");
    
    fs_close(fd);
    return 0;
}

// Add to command table:
{"touch", "Create empty file", cmd_touch},
{"ls", "List files", cmd_ls},
{"write", "Write text to file", cmd_write},
{"cat", "Display file contents", cmd_cat},
```

Test it:
```
CompileOS> touch test.txt
Created: test.txt

CompileOS> write test.txt Hello from CompileOS!
Wrote to: test.txt

CompileOS> cat test.txt

Hello from CompileOS!

CompileOS> ls

Files (2):
  d        0  /
  -       22  test.txt
```

---

## Troubleshooting

### Keyboard not responding
- Check that `keyboard_init()` is called
- Verify IRQ 1 is enabled: `pic_enable_irq(1)`
- Ensure `keyboard_irq_handler()` is wired to interrupt 33
- Check that interrupts are enabled: `interrupts_enable()`

### Terminal not displaying input
- Verify `terminal_run()` is called in main loop
- Check that `terminal_start()` was called
- Ensure VGA is initialized

### Commands not executing
- Check command table in `terminal.c`
- Verify command parsing is working
- Add debug output in `terminal_execute_command()`

### REPL errors
- Check for division by zero
- Verify variable names are valid (alphanumeric + underscore)
- Test simple expressions first: `2 + 2`

### Filesystem errors
- Verify `fs_init()` is called
- Check file limits (max 64 files, 64KB each)
- Ensure memory allocator is working

---

## Performance Tips

1. **Keyboard buffer** - Increase `KEYBOARD_BUFFER_SIZE` if losing keypresses
2. **Terminal history** - Adjust `TERMINAL_HISTORY_SIZE` for more/less history
3. **File limits** - Increase `FS_MAX_FILES` and `FS_MAX_FILE_SIZE` in `fs.h`
4. **REPL variables** - Increase `MAX_VARIABLES` in `repl.c`

---

## Next Features to Add

1. **Tab completion** - Auto-complete commands and filenames
2. **Arrow key history** - Navigate command history with up/down arrows
3. **Pipes** - `cat file.txt | grep pattern`
4. **Wildcards** - `ls *.txt`
5. **Background jobs** - `command &`
6. **Environment variables** - `$PATH`, `$HOME`
7. **Scripting** - Run `.sh` files
8. **Persistent storage** - Save filesystem to disk

---

## Summary

You now have:
- ✅ Working keyboard input
- ✅ Interactive terminal with commands
- ✅ Expression evaluator with variables
- ✅ In-memory filesystem

**CompileOS is now fully interactive!** 🎉

Users can type commands, do calculations, and manage files - all the essentials of a modern OS!
