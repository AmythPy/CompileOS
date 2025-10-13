/**
 * CompileOS Utility Functions
 * Standard library functions for freestanding environment
 */

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>

// Character type functions (replacement for ctype.h)
int isdigit(int c) {
    return c >= '0' && c <= '9';
}

int isalpha(int c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int isalnum(int c) {
    return isdigit(c) || isalpha(c);
}

int isspace(int c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
}

int isupper(int c) {
    return c >= 'A' && c <= 'Z';
}

int islower(int c) {
    return c >= 'a' && c <= 'z';
}

int toupper(int c) {
    if (islower(c)) return c - 32;
    return c;
}

int tolower(int c) {
    if (isupper(c)) return c + 32;
    return c;
}

// Memory functions
void* memset(void* dest, int val, size_t len) {
    unsigned char* ptr = dest;
    while (len--) *ptr++ = (unsigned char)val;
    return dest;
}

void* memcpy(void* dest, const void* src, size_t len) {
    unsigned char* d = dest;
    const unsigned char* s = src;
    while (len--) *d++ = *s++;
    return dest;
}

void* memmove(void* dest, const void* src, size_t len) {
    unsigned char* d = dest;
    const unsigned char* s = src;
    if (d < s) {
        while (len--) *d++ = *s++;
    } else {
        d += len;
        s += len;
        while (len--) *--d = *--s;
    }
    return dest;
}

int memcmp(const void* s1, const void* s2, size_t n) {
    const unsigned char* p1 = s1;
    const unsigned char* p2 = s2;
    while (n--) {
        if (*p1 != *p2) return *p1 - *p2;
        p1++;
        p2++;
    }
    return 0;
}

// String functions
size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len]) len++;
    return len;
}

char* strcpy(char* dest, const char* src) {
    char* d = dest;
    while ((*d++ = *src++));
    return dest;
}

char* strncpy(char* dest, const char* src, size_t n) {
    size_t i;
    for (i = 0; i < n && src[i]; i++) {
        dest[i] = src[i];
    }
    for (; i < n; i++) {
        dest[i] = '\0';
    }
    return dest;
}

int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

int strncmp(const char* s1, const char* s2, size_t n) {
    while (n && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
        n--;
    }
    if (n == 0) return 0;
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

char* strcat(char* dest, const char* src) {
    char* d = dest;
    while (*d) d++;
    while ((*d++ = *src++));
    return dest;
}

char* strchr(const char* str, int c) {
    while (*str) {
        if (*str == (char)c) return (char*)str;
        str++;
    }
    return NULL;
}

// Memory allocation wrappers (stub implementations for standalone build)
void* memory_alloc(size_t size) {
    return (void*)0x200000; // Return a fixed address for now
}

void memory_free(void* ptr) {
    // Stub implementation
}

void* memory_realloc(void* ptr, size_t size) {
    return memory_alloc(size); // Simple realloc
}

void* malloc(size_t size) {
    return memory_alloc(size);
}

void free(void* ptr) {
    memory_free(ptr);
}

void* calloc(size_t num, size_t size) {
    size_t total = num * size;
    void* ptr = memory_alloc(total);
    if (ptr) memset(ptr, 0, total);
    return ptr;
}

void* realloc(void* ptr, size_t size) {
    return memory_realloc(ptr, size);
}

// Number to string conversion helpers
static void reverse_string(char* str, int length) {
    int start = 0;
    int end = length - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

static int int_to_string(int num, char* str, int base) {
    int i = 0;
    int is_negative = 0;
    
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return i;
    }
    
    if (num < 0 && base == 10) {
        is_negative = 1;
        num = -num;
    }
    
    while (num != 0) {
        int rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }
    
    if (is_negative) str[i++] = '-';
    
    str[i] = '\0';
    reverse_string(str, i);
    
    return i;
}

static int uint_to_string(unsigned int num, char* str, int base) {
    int i = 0;
    
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return i;
    }
    
    while (num != 0) {
        int rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }
    
    str[i] = '\0';
    reverse_string(str, i);
    
    return i;
}

static int ulong_to_string(unsigned long num, char* str, int base) {
    int i = 0;
    
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return i;
    }
    
    while (num != 0) {
        int rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }
    
    str[i] = '\0';
    reverse_string(str, i);
    
    return i;
}

// Basic sprintf implementation
int vsprintf(char* str, const char* format, va_list args) {
    int written = 0;
    char buffer[32];
    
    while (*format) {
        if (*format == '%') {
            format++;
            switch (*format) {
                case 'd':
                case 'i': {
                    int val = va_arg(args, int);
                    int len = int_to_string(val, buffer, 10);
                    memcpy(str + written, buffer, len);
                    written += len;
                    break;
                }
                case 'u': {
                    unsigned int val = va_arg(args, unsigned int);
                    int len = uint_to_string(val, buffer, 10);
                    memcpy(str + written, buffer, len);
                    written += len;
                    break;
                }
                case 'x': {
                    unsigned int val = va_arg(args, unsigned int);
                    int len = uint_to_string(val, buffer, 16);
                    memcpy(str + written, buffer, len);
                    written += len;
                    break;
                }
                case 'X': {
                    unsigned int val = va_arg(args, unsigned int);
                    int len = uint_to_string(val, buffer, 16);
                    for (int i = 0; i < len; i++) {
                        if (buffer[i] >= 'a' && buffer[i] <= 'f')
                            buffer[i] = buffer[i] - 'a' + 'A';
                    }
                    memcpy(str + written, buffer, len);
                    written += len;
                    break;
                }
                case 'p': {
                    unsigned long val = (unsigned long)va_arg(args, void*);
                    str[written++] = '0';
                    str[written++] = 'x';
                    int len = ulong_to_string(val, buffer, 16);
                    memcpy(str + written, buffer, len);
                    written += len;
                    break;
                }
                case 's': {
                    char* s = va_arg(args, char*);
                    if (!s) s = "(null)";
                    while (*s) {
                        str[written++] = *s++;
                    }
                    break;
                }
                case 'c': {
                    char c = (char)va_arg(args, int);
                    str[written++] = c;
                    break;
                }
                case '%': {
                    str[written++] = '%';
                    break;
                }
                default:
                    str[written++] = '%';
                    str[written++] = *format;
                    break;
            }
            format++;
        } else {
            str[written++] = *format++;
        }
    }
    
    str[written] = '\0';
    return written;
}

int sprintf(char* str, const char* format, ...) {
    va_list args;
    va_start(args, format);
    int written = vsprintf(str, format, args);
    va_end(args);
    return written;
}

int snprintf(char* str, size_t size, const char* format, ...) {
    va_list args;
    va_start(args, format);
    int written = vsprintf(str, format, args);
    va_end(args);
    if ((size_t)written >= size && size > 0) {
        str[size - 1] = '\0';
        return size - 1;
    }
    return written;
}
