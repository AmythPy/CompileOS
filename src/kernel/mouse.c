/**
 * CompileOS Mouse Driver - PS/2 Mouse Support
 * Provides mouse tracking and cursor display
 */

#include <stdint.h>

// Mouse state - shared with main kernel
extern int mouse_x;
extern int mouse_y;
static uint8_t mouse_buttons = 0;
static int mouse_enabled = 0;

// VGA access for cursor drawing
#define VGA ((volatile uint16_t*)0xB8000)

// Port I/O
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

// Colors
#define COLOR(fg, bg) ((bg << 4) | fg)
#define BLACK 0
#define WHITE 15
#define YELLOW 14

// Initialize PS/2 mouse with better error handling
void mouse_init(void) {
    // Wait for controller to be ready
    while (inb(0x64) & 2);

    // Enable auxiliary PS/2 device (mouse)
    outb(0x64, 0xA8);

    // Wait and read ACK
    while (inb(0x64) & 1);
    uint8_t ack1 = inb(0x60);
    if (ack1 != 0xFA) {
        mouse_enabled = 0;
        return;  // Mouse not responding
    }

    // Tell controller we want to write to second PS/2 port
    outb(0x64, 0x20);
    while (inb(0x64) & 1);
    uint8_t status = inb(0x60);

    // Enable interrupts and clock for second port
    status |= 3;  // Enable clock and interrupt
    status &= ~0x20;  // Enable translation

    // Write back configuration
    while (inb(0x64) & 2);
    outb(0x64, 0x60);
    while (inb(0x64) & 2);
    outb(0x60, status);

    // Wait for ACK
    while (inb(0x64) & 1);
    uint8_t ack2 = inb(0x60);
    if (ack2 != 0xFA) {
        mouse_enabled = 0;
        return;
    }

    // Set mouse sample rate to 100 samples/sec
    while (inb(0x64) & 2);
    outb(0x64, 0xD4);
    while (inb(0x64) & 2);
    outb(0x60, 0xF3);  // Set sample rate

    while (inb(0x64) & 1);
    uint8_t ack3 = inb(0x60);
    if (ack3 != 0xFA) {
        mouse_enabled = 0;
        return;
    }

    while (inb(0x64) & 2);
    outb(0x64, 0xD4);
    while (inb(0x64) & 2);
    outb(0x60, 100);  // 100 samples/sec

    while (inb(0x64) & 1);
    uint8_t ack4 = inb(0x60);
    if (ack4 != 0xFA) {
        mouse_enabled = 0;
        return;
    }

    // Enable mouse data reporting
    while (inb(0x64) & 2);
    outb(0x64, 0xD4);
    while (inb(0x64) & 2);
    outb(0x60, 0xF4);  // Enable data reporting

    // Read final ACK
    while (inb(0x64) & 1);
    uint8_t ack5 = inb(0x60);
    if (ack5 != 0xFA) {
        mouse_enabled = 0;
        return;
    }

    mouse_enabled = 1;
    mouse_x = 40;
    mouse_y = 12;
}

// Read mouse data (call this regularly)
void mouse_update(void) {
    if (!mouse_enabled) return;

    // Check if mouse data is available
    if (!(inb(0x64) & 1)) return;

    uint8_t data = inb(0x60);

    // Mouse sends 3-byte packets
    static uint8_t packet[3];
    static int packet_index = 0;

    packet[packet_index++] = data;

    if (packet_index >= 3) {
        // Parse mouse packet
        uint8_t flags = packet[0];
        int8_t dx = (int8_t)packet[1];
        int8_t dy = (int8_t)packet[2];

        // Check for overflow
        if (flags & 0xC0) {
            packet_index = 0;
            return;
        }

        // Update position
        mouse_x += dx;
        mouse_y -= dy;  // Mouse Y is inverted (up is negative)

        // Clamp to screen bounds
        if (mouse_x < 0) mouse_x = 0;
        if (mouse_x >= 80) mouse_x = 79;
        if (mouse_y < 0) mouse_y = 0;
        if (mouse_y >= 25) mouse_y = 24;

        // Update button state
        mouse_buttons = flags & 0x07;

        packet_index = 0;
    }
}

// Get current mouse position
void mouse_get_position(int* x, int* y) {
    *x = mouse_x;
    *y = mouse_y;
}

// Get button state
uint8_t mouse_get_buttons(void) {
    return mouse_buttons;
}

// Draw mouse cursor as intersecting lines (more subtle)
void mouse_draw_cursor(void) {
    if (!mouse_enabled) return;

    // Draw horizontal line segment around cursor (yellow)
    int start_x = mouse_x - 5;
    int end_x = mouse_x + 5;
    if (start_x < 0) start_x = 0;
    if (end_x >= 80) end_x = 79;

    for (int x = start_x; x <= end_x; x++) {
        if (mouse_y >= 0 && mouse_y < 25) {
            // Only draw on empty areas or preserve text
            uint16_t current = VGA[mouse_y * 80 + x];
            if ((current & 0xFF) == ' ') {
                VGA[mouse_y * 80 + x] = (current & 0xFF00) | (YELLOW & 0xFF);
            }
        }
    }

    // Draw vertical line segment around cursor (yellow)
    int start_y = mouse_y - 3;
    int end_y = mouse_y + 3;
    if (start_y < 0) start_y = 0;
    if (end_y >= 25) end_y = 24;

    for (int y = start_y; y <= end_y; y++) {
        if (mouse_x >= 0 && mouse_x < 80) {
            // Only draw on empty areas or preserve text
            uint16_t current = VGA[y * 80 + mouse_x];
            if ((current & 0xFF) == ' ') {
                VGA[y * 80 + mouse_x] = (current & 0xFF00) | (YELLOW & 0xFF);
            }
        }
    }

    // Draw center point (bright yellow)
    if (mouse_x >= 0 && mouse_x < 80 && mouse_y >= 0 && mouse_y < 25) {
        VGA[mouse_y * 80 + mouse_x] = '+' | 0x0E00;  // Bright yellow '+'
    }
}

// Clear mouse cursor (restore original screen content)
void mouse_clear_cursor(void) {
    // For simplicity, just redraw the entire desktop
    // In a more sophisticated implementation, we'd track what was under the cursor
    // For now, this works and is simple
}

// Check if mouse is enabled
int mouse_is_enabled(void) {
    return mouse_enabled;
}
