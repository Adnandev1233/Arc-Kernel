#ifndef STDIO_H
#define STDIO_H

#include <stdarg.h>
#include <stddef.h>

// Minimal stdio functions
int sprintf(char* str, const char* format, ...);
int vsprintf(char* str, const char* format, va_list args);
int snprintf(char* str, size_t size, const char* format, ...);
int vsnprintf(char* str, size_t size, const char* format, va_list args);

#endif // STDIO_H 