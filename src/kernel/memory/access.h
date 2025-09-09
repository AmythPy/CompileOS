#ifndef MEMORY_ACCESS_H
#define MEMORY_ACCESS_H

#include <stdint.h>

/**
 * Access permissions for memory pages/regions.
 */
typedef enum {
    MEMORY_ACCESS_READ = 0,
    MEMORY_ACCESS_WRITE,
    MEMORY_ACCESS_EXECUTE
} memory_access_t;

#endif // MEMORY_ACCESS_H
