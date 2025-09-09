/**
 * CompileOS REPL (Read-Eval-Print Loop) - Header
 * 
 * Interactive development environment for CompileOS
 */

#ifndef REPL_H
#define REPL_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// REPL expression types
typedef enum {
    REPL_EXPR_VARIABLE = 0,
    REPL_EXPR_FUNCTION_CALL,
    REPL_EXPR_ASSIGNMENT,
    REPL_EXPR_ARITHMETIC,
    REPL_EXPR_LOGICAL,
    REPL_EXPR_COMPARISON,
    REPL_EXPR_MEMORY_ACCESS,
    REPL_EXPR_REGISTER_ACCESS,
    REPL_EXPR_SYSTEM_CALL,
    REPL_EXPR_COMMENT,
    REPL_EXPR_UNKNOWN
} repl_expression_type_t;

// REPL data types
typedef enum {
    REPL_TYPE_VOID = 0,
    REPL_TYPE_INT8,
    REPL_TYPE_INT16,
    REPL_TYPE_INT32,
    REPL_TYPE_INT64,
    REPL_TYPE_UINT8,
    REPL_TYPE_UINT16,
    REPL_TYPE_UINT32,
    REPL_TYPE_UINT64,
    REPL_TYPE_FLOAT,
    REPL_TYPE_DOUBLE,
    REPL_TYPE_POINTER,
    REPL_TYPE_STRING,
    REPL_TYPE_ARRAY,
    REPL_TYPE_STRUCT
} repl_data_type_t;

// REPL value
typedef struct {
    repl_data_type_t type;
    union {
        int8_t int8_val;
        int16_t int16_val;
        int32_t int32_val;
        int64_t int64_val;
        uint8_t uint8_val;
        uint16_t uint16_val;
        uint32_t uint32_val;
        uint64_t uint64_val;
        float float_val;
        double double_val;
        void* pointer_val;
        char* string_val;
    } value;
    bool is_valid;
} repl_value_t;

// REPL variable
typedef struct {
    char name[64];
    repl_value_t value;
    bool is_constant;
    bool is_global;
} repl_variable_t;

// REPL function
typedef struct {
    char name[64];
    repl_data_type_t return_type;
    repl_data_type_t* parameter_types;
    size_t parameter_count;
    void* function_ptr;
    bool is_builtin;
} repl_function_t;

// REPL expression
typedef struct {
    repl_expression_type_t type;
    char* expression;
    repl_value_t result;
    bool is_valid;
    char* error_message;
} repl_expression_t;

// REPL context
typedef struct {
    repl_variable_t* variables;
    size_t variable_count;
    size_t variable_capacity;
    repl_function_t* functions;
    size_t function_count;
    size_t function_capacity;
    char* current_input;
    size_t input_length;
    repl_expression_t* history;
    size_t history_count;
    size_t history_capacity;
    bool running;
} repl_context_t;

// REPL initialization and control
int repl_init(void);
void repl_shutdown(void);
int repl_start(void);
void repl_stop(void);
bool repl_is_running(void);

// REPL input/output
int repl_read_input(char* buffer, size_t buffer_size);
int repl_write_output(const char* format, ...);
int repl_write_error(const char* format, ...);
int repl_write_result(const repl_value_t* value);

// REPL expression evaluation
int repl_evaluate(const char* expression, repl_value_t* result);
int repl_evaluate_expression(const repl_expression_t* expr, repl_value_t* result);
int repl_parse_expression(const char* input, repl_expression_t* expr);

// REPL variable management
int repl_set_variable(const char* name, const repl_value_t* value);
int repl_get_variable(const char* name, repl_value_t* value);
int repl_delete_variable(const char* name);
int repl_list_variables(char* output, size_t output_size);
int repl_clear_variables(void);

// REPL function management
int repl_register_function(const repl_function_t* function);
int repl_unregister_function(const char* name);
int repl_get_function(const char* name, repl_function_t* function);
int repl_list_functions(char* output, size_t output_size);
int repl_call_function(const char* name, const repl_value_t* args, size_t arg_count, repl_value_t* result);

// REPL built-in functions
int repl_builtin_help(int argc, char** argv);
int repl_builtin_clear(int argc, char** argv);
int repl_builtin_vars(int argc, char** argv);
int repl_builtin_funcs(int argc, char** argv);
int repl_builtin_history(int argc, char** argv);
int repl_builtin_exit(int argc, char** argv);
int repl_builtin_memory(int argc, char** argv);
int repl_builtin_register(int argc, char** argv);
int repl_builtin_disassemble(int argc, char** argv);
int repl_builtin_breakpoint(int argc, char** argv);

