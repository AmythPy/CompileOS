/**
 * CompileOS Calculator Implementation
 * Professional calculator with full mathematical operations
 */

#include "calculator.h"
#include "../drivers/vga/vga.h"
#include <stdio.h>
#include <string.h>
// Simple string to double conversion (basic implementation)
double atof(const char* str) {
    double result = 0.0;
    double sign = 1.0;
    int i = 0;

    // Skip whitespace
    while (str[i] == ' ' || str[i] == '\t') i++;

    // Handle sign
    if (str[i] == '-') {
        sign = -1.0;
        i++;
    } else if (str[i] == '+') {
        i++;
    }

    // Parse digits before decimal point
    while (str[i] >= '0' && str[i] <= '9') {
        result = result * 10.0 + (str[i] - '0');
        i++;
    }

    // Parse decimal part
    if (str[i] == '.') {
        i++;
        double decimal = 0.1;
        while (str[i] >= '0' && str[i] <= '9') {
            result += (str[i] - '0') * decimal;
            decimal *= 0.1;
            i++;
        }
    }

    return result * sign;
}

// Simple square root implementation (Newton's method)
double sqrt(double x) {
    if (x < 0) return 0;  // Error case

    double guess = x / 2.0;
    for (int i = 0; i < 10; i++) {
        double new_guess = (guess + x / guess) / 2.0;
        if (new_guess == guess) break;
        guess = new_guess;
    }
    return guess;
}

// Simple power function
double pow(double base, double exp) {
    if (exp == 0) return 1;
    if (exp == 1) return base;

    double result = 1;
    for (int i = 0; i < (int)exp; i++) {
        result *= base;
    }
    return result;
}

// Global calculator state
static calculator_t g_calc_state = {0};
// Calculator button definitions
typedef struct {
    char label[4];
    uint8_t x, y;
    uint8_t width, height;
    bool is_operator;
    char value;
} calc_button_t;

static const calc_button_t calc_buttons[] = {
    // Row 1
    {"7", 1, 3, 4, 3, false, '7'},
    {"8", 6, 3, 4, 3, false, '8'},
    {"9", 11, 3, 4, 3, false, '9'},
    {"÷", 16, 3, 4, 3, true, '/'},
    {"C", 21, 3, 4, 3, true, 'C'},

    // Row 2
    {"4", 1, 7, 4, 3, false, '4'},
    {"5", 6, 7, 4, 3, false, '5'},
    {"6", 11, 7, 4, 3, false, '6'},
    {"×", 16, 7, 4, 3, true, '*'},
    {"⌫", 21, 7, 4, 3, true, 'B'},

    // Row 3
    {"1", 1, 11, 4, 3, false, '1'},
    {"2", 6, 11, 4, 3, false, '2'},
    {"3", 11, 11, 4, 3, false, '3'},
    {"-", 16, 11, 4, 3, true, '-'},
    {"±", 21, 11, 4, 3, true, 'N'},

    // Row 4
    {"0", 1, 15, 4, 3, false, '0'},
    {".", 6, 15, 4, 3, false, '.'},
    {"=", 11, 15, 9, 3, true, '='},
    {"+", 21, 15, 4, 3, true, '+'},

    // Row 5 (scientific functions)
    {"√", 1, 19, 4, 3, true, 'R'},
    {"^", 6, 19, 4, 3, true, '^'},
    {"(", 11, 19, 4, 3, true, '('},
    {")", 16, 19, 4, 3, true, ')'},
    {"π", 21, 19, 4, 3, true, 'P'},
};

#define NUM_BUTTONS (sizeof(calc_buttons) / sizeof(calc_button_t))

/**
 * Initialize calculator
 */
int calculator_init(void) {
    calculator_clear();
    return 0;
}

/**
 * Create calculator window
 */
int calculator_create_window(void) {
    // This would integrate with the window manager
    // For now, just initialize the calculator state
    return calculator_init();
}

/**
 * Handle calculator input
 */
