/**
 * CompileOS PS/2 Keyboard Driver - Implementation
 * 
 * PS/2 keyboard driver with IRQ1 handling
 */

#include "keyboard.h"
#include "../io/io.h"
#include <string.h>

// PS/2 keyboard ports
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define KEYBOARD_COMMAND_PORT 0x64

// Keyboard buffer size
#define KEYBOARD_BUFFER_SIZE 256

// Scancode definitions
#define SCANCODE_LSHIFT 0x2A
#define SCANCODE_RSHIFT 0x36
#define SCANCODE_LCTRL 0x1D
#define SCANCODE_LALT 0x38
#define SCANCODE_CAPS_LOCK 0x3A
#define SCANCODE_NUM_LOCK 0x45
#define SCANCODE_SCROLL_LOCK 0x46

// Keyboard state
static struct {
    bool initialized;
    keyboard_state_t state;
    keyboard_callback_t callback;
    char buffer[KEYBOARD_BUFFER_SIZE];
    size_t buffer_head;
    size_t buffer_tail;
    size_t buffer_count;
} g_keyboard = {0};

// US QWERTY scancode to ASCII table (lowercase)
static const char scancode_to_ascii_lower[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    '-', 0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// US QWERTY scancode to ASCII table (uppercase/shift)
static const char scancode_to_ascii_upper[128] = {
    0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    '-', 0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/**
 * Initialize keyboard driver
 */
int keyboard_init(void) {
    if (g_keyboard.initialized) {
        return 0;
    }
    
    // Clear keyboard state
    memset(&g_keyboard.state, 0, sizeof(keyboard_state_t));
    g_keyboard.callback = NULL;
    g_keyboard.buffer_head = 0;
    g_keyboard.buffer_tail = 0;
    g_keyboard.buffer_count = 0;
    
    // Enable keyboard (clear bit 4 of status register)
    uint8_t status = io_inb(KEYBOARD_COMMAND_PORT);
    status &= ~0x10;
    io_outb(KEYBOARD_COMMAND_PORT, status);
    
    g_keyboard.initialized = true;
    return 0;
}

/**
 * Shutdown keyboard driver
 */
void keyboard_shutdown(void) {
    g_keyboard.initialized = false;
}

/**
 * Check if keyboard is initialized
 */
bool keyboard_is_initialized(void) {
    return g_keyboard.initialized;
}

/**
 * Get keyboard state
 */
keyboard_state_t keyboard_get_state(void) {
    return g_keyboard.state;
}

/**
 * Check if shift is pressed
 */
bool keyboard_is_shift_pressed(void) {
    return g_keyboard.state.shift_pressed;
}

/**
 * Check if ctrl is pressed
 */
bool keyboard_is_ctrl_pressed(void) {
    return g_keyboard.state.ctrl_pressed;
}

/**
 * Check if alt is pressed
 */
bool keyboard_is_alt_pressed(void) {
    return g_keyboard.state.alt_pressed;
}

/**
 * Convert scancode to ASCII
 */
char keyboard_scancode_to_ascii(uint8_t scancode, bool shift) {
    if (scancode >= 128) {
        return 0;
    }
    
    if (shift || g_keyboard.state.caps_lock) {
        return scancode_to_ascii_upper[scancode];
    } else {
        return scancode_to_ascii_lower[scancode];
    }
}

/**
 * Add character to keyboard buffer
 */
static void keyboard_buffer_add(char c) {
    if (g_keyboard.buffer_count >= KEYBOARD_BUFFER_SIZE) {
        return; // Buffer full
    }
    
    g_keyboard.buffer[g_keyboard.buffer_head] = c;
    g_keyboard.buffer_head = (g_keyboard.buffer_head + 1) % KEYBOARD_BUFFER_SIZE;
    g_keyboard.buffer_count++;
}

/**
 * Get character from keyboard buffer
 */
static char keyboard_buffer_get(void) {
    if (g_keyboard.buffer_count == 0) {
        return 0;
    }
    
    char c = g_keyboard.buffer[g_keyboard.buffer_tail];
    g_keyboard.buffer_tail = (g_keyboard.buffer_tail + 1) % KEYBOARD_BUFFER_SIZE;
    g_keyboard.buffer_count--;
    
    return c;
}

/**
 * Keyboard IRQ handler (called from interrupt system)
 */
void keyboard_irq_handler(void) {
    if (!g_keyboard.initialized) {
        return;
    }
    
    // Read scancode from keyboard
    uint8_t scancode = io_inb(KEYBOARD_DATA_PORT);
    
    // Check if this is a key release (bit 7 set)
    bool is_release = (scancode & 0x80) != 0;
    uint8_t key = scancode & 0x7F;
    
    // Update modifier key states
    if (key == SCANCODE_LSHIFT || key == SCANCODE_RSHIFT) {
        g_keyboard.state.shift_pressed = !is_release;
    } else if (key == SCANCODE_LCTRL) {
        g_keyboard.state.ctrl_pressed = !is_release;
    } else if (key == SCANCODE_LALT) {
        g_keyboard.state.alt_pressed = !is_release;
    } else if (key == SCANCODE_CAPS_LOCK && !is_release) {
        g_keyboard.state.caps_lock = !g_keyboard.state.caps_lock;
    } else if (key == SCANCODE_NUM_LOCK && !is_release) {
        g_keyboard.state.num_lock = !g_keyboard.state.num_lock;
    } else if (key == SCANCODE_SCROLL_LOCK && !is_release) {
        g_keyboard.state.scroll_lock = !g_keyboard.state.scroll_lock;
    }
    
    // Only process key presses (not releases) for ASCII conversion
    if (!is_release) {
        char ascii = keyboard_scancode_to_ascii(key, g_keyboard.state.shift_pressed);
        
        if (ascii != 0) {
            // Add to buffer
            keyboard_buffer_add(ascii);
            
            // Call callback if registered
            if (g_keyboard.callback) {
                key_event_t event;
                event.scancode = scancode;
                event.ascii = ascii;
                event.type = KEY_EVENT_PRESS;
                event.shift = g_keyboard.state.shift_pressed;
                event.ctrl = g_keyboard.state.ctrl_pressed;
                event.alt = g_keyboard.state.alt_pressed;
                g_keyboard.callback(&event);
            }
        }
    }
}

/**
 * Get character (blocking)
 */
char keyboard_getchar(void) {
    while (g_keyboard.buffer_count == 0) {
        // Wait for input
        __asm__ volatile ("hlt");
    }
    return keyboard_buffer_get();
}

/**
 * Get character (non-blocking)
 */
int keyboard_getchar_nonblocking(char* c) {
    if (!c || g_keyboard.buffer_count == 0) {
        return -1;
    }
    
    *c = keyboard_buffer_get();
    return 0;
}

/**
 * Check if keyboard has input
 */
bool keyboard_has_input(void) {
    return g_keyboard.buffer_count > 0;
}

/**
 * Read from keyboard buffer
 */
int keyboard_buffer_read(char* buffer, size_t size) {
    if (!buffer || size == 0) {
        return -1;
    }
    
    size_t read = 0;
    while (read < size && g_keyboard.buffer_count > 0) {
        buffer[read++] = keyboard_buffer_get();
    }
    
    return read;
}

/**
 * Clear keyboard buffer
 */
void keyboard_buffer_clear(void) {
    g_keyboard.buffer_head = 0;
    g_keyboard.buffer_tail = 0;
    g_keyboard.buffer_count = 0;
}

/**
 * Get available characters in buffer
 */
size_t keyboard_buffer_available(void) {
    return g_keyboard.buffer_count;
}

/**
 * Register keyboard callback
 */
int keyboard_register_callback(keyboard_callback_t callback) {
    if (!callback) {
        return -1;
    }
    
    g_keyboard.callback = callback;
    return 0;
}

/**
 * Unregister keyboard callback
 */
int keyboard_unregister_callback(void) {
    g_keyboard.callback = NULL;
    return 0;
}