// REPL arithmetic operations
int repl_add(const repl_value_t* a, const repl_value_t* b, repl_value_t* result);
int repl_subtract(const repl_value_t* a, const repl_value_t* b, repl_value_t* result);
int repl_multiply(const repl_value_t* a, const repl_value_t* b, repl_value_t* result);
int repl_divide(const repl_value_t* a, const repl_value_t* b, repl_value_t* result);
int repl_modulo(const repl_value_t* a, const repl_value_t* b, repl_value_t* result);

// REPL logical operations
int repl_and(const repl_value_t* a, const repl_value_t* b, repl_value_t* result);
int repl_or(const repl_value_t* a, const repl_value_t* b, repl_value_t* result);
int repl_not(const repl_value_t* a, repl_value_t* result);
int repl_xor(const repl_value_t* a, const repl_value_t* b, repl_value_t* result);

// REPL comparison operations
int repl_equal(const repl_value_t* a, const repl_value_t* b, repl_value_t* result);
int repl_not_equal(const repl_value_t* a, const repl_value_t* b, repl_value_t* result);
int repl_less_than(const repl_value_t* a, const repl_value_t* b, repl_value_t* result);
int repl_greater_than(const repl_value_t* a, const repl_value_t* b, repl_value_t* result);
int repl_less_equal(const repl_value_t* a, const repl_value_t* b, repl_value_t* result);
int repl_greater_equal(const repl_value_t* a, const repl_value_t* b, repl_value_t* result);

// REPL memory operations
int repl_memory_read(uint64_t address, repl_data_type_t type, repl_value_t* result);
int repl_memory_write(uint64_t address, const repl_value_t* value);
int repl_memory_alloc(size_t size, repl_value_t* result);
int repl_memory_free(uint64_t address);
int repl_memory_search(uint64_t start, uint64_t end, const repl_value_t* pattern, repl_value_t* result);

// REPL register operations
int repl_register_read(const char* name, repl_value_t* result);
int repl_register_write(const char* name, const repl_value_t* value);
int repl_register_list(char* output, size_t output_size);

// REPL type conversion
int repl_convert_type(const repl_value_t* input, repl_data_type_t target_type, repl_value_t* result);
int repl_promote_type(const repl_value_t* a, const repl_value_t* b, repl_data_type_t* result_type);

// REPL value operations
int repl_value_create(repl_data_type_t type, const void* value, repl_value_t* result);
int repl_value_copy(const repl_value_t* src, repl_value_t* dest);
int repl_value_destroy(repl_value_t* value);
int repl_value_to_string(const repl_value_t* value, char* buffer, size_t buffer_size);
int repl_value_from_string(const char* str, repl_data_type_t type, repl_value_t* result);

// REPL history
int repl_add_to_history(const char* expression);
int repl_get_history(size_t index, char* buffer, size_t buffer_size);
int repl_clear_history(void);
int repl_search_history(const char* pattern, char* output, size_t output_size);

// REPL configuration
typedef struct {
    bool auto_complete;
    bool syntax_highlighting;
    bool show_types;
    bool verbose_output;
    size_t max_history;
    size_t max_variables;
    size_t max_functions;
} repl_config_t;

int repl_set_config(const repl_config_t* config);
int repl_get_config(repl_config_t* config);

// REPL error handling
typedef enum {
    REPL_ERROR_NONE = 0,
    REPL_ERROR_SYNTAX,
    REPL_ERROR_TYPE_MISMATCH,
    REPL_ERROR_UNDEFINED_VARIABLE,
    REPL_ERROR_UNDEFINED_FUNCTION,
    REPL_ERROR_DIVISION_BY_ZERO,
    REPL_ERROR_MEMORY_ACCESS,
    REPL_ERROR_INVALID_ADDRESS,
    REPL_ERROR_STACK_OVERFLOW,
    REPL_ERROR_UNKNOWN
} repl_error_t;

int repl_get_last_error(repl_error_t* error, char* message, size_t message_size);
int repl_clear_error(void);

// REPL debugging
int repl_debug_mode(bool enable);
bool repl_is_debug_mode(void);
int repl_debug_print(const char* format, ...);
int repl_debug_variables(void);
int repl_debug_functions(void);
int repl_debug_memory(void);

#endif // REPL_H
