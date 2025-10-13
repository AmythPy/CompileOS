/**
 * CompileOS PS/2 Keyboard Driver - Header
 * 
 * Full keyboard driver with scancode translation and input buffering
 */

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// Keyboard status flags
typedef struct {
    bool shift_pressed;
    bool ctrl_pressed;
    bool alt_pressed;
    bool caps_lock;
    bool num_lock;
    bool scroll_lock;
} keyboard_state_t;

// Key event types
typedef enum {
    KEY_EVENT_PRESS = 0,
    KEY_EVENT_RELEASE
} key_event_type_t;

// Key event structure
typedef struct {
    uint8_t scancode;
    char ascii;
    key_event_type_t type;
    bool shift;
    bool ctrl;
    bool alt;
} key_event_t;

// Keyboard callback function type
typedef void (*keyboard_callback_t)(key_event_t* event);

// Keyboard initialization and control
int keyboard_init(void);
void keyboard_shutdown(void);
bool keyboard_is_initialized(void);

// Keyboard state
keyboard_state_t keyboard_get_state(void);
bool keyboard_is_shift_pressed(void);
bool keyboard_is_ctrl_pressed(void);
bool keyboard_is_alt_pressed(void);

// Keyboard input
char keyboard_getchar(void);
int keyboard_getchar_nonblocking(char* c);
bool keyboard_has_input(void);

// Keyboard buffer
int keyboard_buffer_read(char* buffer, size_t size);
void keyboard_buffer_clear(void);
size_t keyboard_buffer_available(void);

// Keyboard callbacks
int keyboard_register_callback(keyboard_callback_t callback);
int keyboard_unregister_callback(void);

// IRQ handler (called by interrupt system)
void keyboard_irq_handler(void);

// Scancode to ASCII conversion
char keyboard_scancode_to_ascii(uint8_t scancode, bool shift);

#endif // KEYBOARD_H
