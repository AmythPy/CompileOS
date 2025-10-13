/**
 * CompileOS Boot Splash Screen
 * Beautiful boot screen with progress display and animations
 */

#ifndef SPLASH_H
#define SPLASH_H

#include <stdint.h>
#include <stdbool.h>

// Boot sequence phases
typedef enum {
    SPLASH_PHASE_LOGO_FADE_IN = 0,
    SPLASH_PHASE_SYSTEM_INIT,
    SPLASH_PHASE_HARDWARE_DETECT,
    SPLASH_PHASE_DRIVERS_LOAD,
    SPLASH_PHASE_SERVICES_START,
    SPLASH_PHASE_DESKTOP_LOAD,
    SPLASH_PHASE_COMPLETE
} splash_phase_t;

// Boot animation types
typedef enum {
    SPLASH_ANIM_NONE = 0,
    SPLASH_ANIM_FADE_IN,
    SPLASH_ANIM_FADE_OUT,
    SPLASH_ANIM_SLIDE_UP,
    SPLASH_ANIM_SLIDE_DOWN,
    SPLASH_ANIM_TYPEWRITER,
    SPLASH_ANIM_PROGRESS_BAR,
    SPLASH_ANIM_PULSE,
    SPLASH_ANIM_ROTATE
} splash_animation_t;

// Boot configuration
typedef struct {
    uint32_t screen_width;
    uint32_t screen_height;
    uint32_t logo_x, logo_y;
    uint32_t logo_width, logo_height;
    uint32_t progress_bar_x, progress_bar_y;
    uint32_t progress_bar_width, progress_bar_height;
    uint32_t text_x, text_y;
    uint32_t bg_color;
    uint32_t fg_color;
    uint32_t accent_color;
    char logo_text[64];
    char subtitle[128];
    bool show_progress;
    bool show_animations;
    uint32_t animation_speed;
} splash_config_t;

// Boot sequence functions
int splash_init(void);
int splash_run_boot_sequence(void);
void splash_set_phase(splash_phase_t phase);
splash_phase_t splash_get_phase(void);

// Animation functions
void splash_animate_fade_in(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color, uint32_t frames);
void splash_animate_typewriter(const char* text, uint32_t x, uint32_t y, uint32_t color, uint32_t speed);
void splash_draw_progress_bar(uint32_t x, uint32_t y, uint32_t width, uint32_t height, float progress, uint32_t bg_color, uint32_t fg_color);
void splash_draw_logo(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const char* text, uint32_t color);

// Professional boot messages
void splash_show_message(const char* message);
void splash_show_submessage(const char* submessage);
void splash_update_progress(float progress);

// Boot effects
void splash_draw_gradient_background(uint32_t color1, uint32_t color2);
void splash_draw_particles(uint32_t count);
void splash_draw_loading_spinner(uint32_t x, uint32_t y, uint32_t radius, uint32_t color);

// Hero-level boot sequence
int splash_run_hero_boot_sequence(void);
void splash_hero_logo_animation(void);
void splash_hero_progress_display(void);
void splash_hero_completion_effect(void);

// Boot configuration
void splash_set_config(const splash_config_t* config);
splash_config_t* splash_get_config(void);

#endif // SPLASH_H
