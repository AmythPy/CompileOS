/**
 * CompileOS REPL - Full Implementation
 * Interactive expression evaluator with variables
 */

#include "repl.h"
#include <string.h>

// Use our custom ctype functions from util.c
extern int isdigit(int c);
extern int isalpha(int c);
extern int isalnum(int c);
extern int isspace(int c);
extern int snprintf(char* str, size_t size, const char* format, ...);

#define MAX_VARIABLES 64
#define MAX_EXPRESSION_LENGTH 256

// Variable storage
typedef struct {
    char name[64];
    int64_t value;
    bool in_use;
} repl_variable_storage_t;

// REPL state
static struct {
    bool initialized;
    bool running;
    repl_variable_storage_t variables[MAX_VARIABLES];
    size_t variable_count;
} g_repl = {0};

// Expression parser state
typedef struct {
    const char* input;
    size_t pos;
    char current_char;
} parser_state_t;

// Forward declarations
static int64_t parse_expression(parser_state_t* state);
static int64_t parse_term(parser_state_t* state);
static int64_t parse_factor(parser_state_t* state);
static void advance(parser_state_t* state);
static void skip_whitespace(parser_state_t* state);
static int64_t get_variable(const char* name);
static void set_variable(const char* name, int64_t value);

// Parser helper functions
static void advance(parser_state_t* state) {
    state->current_char = state->input[state->pos++];
}

static void skip_whitespace(parser_state_t* state) {
    while (state->current_char == ' ' || state->current_char == '\t') {
        advance(state);
    }
}

// Variable management
static int64_t get_variable(const char* name) {
    for (size_t i = 0; i < MAX_VARIABLES; i++) {
        if (g_repl.variables[i].in_use && strcmp(g_repl.variables[i].name, name) == 0) {
            return g_repl.variables[i].value;
        }
    }
    return 0; // Undefined variable returns 0
}

static void set_variable(const char* name, int64_t value) {
    // Check if variable exists
    for (size_t i = 0; i < MAX_VARIABLES; i++) {
        if (g_repl.variables[i].in_use && strcmp(g_repl.variables[i].name, name) == 0) {
            g_repl.variables[i].value = value;
            return;
        }
    }
    
    // Create new variable
    for (size_t i = 0; i < MAX_VARIABLES; i++) {
        if (!g_repl.variables[i].in_use) {
            strncpy(g_repl.variables[i].name, name, 63);
            g_repl.variables[i].name[63] = '\0';
            g_repl.variables[i].value = value;
            g_repl.variables[i].in_use = true;
            g_repl.variable_count++;
            return;
        }
    }
}

// Parse number or variable
static int64_t parse_factor(parser_state_t* state) {
    skip_whitespace(state);
    
    // Handle parentheses
    if (state->current_char == '(') {
        advance(state); // Skip '('
        int64_t result = parse_expression(state);
        if (state->current_char == ')') {
            advance(state); // Skip ')'
        }
        return result;
    }
    
    // Handle negative numbers
    if (state->current_char == '-') {
        advance(state);
        return -parse_factor(state);
    }
    
    // Handle numbers
    if (isdigit(state->current_char)) {
        int64_t value = 0;
        while (isdigit(state->current_char)) {
            value = value * 10 + (state->current_char - '0');
            advance(state);
        }
        return value;
    }
    
    // Handle variables
    if (isalpha(state->current_char) || state->current_char == '_') {
        char name[64];
        size_t i = 0;
        while ((isalnum(state->current_char) || state->current_char == '_') && i < 63) {
            name[i++] = state->current_char;
            advance(state);
        }
        name[i] = '\0';
        return get_variable(name);
    }
    
    return 0;
}

// Parse multiplication and division
static int64_t parse_term(parser_state_t* state) {
    int64_t result = parse_factor(state);
    
    skip_whitespace(state);
    while (state->current_char == '*' || state->current_char == '/' || state->current_char == '%') {
        char op = state->current_char;
        advance(state);
        int64_t right = parse_factor(state);
        
        if (op == '*') {
            result *= right;
        } else if (op == '/') {
            if (right != 0) {
                result /= right;
            }
        } else if (op == '%') {
            if (right != 0) {
                result %= right;
            }
        }
        skip_whitespace(state);
    }
    
    return result;
}

// Parse addition and subtraction
static int64_t parse_expression(parser_state_t* state) {
    int64_t result = parse_term(state);
    
    skip_whitespace(state);
    while (state->current_char == '+' || state->current_char == '-') {
        char op = state->current_char;
        advance(state);
        int64_t right = parse_term(state);
        
        if (op == '+') {
            result += right;
        } else {
            result -= right;
        }
        skip_whitespace(state);
    }
    
    return result;
}

