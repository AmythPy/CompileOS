/**
 * CompileOS Calculator Application
 * Full-featured calculator with GUI interface
 */

#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <stdbool.h>

// Calculator state
typedef struct {
    char display[32];        // Current display value
    char operation;          // Current operation (+, -, *, /)
    double operand1;         // First operand
    double operand2;         // Second operand
    bool new_number;         // Waiting for new number input
    bool has_result;         // Has a result to display
} calculator_t;

// Calculator functions
int calculator_init(void);
int calculator_create_window(void);
int calculator_handle_input(char input);
int calculator_draw(void);
double calculator_evaluate(double a, double b, char op);
int calculator_clear(void);
int calculator_backspace(void);

#endif // CALCULATOR_H
