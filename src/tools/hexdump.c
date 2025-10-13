/**
 * CompileOS Hex Dump Tool
 * 
 * Display file contents in hexadecimal format
 */

#include "../runtime/runtime.h"
#include <string.h>

static void print_hex_line(const uint8_t* data, size_t offset, size_t length) {
    // Print offset
    printf_rt("%08x  ", (unsigned int)offset);
    
    // Print hex bytes
    for (size_t i = 0; i < 16; i++) {
        if (i < length) {
            printf_rt("%02x ", data[i]);
        } else {
            print("   ");
        }
        
        if (i == 7) print(" ");
    }
    
    print(" |");
    
    // Print ASCII
    for (size_t i = 0; i < length; i++) {
        char c = data[i];
        if (c >= 32 && c < 127) {
            printf_rt("%c", c);
        } else {
            print(".");
        }
    }
    
    println("|");
}

int hexdump_main(const char* filename) {
    if (!filename) {
        println("Usage: hexdump <filename>");
        return -1;
    }
    
    // Read file
    char buffer[4096];
    int bytes = rt_file_read(filename, buffer, sizeof(buffer));
    
    if (bytes < 0) {
        println("Error reading file!");
        return -1;
    }
    
    printf_rt("\nHex dump of '%s' (%d bytes):\n\n", filename, bytes);
    
    // Print hex dump
    for (int offset = 0; offset < bytes; offset += 16) {
        size_t length = (offset + 16 <= bytes) ? 16 : (bytes - offset);
        print_hex_line((uint8_t*)&buffer[offset], offset, length);
    }
    
    println("");
    return 0;
}
