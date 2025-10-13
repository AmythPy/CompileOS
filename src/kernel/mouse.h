/**
 * CompileOS Mouse Driver - Header
 * PS/2 mouse support for cursor tracking
 */

#ifndef MOUSE_H
#define MOUSE_H

#include <stdint.h>

// Mouse initialization and control
void mouse_init(void);
void mouse_update(void);
int mouse_is_enabled(void);

// Mouse position and state
void mouse_get_position(int* x, int* y);
uint8_t mouse_get_buttons(void);

// Mouse cursor display
void mouse_draw_cursor(void);
void mouse_clear_cursor(void);

#endif
