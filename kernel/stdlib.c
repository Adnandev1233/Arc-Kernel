#include "include/stdlib.h"
#include "include/string.h"

// Simple memory allocator
static char heap[1024 * 1024]; // 1MB heap
static size_t heap_ptr = 0;

void* malloc(size_t size) {
    if (heap_ptr + size > sizeof(heap)) {
        return NULL;
    }
    void* ptr = &heap[heap_ptr];
    heap_ptr += size;
    return ptr;
}

void free(void* ptr) {
    // Simple implementation - no actual freeing
    (void)ptr;
}

void* calloc(size_t nmemb, size_t size) {
    void* ptr = malloc(nmemb * size);
    if (ptr) {
        memset(ptr, 0, nmemb * size);
    }
    return ptr;
}

void* realloc(void* ptr, size_t size) {
    // Simple implementation - just allocate new memory
    void* new_ptr = malloc(size);
    if (new_ptr && ptr) {
        memcpy(new_ptr, ptr, size);
    }
    return new_ptr;
}

// String conversion
int atoi(const char* str) {
    int result = 0;
    int sign = 1;
    
    // Skip whitespace
    while (*str == ' ' || *str == '\t') {
        str++;
    }
    
    // Handle sign
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }
    
    // Convert digits
    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }
    
    return result * sign;
}

long atol(const char* str) {
    return (long)atoi(str);
}

long long atoll(const char* str) {
    return (long long)atoi(str);
}

// Random numbers
static unsigned int rand_seed = 1;

int rand(void) {
    rand_seed = rand_seed * 1103515245 + 12345;
    return (int)(rand_seed >> 16) & 0x7fff;
}

void srand(unsigned int seed) {
    rand_seed = seed;
}

// Program termination
void abort(void) {
    while (1) {
        __asm__ __volatile__("hlt");
    }
}

void exit(int status) {
    (void)status;
    abort();
}

// Environment - not implemented in kernel
char* getenv(const char* name) {
    (void)name;
    return NULL;
}

int setenv(const char* name, const char* value, int overwrite) {
    (void)name;
    (void)value;
    (void)overwrite;
    return -1;
}

int unsetenv(const char* name) {
    (void)name;
    return -1;
} 