int calculator_handle_input(char input) {
    if (!input) return 0;

    // Handle numbers
    if (input >= '0' && input <= '9') {
        if (g_calc_state.new_number) {
            strcpy(g_calc_state.display, "");
            g_calc_state.new_number = false;
        }
        if (strlen(g_calc_state.display) < sizeof(g_calc_state.display) - 1) {
            char digit[2] = {input, '\0'};
            strcat(g_calc_state.display, digit);
        }
    }
    // Handle decimal point
    else if (input == '.') {
        if (g_calc_state.new_number) {
            strcpy(g_calc_state.display, "0");
            g_calc_state.new_number = false;
        }
        if (!strchr(g_calc_state.display, '.')) {
            strcat(g_calc_state.display, ".");
        }
    }
    // Handle operators
    else if (strchr("+-*/", input)) {
        if (!g_calc_state.new_number && strlen(g_calc_state.display) > 0) {
            g_calc_state.operand1 = atof(g_calc_state.display);
            g_calc_state.operation = input;
            g_calc_state.new_number = true;
        }
    }
    // Handle equals
    else if (input == '=') {
        if (!g_calc_state.new_number && strlen(g_calc_state.display) > 0) {
            g_calc_state.operand2 = atof(g_calc_state.display);
            double result = calculator_evaluate(g_calc_state.operand1, g_calc_state.operand2, g_calc_state.operation);

            // Format result
            if (result == (int)result) {
                snprintf(g_calc_state.display, sizeof(g_calc_state.display), "%.0f", result);
            } else {
                snprintf(g_calc_state.display, sizeof(g_calc_state.display), "%.6f", result);
            }

            g_calc_state.has_result = true;
            g_calc_state.new_number = true;
        }
    }
    // Handle clear
    else if (input == 'C') {
        calculator_clear();
    }
    // Handle backspace
    else if (input == 'B') {
        calculator_backspace();
    }
    // Handle sign change
    else if (input == 'N') {
        if (strlen(g_calc_state.display) > 0) {
            double value = atof(g_calc_state.display);
            value = -value;
            snprintf(g_calc_state.display, sizeof(g_calc_state.display), "%.6f", value);
        }
    }
    // Handle square root
    else if (input == 'R') {
        if (strlen(g_calc_state.display) > 0) {
            double value = atof(g_calc_state.display);
            if (value >= 0) {
                value = sqrt(value);
                snprintf(g_calc_state.display, sizeof(g_calc_state.display), "%.6f", value);
                g_calc_state.has_result = true;
                g_calc_state.new_number = true;
            }
        }
    }
    // Handle pi
    else if (input == 'P') {
        snprintf(g_calc_state.display, sizeof(g_calc_state.display), "%.6f", 3.1415926535);
        g_calc_state.has_result = true;
        g_calc_state.new_number = true;
    }

    return 0;
}

/**
 * Draw calculator interface
 */
int calculator_draw(void) {
    // Draw display area
    vga_puts_at(2, 1, "╔══════════════════════════════════════╗");
    vga_puts_at(2, 2, "║           CALCULATOR v1.0           ║");
    vga_puts_at(2, 3, "╚══════════════════════════════════════╝");

    // Draw display
    vga_puts_at(3, 5, "┌────────────────────────────────────┐");
    for (int y = 6; y <= 8; y++) {
        vga_puts_at(3, y, "│                                    │");
    }
    vga_puts_at(3, 9, "└────────────────────────────────────┘");

    // Display current value (right-aligned)
    if (strlen(g_calc_state.display) > 0) {
        int display_x = 37 - strlen(g_calc_state.display);
        vga_puts_at(display_x, 7, g_calc_state.display);
    } else {
        vga_puts_at(35, 7, "0");
    }

    // Draw buttons
    for (int i = 0; i < NUM_BUTTONS; i++) {
        const calc_button_t* btn = &calc_buttons[i];

        // Button border
        vga_puts_at(btn->x, btn->y, "┌");
        vga_puts_at(btn->x + btn->width - 1, btn->y, "┐");
        vga_puts_at(btn->x, btn->y + btn->height - 1, "└");
        vga_puts_at(btn->x + btn->width - 1, btn->y + btn->height - 1, "┘");

        for (int by = btn->y + 1; by < btn->y + btn->height - 1; by++) {
            vga_puts_at(btn->x, by, "│");
            vga_puts_at(btn->x + btn->width - 1, by, "│");
        }

        // Button label (centered)
        int label_x = btn->x + (btn->width - strlen(btn->label)) / 2;
        int label_y = btn->y + (btn->height - 1) / 2;
        vga_puts_at(label_x, label_y, btn->label);
    }

    return 0;
}

/**
 * Evaluate mathematical expression
 */
double calculator_evaluate(double a, double b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/':
            if (b != 0) return a / b;
            return 0; // Division by zero
        case '^': return pow(a, b);
        default: return 0;
    }
}

/**
 * Clear calculator
 */
int calculator_clear(void) {
    strcpy(g_calc_state.display, "");
    g_calc_state.operand1 = 0;
    g_calc_state.operand2 = 0;
    g_calc_state.operation = '\0';
    g_calc_state.new_number = true;
    g_calc_state.has_result = false;
    return 0;
}

/**
 * Backspace function
 */
int calculator_backspace(void) {
    size_t len = strlen(g_calc_state.display);
    if (len > 0) {
        g_calc_state.display[len - 1] = '\0';
    }
    return 0;
}
