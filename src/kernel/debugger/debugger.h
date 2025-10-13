/**
 * CompileOS Integrated Debugger - Header
 * 
 * Full-featured debugger with breakpoints, memory inspection, and step debugging
 */

#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// Debugger breakpoint types
typedef enum {
    BREAKPOINT_TYPE_SOFTWARE = 0,
    BREAKPOINT_TYPE_HARDWARE,
    BREAKPOINT_TYPE_MEMORY,
    BREAKPOINT_TYPE_IO
} breakpoint_type_t;

// Debugger breakpoint
typedef struct {
    uint32_t id;
    uint64_t address;
    breakpoint_type_t type;
    bool enabled;
    bool temporary;
    uint8_t original_byte; // For software breakpoints
    char description[64];
} debugger_breakpoint_t;

// Debugger register context
typedef struct {
    uint64_t rax, rbx, rcx, rdx;
    uint64_t rsi, rdi, rbp, rsp;
    uint64_t r8, r9, r10, r11;
    uint64_t r12, r13, r14, r15;
    uint64_t rip, rflags;
    uint64_t cs, ds, es, fs, gs, ss;
    uint64_t cr0, cr2, cr3, cr4;
} debugger_registers_t;

// Debugger memory region
typedef struct {
    uint64_t address;
    size_t size;
    uint8_t* data;
    bool is_readable;
    bool is_writable;
    bool is_executable;
} debugger_memory_region_t;

// Debugger state
typedef enum {
    DEBUGGER_STATE_STOPPED = 0,
    DEBUGGER_STATE_RUNNING,
    DEBUGGER_STATE_STEPPING,
    DEBUGGER_STATE_BREAKPOINT,
    DEBUGGER_STATE_EXCEPTION
} debugger_state_t;

// Debugger command types
typedef enum {
    DEBUGGER_CMD_CONTINUE = 0,
    DEBUGGER_CMD_STEP,
    DEBUGGER_CMD_STEP_OVER,
    DEBUGGER_CMD_STEP_OUT,
    DEBUGGER_CMD_BREAKPOINT_SET,
    DEBUGGER_CMD_BREAKPOINT_DELETE,
    DEBUGGER_CMD_BREAKPOINT_LIST,
    DEBUGGER_CMD_REGISTERS,
    DEBUGGER_CMD_MEMORY_READ,
    DEBUGGER_CMD_MEMORY_WRITE,
    DEBUGGER_CMD_MEMORY_SEARCH,
    DEBUGGER_CMD_DISASSEMBLE,
    DEBUGGER_CMD_BACKTRACE,
    DEBUGGER_CMD_VARIABLES,
    DEBUGGER_CMD_WATCH,
    DEBUGGER_CMD_QUIT
} debugger_command_type_t;

// Debugger command structure
typedef struct {
    debugger_command_type_t type;
    char args[256];
    bool success;
    char output[512];
} debugger_command_t;

// Debugger initialization and control
int debugger_init(void);
void debugger_shutdown(void);
debugger_state_t debugger_get_state(void);
void debugger_set_state(debugger_state_t state);

// Breakpoint management
int debugger_set_breakpoint(uint64_t address, breakpoint_type_t type, const char* description);
int debugger_delete_breakpoint(uint32_t id);
int debugger_enable_breakpoint(uint32_t id);
int debugger_disable_breakpoint(uint32_t id);
int debugger_list_breakpoints(debugger_breakpoint_t* breakpoints, size_t max_count, size_t* actual_count);

// Execution control
int debugger_continue(void);
int debugger_step(void);
int debugger_step_over(void);
int debugger_step_out(void);
int debugger_pause(void);

// Register access
int debugger_get_registers(debugger_registers_t* registers);
int debugger_set_register(const char* name, uint64_t value);
uint64_t debugger_get_register(const char* name);

// Memory access
int debugger_read_memory(uint64_t address, size_t size, uint8_t* buffer);
int debugger_write_memory(uint64_t address, size_t size, const uint8_t* buffer);
int debugger_search_memory(uint64_t start, uint64_t end, const uint8_t* pattern, size_t pattern_size, uint64_t* result);
int debugger_get_memory_regions(debugger_memory_region_t* regions, size_t max_count, size_t* actual_count);

// Disassembly
int debugger_disassemble(uint64_t address, size_t count, char* output, size_t output_size);
int debugger_disassemble_function(const char* function_name, char* output, size_t output_size);

// Backtrace and call stack
int debugger_get_backtrace(uint64_t* addresses, size_t max_count, size_t* actual_count);
int debugger_get_call_stack(char* output, size_t output_size);

// Variable inspection
int debugger_get_variables(char* output, size_t output_size);
int debugger_get_variable(const char* name, char* output, size_t output_size);
int debugger_set_variable(const char* name, const char* value);

// Watch expressions
int debugger_add_watch(const char* expression);
int debugger_remove_watch(uint32_t watch_id);
int debugger_evaluate_watch(uint32_t watch_id, char* output, size_t output_size);
int debugger_list_watches(char* output, size_t output_size);

// Command processing
int debugger_process_command(const char* command, char* output, size_t output_size);
int debugger_parse_command(const char* input, debugger_command_t* cmd);

// Debugger interface
int debugger_start_session(void);
int debugger_end_session(void);
bool debugger_is_active(void);

// Debugger output
int debugger_print(const char* format, ...);
int debugger_print_error(const char* format, ...);
int debugger_print_warning(const char* format, ...);

// Debugger symbols (for future ELF support)
int debugger_load_symbols(const char* symbol_file);
int debugger_get_symbol_name(uint64_t address, char* name, size_t name_size);
uint64_t debugger_get_symbol_address(const char* name);

// Debugger exceptions
int debugger_handle_exception(uint32_t exception_number, uint64_t error_code, void* context);
int debugger_handle_breakpoint(uint64_t address, void* context);

// Debugger configuration
typedef struct {
    bool auto_break_on_exception;
    bool auto_break_on_signal;
    bool show_register_changes;
    bool show_memory_changes;
    bool verbose_output;
    uint32_t max_breakpoints;
    uint32_t max_watch_expressions;
} debugger_config_t;

int debugger_set_config(const debugger_config_t* config);
int debugger_get_config(debugger_config_t* config);

#endif // DEBUGGER_H










