# CompileOS Desktop Environment Guide

## Overview

CompileOS now features a **full graphical desktop environment** with:
- ✅ Window management with borders and titles
- ✅ Focus indication (yellow border for active window)
- ✅ Status bar with function key shortcuts
- ✅ Title bar with OS information
- ✅ Terminal integration
- ✅ Keyboard input handling
- ✅ Multi-window support

---

## Desktop Layout

```
┌────────────────────────────────────────────────────────────────────────────┐
│ CompileOS v0.1.0 - Hardware Agnostic Development Platform            [X]  │ ← Title Bar
├────────────────────────────────────────────────────────────────────────────┤
│                                                                            │
│  ┌──────────────────────────────────────────────────────────────────────┐ │
│  │ CompileOS Terminal - Interactive Shell                              │ │ ← Window Title
│  ├──────────────────────────────────────────────────────────────────────┤ │
│  │                                                                      │ │
│  │  Welcome to CompileOS Interactive Desktop!                          │ │
│  │                                                                      │ │
│  │  Features:                                                           │ │
│  │    * PS/2 Keyboard Input                                            │ │
│  │    * Interactive Terminal Shell                                     │ │
│  │    * REPL Expression Evaluator                                      │ │
│  │    * In-Memory Filesystem                                           │ │
│  │    * Window Management                                              │ │
│  │                                                                      │ │
│  │  Type 'help' for available commands                                 │ │
│  │                                                                      │ │
│  │  System Info:                                                        │ │
│  │    Architecture: x86_64                                             │ │
│  │    Memory: 16/32/64-bit support                                     │ │
│  │    Status: Ready                                                     │ │
│  │                                                                      │ │
│  │  CompileOS> _                                                        │ │ ← Input Prompt
│  └──────────────────────────────────────────────────────────────────────┘ │
├────────────────────────────────────────────────────────────────────────────┤
│ F1-Help | F2-Files | F3-REPL | F4-Memory | F5-Debug | F10-Exit  Status: Ready │ ← Status Bar
└────────────────────────────────────────────────────────────────────────────┘
```

---

## Features

### 1. Window Management

**Main Terminal Window**
- Position: (1, 1)
- Size: 78x21 characters
- Features:
  - Colored border (yellow when focused)
  - Title bar with window name
  - Content area with welcome message
  - Interactive terminal prompt
  - Scrollable content (future)

**Status Bar**
- Position: Bottom of screen (0, 23)
- Size: 80x2 characters
- Features:
  - Function key shortcuts
  - System status indicator
  - Always visible

### 2. Color Scheme

| Element | Foreground | Background |
|---------|-----------|------------|
| Desktop Background | Cyan | Blue |
| Title Bar | White | Blue |
| Window Border (Focused) | Yellow | Cyan |
| Window Border (Unfocused) | White | Blue |
| Window Content | White | Black |
| Status Bar | Black | Light Gray |
| Welcome Text | Light Cyan | Black |
| Feature List | Light Green | Black |
| System Info | Light Gray | Black |
| Prompt | Light Cyan | Black |

### 3. Keyboard Integration

The desktop automatically handles keyboard input:
- All keypresses are captured by the keyboard driver
- Input is routed to the focused window
- Terminal commands are processed
- Desktop refreshes after each input

---

## API Functions

### Desktop Control

```c
// Initialize desktop (call once at startup)
int desktop_init(void);

// Start desktop (displays windows)
int desktop_start(void);

// Update desktop (call in main loop)
void desktop_update(void);

// Stop desktop
void desktop_stop(void);

// Check if running
bool desktop_is_running(void);
```

### Window Management

```c
// Create a new window
window_t* desktop_create_window(const char* title, 
                                uint16_t x, uint16_t y, 
                                uint16_t width, uint16_t height);

// Destroy window
int desktop_destroy_window(window_t* window);

// Show/hide window
int desktop_show_window(window_t* window);
int desktop_hide_window(window_t* window);

// Focus window
int desktop_focus_window(window_t* window);

// Move/resize window
int desktop_move_window(window_t* window, uint16_t x, uint16_t y);
int desktop_resize_window(window_t* window, uint16_t width, uint16_t height);
```

### Rendering

```c
// Render entire desktop
int desktop_render(void);

// Render specific components
int desktop_draw_background(void);
int desktop_draw_windows(void);
int desktop_draw_cursor(void);
```

---

## Integration with Kernel

### In kernel_main.c:

```c
#include "kernel/desktop/desktop.h"

// In kernel_init()
if (desktop_init() != 0) {
    return -1;
}

// After all initialization
if (desktop_start() != 0) {
    return -1;
}

// In main loop
while (g_kernel_state.status == KERNEL_STATUS_RUNNING) {
    // Update desktop (handles keyboard input)
    desktop_update();
    
    // Yield CPU
    hal_halt();
}
```

