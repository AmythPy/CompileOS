#define true 1
#define false 0
/**
 * CompileOS V0.01a  Kernel - Jesus Is King!
 * 
 * This is the main kernel entry point for CompileOS V0.01a.
 * It initializes the core systems and starts the runtime environment.
 */

#include <stdint.h>
#include <stddef.h>
// #include "stubs/stub.h"
#include "kernel.h"
#include "hal/hal.h"
#include "memory/memory.h"
#include "memory/multibit.h"
#include "memory/memory_tools.h"
#include "process/process.h"
#include "debugger/debugger.h"
#include "terminal/terminal.h"
#include "repl/repl.h"
#include "desktop/desktop.h"

// Kernel version information
#define KERNEL_VERSION_MAJOR 0
#define KERNEL_VERSION_MINOR 1
#define KERNEL_VERSION_PATCH 0
#define KERNEL_VERSION_STRING "V0.01a"

// Global kernel state
kernel_state_t g_kernel_state = {0};

/**
 * Early kernel initialization
 * Sets up basic hardware and memory management
 */
static int kernel_early_init(void) {
    // Initialize hardware abstraction layer
    if (hal_init() != 0) {
        return -1;
    }
    
    // Initialize memory management
    if (memory_init() != 0) {
        return -1;
    }
    
    return 0;
}

/**
 * Main kernel initialization
 * Sets up all kernel subsystems
 */
static int kernel_init(void) {
    // Initialize process management
    if (process_init() != 0) {
        return -1;
    }

    // Initialize multi-bit memory system
    if (multibit_init() != 0) {
        return -1;
    }

    // Initialize memory tools
    if (memory_tools_init() != 0) {
        return -1;
    }

    // Initialize debugger
    if (debugger_init() != 0) {
        return -1;
    }

    // Initialize terminal
    if (terminal_init() != 0) {
        return -1;
    }

    // Initialize REPL
    if (repl_init() != 0) {
        return -1;
    }

    // Initialize desktop
    if (desktop_init() != 0) {
        return -1;
    }

    return 0;
}
    
    // Initialize device drivers
    // TODO: Implement device driver initialization
    
    return 0;
}

/**
 * Main kernel entry point
 * Called by the bootloader
 */
void kernel_main(void) {
    // Set up kernel state
    g_kernel_state.version_major = KERNEL_VERSION_MAJOR;
    g_kernel_state.version_minor = KERNEL_VERSION_MINOR;
    g_kernel_state.version_patch = KERNEL_VERSION_PATCH;
    g_kernel_state.status = KERNEL_STATUS_INITIALIZING;
    
    // Early initialization
    if (kernel_early_init() != 0) {
        g_kernel_state.status = KERNEL_STATUS_ERROR;
        // TODO: Implement proper error handling
        return;
    }
    
    // Main initialization
    if (kernel_init() != 0) {
        g_kernel_state.status = KERNEL_STATUS_ERROR;
        // TODO: Implement proper error handling
        return;
    }
    
    // Mark kernel as running
    g_kernel_state.status = KERNEL_STATUS_RUNNING;
    
    // Display boot message
    terminal_printf("CompileOS v%s - Hardware Agnostic Development Platform\n", kernel_get_version_string());
    terminal_printf("Architecture: %s\n", hal_get_cpu_architecture_string());
    terminal_printf("Initializing development environment...\n");
    
    // Start desktop
    desktop_start();
    
    // Start terminal
    terminal_start();
    
    // Start REPL
    repl_start();
    
    // Main kernel loop
    while (g_kernel_state.status == KERNEL_STATUS_RUNNING) {
        // Handle terminal input
        terminal_handle_input(0); // This will be called by interrupt handlers
        
        // Process system calls
        // TODO: Implement system call handling
        
        // Handle interrupts
        // TODO: Implement interrupt handling
        
        // Yield to other processes
        process_schedule();
        
        // Small delay to prevent busy waiting
        hal_halt();
    }
}

/**
 * Get kernel version string
 */
const char* kernel_get_version_string(void) {
    return KERNEL_VERSION_STRING;
}

/**
 * Get kernel state
 */
const kernel_state_t* kernel_get_state(void) {
    return &g_kernel_state;
}
