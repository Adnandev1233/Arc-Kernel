#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>

// Memory allocation functions
void* kmalloc(size_t size);
void kfree(void* ptr);

// Memory constants
#define PAGE_SIZE 4096
#define MAX_PAGES 1024

#endif // MEMORY_H 