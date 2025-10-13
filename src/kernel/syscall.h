/**
 * CompileOS System Call Interface
 * INT 0x80 syscall handler
 */

#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

// Syscall numbers
#define SYS_EXIT        1
#define SYS_WRITE       2
#define SYS_READ        3
#define SYS_OPEN        4
#define SYS_CLOSE       5
#define SYS_GETPID      6
#define SYS_SLEEP       7
#define SYS_YIELD       8
#define SYS_MALLOC      9
#define SYS_FREE        10

// Initialize syscall handler
void syscall_init(void);

// Syscall handler (called from interrupt)
void syscall_handler(void);

#endif // SYSCALL_H
