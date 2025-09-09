#ifndef STUBS_H
#define STUBS_H

#include <stddef.h>
#include <stdint.h>

typedef int bool;
#define true 1
#define false 0

/* Memory */
int multibit_memory_init(void) { return 0; }
int memory_tools_init(void) { return 0; }

/* Debugger */
typedef struct { int dummy; } debugger_command_t;
int debugger_init(void) { return 0; }

/* Terminal */
int terminal_init(void) { return 0; }
int terminal_clear_line(void) { return 0; }
int terminal_printf(const char* fmt, ...) { return 0; }
int terminal_start(void) { return 0; }
int terminal_handle_input(int code) { return 0; }

/* REPL */
int repl_init(void) { return 0; }

/* Desktop */
int desktop_init(void) { return 0; }

/* Kernel */
const char* kernel_get_version_string(void) { return "0.0.0-stub"; }

#endif
