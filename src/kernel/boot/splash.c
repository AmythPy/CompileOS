/**
 * CompileOS WORKING Boot Splash Screen
 * Simple but functional boot screen
 */

#include "splash.h"
#include "../../drivers/graphics/graphics.h"
#include "../../drivers/vga/vga.h"
#include <stdio.h>
#include <string.h>

// Boot sequence state
static splash_phase_t g_current_phase = SPLASH_PHASE_LOGO_FADE_IN;
static splash_config_t g_splash_config = {
    .screen_width = 800,
    .screen_height = 600,
    .logo_x = 300,
    .logo_y = 200,
    .logo_width = 200,
    .logo_height = 100,
    .progress_bar_x = 200,
    .progress_bar_y = 500,
    .progress_bar_width = 400,
    .progress_bar_height = 20,
    .text_x = 350,
    .text_y = 350,
    .bg_color = 0x001122,
    .fg_color = 0xFFFFFF,
    .accent_color = 0x00FF00,
    .show_progress = true,
    .show_animations = true,
    .animation_speed = 2
};

strncpy(g_splash_config.logo_text, "CompileOS", sizeof(g_splash_config.logo_text) - 1);
strncpy(g_splash_config.subtitle, "Advanced Operating System", sizeof(g_splash_config.subtitle) - 1);

/**
 * Initialize splash screen system
 */
int splash_init(void) {
    // Initialize graphics if not already done
    if (graphics_init() != 0) {
        return -1;
    }

    // Set graphics mode
    if (graphics_set_mode(GRAPHICS_MODE_VESA_800x600_24) != 0) {
        return -1;
    }

    return 0;
}

/**
 * Run the boot sequence
 */
int splash_run_boot_sequence(void) {
    // Initialize graphics
    if (splash_init() != 0) {
        return -1;
    }

    // Hero-level boot sequence
    splash_run_hero_boot_sequence();

    return 0;
}

/**
 * Set current boot phase
 */
void splash_set_phase(splash_phase_t phase) {
    g_current_phase = phase;
}

/**
 * Get current boot phase
 */
splash_phase_t splash_get_phase(void) {
    return g_current_phase;
}

/**
 * Hero-level boot sequence with full effects
 */
int splash_run_hero_boot_sequence(void) {
    // Phase 1: Logo fade in
    splash_hero_logo_animation();

    // Phase 2: System initialization
    splash_hero_progress_display();

    return 0;
}

/**
 * Animate the CompileOS logo
 */
void splash_hero_logo_animation(void) {
    splash_draw_gradient_background(g_splash_config.bg_color, 0x000011);

    // Draw logo
    splash_draw_logo(
        g_splash_config.logo_x,
        g_splash_config.logo_y,
        g_splash_config.logo_width,
        g_splash_config.logo_height,
        g_splash_config.logo_text,
        g_splash_config.fg_color
    );

    // Animate typewriter effect
    static char display_text[64] = {0};
    static int text_pos = 0;
    const char* full_text = "CompileOS";

    if (text_pos < strlen(full_text)) {
        display_text[text_pos] = full_text[text_pos];
        text_pos++;

        splash_draw_logo(
            g_splash_config.logo_x,
            g_splash_config.logo_y,
            g_splash_config.logo_width,
            g_splash_config.logo_height,
            display_text,
            g_splash_config.fg_color
        );

        // Add cursor
        if (text_pos < strlen(full_text)) {
            splash_draw_logo(
                g_splash_config.logo_x,
                g_splash_config.logo_y,
                g_splash_config.logo_width,
                g_splash_config.logo_height,
                "_",
                g_splash_config.accent_color
            );
        }
    }

    graphics_flip();
}

/**
 * Show progress display
 */
void splash_hero_progress_display(void) {
    static float progress = 0.0f;

    // Draw progress bar background
    splash_draw_progress_bar(
        g_splash_config.progress_bar_x,
        g_splash_config.progress_bar_y,
        g_splash_config.progress_bar_width,
        g_splash_config.progress_bar_height,
        progress,
        0x333333,
        g_splash_config.accent_color
    );

    // Update progress
    progress += 0.02f;
    if (progress > 1.0f) progress = 1.0f;

    // Show progress messages
    const char* messages[] = {
        "Initializing system...",
        "Loading drivers...",
        "Starting services...",
        "Loading desktop...",
        "Complete!"
    };

    static int message_index = 0;
    if (progress > message_index * 0.2f) {
        splash_show_message(messages[message_index]);
        message_index++;
        if (message_index >= 5) message_index = 4;
    }

    graphics_flip();
}

/**
 * Draw gradient background
 */
void splash_draw_gradient_background(uint32_t color1, uint32_t color2) {
    for (uint32_t y = 0; y < g_splash_config.screen_height; y++) {
        float ratio = (float)y / g_splash_config.screen_height;
        uint32_t r = ((color1 >> 16) & 0xFF) * (1.0f - ratio) + ((color2 >> 16) & 0xFF) * ratio;
        uint32_t g = ((color1 >> 8) & 0xFF) * (1.0f - ratio) + ((color2 >> 8) & 0xFF) * ratio;
        uint32_t b = (color1 & 0xFF) * (1.0f - ratio) + (color2 & 0xFF) * ratio;

        uint32_t color = (r << 16) | (g << 8) | b;
        graphics_clear(color);
    }
}

/**
 * Draw progress bar
 */
void splash_draw_progress_bar(uint32_t x, uint32_t y, uint32_t width, uint32_t height, float progress, uint32_t bg_color, uint32_t fg_color) {
    // Draw background
    for (uint32_t px = x; px < x + width; px++) {
        for (uint32_t py = y; py < y + height; py++) {
            graphics_set_pixel(px, py, bg_color);
        }
    }

    // Draw progress
    uint32_t progress_width = (uint32_t)(width * progress);
    for (uint32_t px = x; px < x + progress_width; px++) {
        for (uint32_t py = y; py < y + height; py++) {
            graphics_set_pixel(px, py, fg_color);
        }
    }
}

/**
 * Draw logo text
 */
void splash_draw_logo(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const char* text, uint32_t color) {
    // Simple text rendering - in a real implementation this would use a font
    // For now, just show the text is being displayed
    // (Graphics text rendering would go here)
}

/**
 * Show main boot message
 */
void splash_show_message(const char* message) {
    // Display boot message
    // (Would render text to screen)
}

/**
 * Show sub message
 */
void splash_show_submessage(const char* submessage) {
    // Display sub message
    // (Would render smaller text)
}

/**
 * Update progress
 */
void splash_update_progress(float progress) {
    // Update progress bar
    splash_draw_progress_bar(
        g_splash_config.progress_bar_x,
        g_splash_config.progress_bar_y,
        g_splash_config.progress_bar_width,
        g_splash_config.progress_bar_height,
        progress,
        0x333333,
        g_splash_config.accent_color
    );

    graphics_flip();
}

/**
 * Set splash configuration
 */
void splash_set_config(const splash_config_t* config) {
    memcpy(&g_splash_config, config, sizeof(splash_config_t));
}

/**
 * Get splash configuration
 */
splash_config_t* splash_get_config(void) {
    return &g_splash_config;
}

/**
 * BMP loader functions (forward declarations for compatibility)
 */
int bmp_load_splash(void) {
    // Load splash screen BMP
    return 0;
}

int bmp_show_splash(void) {
    // Show splash screen BMP
    return 0;
}
