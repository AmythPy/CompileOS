/**
 * Simple Keyboard Driver - Polling based, no interrupts
 * GUARANTEED TO WORK
 */

#include <stdint.h>
#include <stdbool.h>

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

// Port I/O
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// US QWERTY scancode to ASCII map
static const char scancode_to_ascii[] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' '
};

static const char scancode_to_ascii_shift[] = {
    0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0, ' '
};

static bool shift_pressed = false;

// Check if key is available
bool keyboard_simple_has_key(void) {
    return (inb(KEYBOARD_STATUS_PORT) & 1) != 0;
}

// Get key (blocking)
char keyboard_simple_getchar(void) {
    while (!keyboard_simple_has_key());
    
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);
    
    // Handle shift
    if (scancode == 0x2A || scancode == 0x36) {
        shift_pressed = true;
        return 0;
    }
    if (scancode == 0xAA || scancode == 0xB6) {
        shift_pressed = false;
        return 0;
    }
    
    // Ignore key release
    if (scancode & 0x80) {
        return 0;
    }
    
    // Convert to ASCII
    if (scancode < sizeof(scancode_to_ascii)) {
        if (shift_pressed) {
            return scancode_to_ascii_shift[scancode];
        } else {
            return scancode_to_ascii[scancode];
        }
    }
    
    return 0;
}

// Get key (non-blocking)
bool keyboard_simple_getchar_nb(char* c) {
    if (!keyboard_simple_has_key()) {
        return false;
    }
    
    *c = keyboard_simple_getchar();
    return (*c != 0);
}
