/**
 * CompileOS Mouse Driver
 * PS/2 Mouse support with cursor tracking and click detection
 */

#ifndef MOUSE_H
#define MOUSE_H

#include <stdint.h>
#include <stdbool.h>

// Mouse constants
#define MOUSE_PORT_DATA     0x60
#define MOUSE_PORT_STATUS   0x64
#define MOUSE_PORT_COMMAND  0x64

#define MOUSE_CMD_RESET     0xFF
#define MOUSE_CMD_RESEND    0xFE
#define MOUSE_CMD_SET_DEFAULT 0xF6
#define MOUSE_CMD_DISABLE   0xF5
#define MOUSE_CMD_ENABLE    0xF4
#define MOUSE_CMD_SET_SAMPLE_RATE 0xF3
#define MOUSE_CMD_GET_DEVICE_ID 0xF2
#define MOUSE_CMD_SET_REMOTE_MODE 0xF0
#define MOUSE_CMD_SET_WRAP_MODE 0xEE
#define MOUSE_CMD_RESET_WRAP_MODE 0xEC
#define MOUSE_CMD_SET_STREAM_MODE 0xEA
#define MOUSE_CMD_SET_REPORT_RATE 0xC8
#define MOUSE_CMD_GET_STATUS 0xE9
#define MOUSE_CMD_SET_RESOLUTION 0xE8

// Mouse packet structure
typedef struct {
    uint8_t flags;
    int8_t dx;
    int8_t dy;
} __attribute__((packed)) mouse_packet_t;

// Mouse state
typedef struct {
    int x, y;           // Current mouse position
    int prev_x, prev_y; // Previous mouse position
    bool left_button;
    bool right_button;
    bool middle_button;
    bool left_pressed;
    bool right_pressed;
    bool middle_pressed;
    uint8_t cycle;      // For 3-byte packet reading
    uint8_t packet[3];  // Current packet being read
} mouse_state_t;

// Mouse functions
int mouse_init(void);
void mouse_update(void);
int mouse_get_x(void);
int mouse_get_y(void);
bool mouse_left_pressed(void);
bool mouse_right_pressed(void);
bool mouse_middle_pressed(void);
void mouse_get_movement(int* dx, int* dy);

#endif // MOUSE_H
