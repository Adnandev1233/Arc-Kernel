#ifndef STDLIB_H
#define STDLIB_H

#include <stddef.h>

// Memory allocation
void* malloc(size_t size);
void free(void* ptr);
void* calloc(size_t nmemb, size_t size);
void* realloc(void* ptr, size_t size);

// String conversion
int atoi(const char* str);
long atol(const char* str);
long long atoll(const char* str);

// Random numbers
int rand(void);
void srand(unsigned int seed);

// Program termination
void abort(void);
void exit(int status);

// Environment
char* getenv(const char* name);
int setenv(const char* name, const char* value, int overwrite);
int unsetenv(const char* name);

// Memory operations
void* memcpy(void* dest, const void* src, size_t n);
void* memmove(void* dest, const void* src, size_t n);
void* memset(void* s, int c, size_t n);
int memcmp(const void* s1, const void* s2, size_t n);

#endif // STDLIB_H 