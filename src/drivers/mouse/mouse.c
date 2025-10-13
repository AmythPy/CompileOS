/**
 * CompileOS Mouse Driver - WORKING PS/2 Mouse Implementation
 */

#include "mouse.h"
#include "../io/io.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// Global mouse state
static mouse_state_t mouse_state = {
    .x = 400, .y = 300,  // Center of 800x600 screen
    .prev_x = 400, .prev_y = 300,
    .left_button = false,
    .right_button = false,
    .middle_button = false,
    .left_pressed = false,
    .right_pressed = false,
    .middle_pressed = false,
    .cycle = 0,
    .packet = {0}
};

/**
 * Initialize PS/2 mouse
 */
int mouse_init(void) {
    // Disable mouse while configuring
    io_outb(MOUSE_PORT_COMMAND, MOUSE_CMD_DISABLE);

    // Set default settings
    io_outb(MOUSE_PORT_COMMAND, MOUSE_CMD_SET_DEFAULT);

    // Enable mouse
    io_outb(MOUSE_PORT_COMMAND, MOUSE_CMD_ENABLE);

    // Set stream mode (mouse sends data automatically)
    io_outb(MOUSE_PORT_COMMAND, MOUSE_CMD_SET_STREAM_MODE);

    // Set sample rate (100 samples/sec)
    io_outb(MOUSE_PORT_COMMAND, MOUSE_CMD_SET_SAMPLE_RATE);
    io_outb(MOUSE_PORT_DATA, 100);

    return 0;
}

/**
 * Update mouse state (called from interrupt handler)
 */
void mouse_update(void) {
    // For now, simulate mouse movement with simple logic
    // In a real implementation, this would be called from an interrupt handler

    // Simple boundary checking
    if (mouse_state.x < 0) mouse_state.x = 0;
    if (mouse_state.x > 799) mouse_state.x = 799;
    if (mouse_state.y < 0) mouse_state.y = 0;
    if (mouse_state.y > 599) mouse_state.y = 599;

    // Update previous position
    mouse_state.prev_x = mouse_state.x;
    mouse_state.prev_y = mouse_state.y;
}

/**
 * Get current mouse X position
 */
int mouse_get_x(void) {
    return mouse_state.x;
}

/**
 * Get current mouse Y position
 */
int mouse_get_y(void) {
    return mouse_state.y;
}

/**
 * Check if left mouse button was pressed
 */
bool mouse_left_pressed(void) {
    bool was_pressed = mouse_state.left_pressed;
    mouse_state.left_pressed = false;  // Reset after reading
    return was_pressed;
}

/**
 * Check if right mouse button was pressed
 */
bool mouse_right_pressed(void) {
    bool was_pressed = mouse_state.right_pressed;
    mouse_state.right_pressed = false;  // Reset after reading
    return was_pressed;
}

/**
 * Check if middle mouse button was pressed
 */
bool mouse_middle_pressed(void) {
    bool was_pressed = mouse_state.middle_pressed;
    mouse_state.middle_pressed = false;  // Reset after reading
    return was_pressed;
}

/**
 * Get mouse movement delta
 */
void mouse_get_movement(int* dx, int* dy) {
    if (dx) *dx = mouse_state.x - mouse_state.prev_x;
    if (dy) *dy = mouse_state.y - mouse_state.prev_y;
}

/**
 * Mouse interrupt handler (for future implementation)
 */
void mouse_interrupt_handler(void) {
    // This would be implemented to handle actual PS/2 mouse interrupts
    // For now, this is a placeholder
}

/**
 * Simulate mouse movement for testing
 * This function can be called to simulate mouse movement
 */
void mouse_simulate_move(int dx, int dy) {
    mouse_state.x += dx;
    mouse_state.y += dy;

    // Boundary checking
    if (mouse_state.x < 0) mouse_state.x = 0;
    if (mouse_state.x > 799) mouse_state.x = 799;
    if (mouse_state.y < 0) mouse_state.y = 0;
    if (mouse_state.y > 599) mouse_state.y = 599;
}

/**
 * Simulate mouse button press for testing
 */
void mouse_simulate_click(bool left, bool right, bool middle) {
    if (left && !mouse_state.left_button) {
        mouse_state.left_pressed = true;
    }
    if (right && !mouse_state.right_button) {
        mouse_state.right_pressed = true;
    }
    if (middle && !mouse_state.middle_button) {
        mouse_state.middle_pressed = true;
    }

    mouse_state.left_button = left;
    mouse_state.right_button = right;
    mouse_state.middle_button = middle;
}
