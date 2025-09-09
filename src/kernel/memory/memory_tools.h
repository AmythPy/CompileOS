/**
 * CompileOS Memory Tools - Header
 * 
 * Direct memory access, inspection, and manipulation tools
 */

#ifndef MEMORY_TOOLS_H
#define MEMORY_TOOLS_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "access.h"

// Memory access modes
//typedef enum {
    //MEMORY_ACCESS_READ = 0,
    //MEMORY_ACCESS_WRITE,
    //MEMORY_ACCESS_EXECUTE,
    //MEMORY_ACCESS_READ_WRITE,
    //MEMORY_ACCESS_READ_EXECUTE,
  //  MEMORY_ACCESS_WRITE_EXECUTE,
//    MEMORY_ACCESS_ALL
//} memory_access_mode_t;

// Memory data types
//typedef enum {
  //  MEMORY_DATA_BYTE = 1,
    //MEMORY_DATA_WORD = 2,
    //MEMORY_DATA_DWORD = 4,
    //MEMORY_DATA_QWORD = 8,
    //MEMORY_DATA_FLOAT = 4,
    //MEMORY_DATA_DOUBLE = 8
//} memory_data_type_t;

// Memory region info
typedef struct {
    uint64_t base_address;
    uint64_t size;
    memory_access_mode_t access;
    bool is_allocated;
    bool is_executable;
    bool is_readable;
    bool is_writable;
    char description[64];
} memory_region_info_t;

// Memory dump format
typedef enum {
    MEMORY_DUMP_HEX = 0,
    MEMORY_DUMP_ASCII,
    MEMORY_DUMP_BINARY,
    MEMORY_DUMP_DISASSEMBLY
} memory_dump_format_t;

// Memory search result
typedef struct {
    uint64_t address;
    size_t size;
    uint8_t* data;
    bool found;
} memory_search_result_t;

// Memory tools initialization
int memory_tools_init(void);
void memory_tools_shutdown(void);

// Direct memory access
int memory_read_direct(uint64_t address, void* buffer, size_t size);
int memory_write_direct(uint64_t address, const void* buffer, size_t size);
int memory_read_byte(uint64_t address, uint8_t* value);
int memory_write_byte(uint64_t address, uint8_t value);
int memory_read_word(uint64_t address, uint16_t* value);
int memory_write_word(uint64_t address, uint16_t value);
int memory_read_dword(uint64_t address, uint32_t* value);
int memory_write_dword(uint64_t address, uint32_t value);
int memory_read_qword(uint64_t address, uint64_t* value);
int memory_write_qword(uint64_t address, uint64_t value);

// Memory inspection
int memory_dump(uint64_t address, size_t size, memory_dump_format_t format, char* output, size_t output_size);
int memory_dump_hex(uint64_t address, size_t size, char* output, size_t output_size);
int memory_dump_ascii(uint64_t address, size_t size, char* output, size_t output_size);
int memory_dump_binary(uint64_t address, size_t size, char* output, size_t output_size);
int memory_dump_disassembly(uint64_t address, size_t size, char* output, size_t output_size);

// Memory search
int memory_search_byte(uint64_t start, uint64_t end, uint8_t pattern, memory_search_result_t* result);
int memory_search_word(uint64_t start, uint64_t end, uint16_t pattern, memory_search_result_t* result);
int memory_search_dword(uint64_t start, uint64_t end, uint32_t pattern, memory_search_result_t* result);
int memory_search_qword(uint64_t start, uint64_t end, uint64_t pattern, memory_search_result_t* result);
int memory_search_pattern(uint64_t start, uint64_t end, const uint8_t* pattern, size_t pattern_size, memory_search_result_t* result);
int memory_search_string(uint64_t start, uint64_t end, const char* string, memory_search_result_t* result);

// Memory comparison
int memory_compare(uint64_t addr1, uint64_t addr2, size_t size, bool* equal);
int memory_compare_regions(const memory_region_info_t* region1, const memory_region_info_t* region2, bool* equal);

// Memory region management
int memory_get_regions(memory_region_info_t* regions, size_t max_count, size_t* actual_count);
int memory_get_region_info(uint64_t address, memory_region_info_t* info);
int memory_allocate_region(size_t size, memory_access_mode_t access, memory_region_info_t* region);
int memory_free_region(uint64_t address);
int memory_protect_region(uint64_t address, size_t size, memory_access_mode_t access);

// Memory statistics
typedef struct {
    uint64_t total_memory;
    uint64_t allocated_memory;
    uint64_t free_memory;
    uint64_t kernel_memory;
    uint64_t user_memory;
    size_t region_count;
    size_t allocation_count;
} memory_statistics_t;

int memory_get_statistics(memory_statistics_t* stats);

// Memory validation
int memory_validate_address(uint64_t address, size_t size, memory_access_mode_t access);
int memory_is_accessible(uint64_t address, size_t size, memory_access_mode_t access);
int memory_is_aligned(uint64_t address, size_t alignment);

// Memory copying and moving
int memory_copy_region(uint64_t dest, uint64_t src, size_t size);
int memory_move_region(uint64_t dest, uint64_t src, size_t size);
int memory_fill_region(uint64_t address, size_t size, uint8_t value);
int memory_zero_region(uint64_t address, size_t size);

// Memory pattern operations
int memory_fill_pattern(uint64_t address, size_t size, const uint8_t* pattern, size_t pattern_size);
int memory_find_pattern(uint64_t start, uint64_t end, const uint8_t* pattern, size_t pattern_size, uint64_t* result);

// Memory debugging
int memory_debug_print_regions(void);
int memory_debug_print_allocations(void);
int memory_debug_print_statistics(void);
int memory_debug_validate_all(void);

// Memory tools command interface
int memory_tools_command(const char* command, char* output, size_t output_size);

// Built-in memory commands
int memory_cmd_dump(int argc, char** argv);
int memory_cmd_search(int argc, char** argv);
int memory_cmd_compare(int argc, char** argv);
int memory_cmd_alloc(int argc, char** argv);
int memory_cmd_free(int argc, char** argv);
int memory_cmd_protect(int argc, char** argv);
int memory_cmd_regions(int argc, char** argv);
int memory_cmd_stats(int argc, char** argv);
int memory_cmd_validate(int argc, char** argv);

// Memory tools configuration
typedef struct {
    bool enable_logging;
    bool enable_validation;
    bool enable_statistics;
    size_t max_search_size;
    size_t max_dump_size;
    bool auto_align;
} memory_tools_config_t;

int memory_tools_set_config(const memory_tools_config_t* config);
int memory_tools_get_config(memory_tools_config_t* config);

// Memory tools error handling
typedef enum {
    MEMORY_TOOLS_ERROR_NONE = 0,
    MEMORY_TOOLS_ERROR_INVALID_ADDRESS,
    MEMORY_TOOLS_ERROR_INVALID_SIZE,
    MEMORY_TOOLS_ERROR_ACCESS_DENIED,
    MEMORY_TOOLS_ERROR_NOT_FOUND,
    MEMORY_TOOLS_ERROR_OUT_OF_MEMORY,
    MEMORY_TOOLS_ERROR_ALIGNMENT,
    MEMORY_TOOLS_ERROR_UNKNOWN
} memory_tools_error_t;

int memory_tools_get_last_error(memory_tools_error_t* error, char* message, size_t message_size);
int memory_tools_clear_error(void);

#endif // MEMORY_TOOLS_H