// Evaluate expression
int repl_evaluate(const char* expression, repl_value_t* result) {
    if (!expression || !result) {
        return -1;
    }
    
    // Check for assignment (variable = expression)
    const char* equals = strchr(expression, '=');
    if (equals) {
        // Extract variable name
        char var_name[64];
        size_t i = 0;
        const char* p = expression;
        while (p < equals && i < 63) {
            if (!isspace(*p)) {
                var_name[i++] = *p;
            }
            p++;
        }
        var_name[i] = '\0';
        
        // Evaluate right side
        parser_state_t state;
        state.input = equals + 1;
        state.pos = 0;
        state.current_char = state.input[0];
        
        int64_t value = parse_expression(&state);
        set_variable(var_name, value);
        
        result->type = REPL_TYPE_INT64;
        result->value.int64_val = value;
        result->is_valid = true;
        return 0;
    }
    
    // Regular expression evaluation
    parser_state_t state;
    state.input = expression;
    state.pos = 0;
    state.current_char = state.input[0];
    
    int64_t value = parse_expression(&state);
    
    result->type = REPL_TYPE_INT64;
    result->value.int64_val = value;
    result->is_valid = true;
    return 0;
}

// Initialize REPL
int repl_init(void) {
    if (g_repl.initialized) {
        return 0;
    }
    
    memset(g_repl.variables, 0, sizeof(g_repl.variables));
    g_repl.variable_count = 0;
    g_repl.initialized = true;
    g_repl.running = false;
    return 0;
}

// Shutdown REPL
void repl_shutdown(void) {
    g_repl.initialized = false;
    g_repl.running = false;
}

// Start REPL
int repl_start(void) {
    if (!g_repl.initialized) {
        return -1;
    }
    g_repl.running = true;
    return 0;
}

// Stop REPL
void repl_stop(void) {
    g_repl.running = false;
}

// Check if running
bool repl_is_running(void) {
    return g_repl.running;
}

// List variables
int repl_list_variables(char* output, size_t output_size) {
    if (!output || output_size == 0) {
        return -1;
    }
    
    size_t written = 0;
    for (size_t i = 0; i < MAX_VARIABLES && written < output_size - 1; i++) {
        if (g_repl.variables[i].in_use) {
            int len = snprintf(output + written, output_size - written,
                             "%s = %lld\n", 
                             g_repl.variables[i].name,
                             (long long)g_repl.variables[i].value);
            if (len > 0) {
                written += len;
            }
        }
    }
    
    return written;
}

// Clear all variables
int repl_clear_variables(void) {
    memset(g_repl.variables, 0, sizeof(g_repl.variables));
    g_repl.variable_count = 0;
    return 0;
}

// Set variable (public API)
int repl_set_variable(const char* name, const repl_value_t* value) {
    if (!name || !value) {
        return -1;
    }
    
    set_variable(name, value->value.int64_val);
    return 0;
}

// Get variable (public API)
int repl_get_variable(const char* name, repl_value_t* value) {
    if (!name || !value) {
        return -1;
    }
    
    value->type = REPL_TYPE_INT64;
    value->value.int64_val = get_variable(name);
    value->is_valid = true;
    return 0;
}

// Delete variable
int repl_delete_variable(const char* name) {
    if (!name) {
        return -1;
    }
    
    for (size_t i = 0; i < MAX_VARIABLES; i++) {
        if (g_repl.variables[i].in_use && strcmp(g_repl.variables[i].name, name) == 0) {
            g_repl.variables[i].in_use = false;
            g_repl.variable_count--;
            return 0;
        }
    }
    
    return -1;
}

// Stub implementations for remaining functions
int repl_read_input(char* buffer, size_t buffer_size) { (void)buffer; (void)buffer_size; return 0; }
int repl_write_output(const char* format, ...) { (void)format; return 0; }
int repl_write_error(const char* format, ...) { (void)format; return 0; }
int repl_write_result(const repl_value_t* value) { (void)value; return 0; }
int repl_evaluate_expression(const repl_expression_t* expr, repl_value_t* result) { (void)expr; (void)result; return 0; }
int repl_parse_expression(const char* input, repl_expression_t* expr) { (void)input; (void)expr; return 0; }
int repl_register_function(const repl_function_t* function) { (void)function; return 0; }
int repl_unregister_function(const char* name) { (void)name; return 0; }
int repl_get_function(const char* name, repl_function_t* function) { (void)name; (void)function; return 0; }
int repl_list_functions(char* output, size_t output_size) { (void)output; (void)output_size; return 0; }
int repl_call_function(const char* name, const repl_value_t* args, size_t arg_count, repl_value_t* result) {
    (void)name; (void)args; (void)arg_count; (void)result; return 0;
}