---

## Customization

### Change Desktop Colors

Edit `desktop_draw_background()` in `desktop.c`:

```c
// Change background color
vga_set_foreground_color(VGA_COLOR_GREEN);  // Your choice
vga_set_background_color(VGA_COLOR_BLACK);  // Your choice
```

### Change Window Layout

Edit `desktop_start()` in `desktop.c`:

```c
// Create custom window
desktop_create_window("My Window", 10, 5, 60, 15);
```

### Add More Windows

```c
// In desktop_start() or anywhere after desktop_start()
window_t* file_window = desktop_create_window("File Manager", 5, 5, 70, 18);
window_t* debug_window = desktop_create_window("Debugger", 5, 10, 70, 12);
```

### Customize Status Bar

Edit the status bar section in `desktop_draw_windows()`:

```c
sprintf(status, " Your custom status bar text here ");
vga_puts_at(window->x + 1, window->y + 1, status);
```

---

## Window IDs

| ID | Window | Purpose |
|----|--------|---------|
| 1 | Main Terminal | Interactive shell and command input |
| 2 | Status Bar | Function keys and system status |

---

## Future Enhancements

### Planned Features
1. **Multiple terminal windows** - Open several terminals
2. **Window switching** - Alt+Tab to switch windows
3. **Window minimize/maximize** - Manage window states
4. **Drag and drop** - Move windows with mouse
5. **Context menus** - Right-click for options
6. **Taskbar** - Show running applications
7. **Desktop icons** - Launch apps from desktop
8. **Themes** - Customizable color schemes

### How to Add a New Window Type

```c
// 1. Define window ID
#define WINDOW_ID_MY_APP 3

// 2. Create window in desktop_start()
desktop_create_window("My Application", 10, 5, 60, 15);

// 3. Add rendering in desktop_draw_windows()
if (window->id == 3) { // My application window
    vga_set_foreground_color(VGA_COLOR_WHITE);
    vga_puts_at(window->x + 2, window->y + 2, "My App Content");
    // Draw your content here
}
```

---

## Troubleshooting

### Desktop not showing
- Check that `desktop_init()` is called before `desktop_start()`
- Verify VGA is initialized
- Ensure `desktop_render()` is called

### Keyboard input not working
- Verify `desktop_update()` is called in main loop
- Check that keyboard driver is initialized
- Ensure IRQ 1 is enabled

### Windows not displaying correctly
- Check window coordinates are within screen bounds (80x25)
- Verify window width/height are reasonable
- Ensure VGA colors are set correctly

### Cursor not visible
- Check `desktop_draw_cursor()` is called
- Verify cursor position is within window bounds
- Ensure VGA cursor is enabled

---

## Performance Tips

1. **Minimize redraws** - Only call `desktop_render()` when needed
2. **Use dirty regions** - Track which areas need updating
3. **Buffer content** - Cache window content to avoid recalculation
4. **Optimize loops** - Reduce nested loops in drawing functions

---

## Example: Creating a Custom Application Window

```c
// In your application code
void my_app_init(void) {
    // Create application window
    window_t* app_window = desktop_create_window(
        "My Application", 
        10,   // x position
        5,    // y position
        60,   // width
        15    // height
    );
    
    if (app_window) {
        // Focus the window
        desktop_focus_window(app_window);
        
        // Render desktop
        desktop_render();
    }
}

// Add custom rendering in desktop_draw_windows()
if (window->id == YOUR_WINDOW_ID) {
    // Clear content area
    for (uint16_t y = window->y + 1; y < window->y + window->height - 1; y++) {
        for (uint16_t x = window->x + 1; x < window->x + window->width - 1; x++) {
            vga_putchar_at(x, y, ' ');
        }
    }
    
    // Draw your content
    vga_set_foreground_color(VGA_COLOR_WHITE);
    vga_puts_at(window->x + 2, window->y + 2, "Hello from my app!");
}
```

---

## Summary

CompileOS Desktop provides:
- ✅ **Professional appearance** - Windowed interface with borders
- ✅ **User-friendly** - Clear visual feedback and status information
- ✅ **Extensible** - Easy to add new windows and features
- ✅ **Integrated** - Works seamlessly with keyboard and terminal
- ✅ **Customizable** - Colors, layout, and content are configurable

**The desktop is now ready for interactive use!** 🖥️✨

Users will see a polished graphical environment when CompileOS boots, making it feel like a real operating system with a proper desktop interface!
