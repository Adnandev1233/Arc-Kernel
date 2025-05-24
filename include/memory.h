#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>
#include <stdint.h>
#include "../kernel/include/multiboot.h"

// Memory management functions
void* kmalloc(size_t size);
void kfree(void* ptr);
bool init_paging(void);

// Memory constants
#define PAGE_SIZE 4096
#define MAX_PAGES 1024

#endif /* MEMORY_H */ 