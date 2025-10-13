/**
 * CompileOS Calculator - Interactive Calculator Application
 * 
 * A graphical calculator using the REPL engine
 */

#include "../runtime/runtime.h"
#include "../kernel/repl/repl.h"
#include <string.h>

#define MAX_DISPLAY 32
#define MAX_INPUT 256

// Calculator state
typedef struct {
    char display[MAX_DISPLAY];
    char input[MAX_INPUT];
    size_t input_pos;
    bool running;
} calculator_t;

static calculator_t g_calc = {0};

// Initialize calculator
static void calc_init(void) {
    strcpy(g_calc.display, "0");
    g_calc.input[0] = '\0';
    g_calc.input_pos = 0;
    g_calc.running = true;
}

// Draw calculator UI
static void calc_draw(void) {
    println("\n╔════════════════════════════════╗");
    println("║   CompileOS Calculator v1.0    ║");
    println("╠════════════════════════════════╣");
    printf_rt("║ %-30s ║\n", g_calc.display);
    println("╠════════════════════════════════╣");
    println("║  Commands:                     ║");
    println("║  - Type expression and Enter   ║");
    println("║  - 'clear' to clear            ║");
    println("║  - 'exit' to quit              ║");
    println("╚════════════════════════════════╝");
    print("\nExpression: ");
}

// Evaluate expression
static void calc_evaluate(const char* expr) {
    if (strcmp(expr, "clear") == 0) {
        strcpy(g_calc.display, "0");
        return;
    }
    
    if (strcmp(expr, "exit") == 0) {
        g_calc.running = false;
        return;
    }
    
    repl_value_t result;
    if (repl_evaluate(expr, &result) == 0) {
        sprintf(g_calc.display, "%lld", (long long)result.value.int64_val);
    } else {
        strcpy(g_calc.display, "Error");
    }
}

// Main calculator loop
int calculator_main(void) {
    calc_init();
    
    while (g_calc.running) {
        calc_draw();
        
        // Read input
        char* line = readline();
        if (!line || strlen(line) == 0) {
            continue;
        }
        
        // Evaluate
        calc_evaluate(line);
    }
    
    println("\nCalculator closed.");
    return 0;
}
