#ifndef MEMORY_H
#define MEMORY_H

#include "stdint.h"
#include "stdbool.h"

// Memory management constants
#define PAGE_SIZE 4096
#define MAX_PAGES 1048576  // 4GB / 4KB = 1048576 pages

// Memory management globals
extern uint32_t* page_directory;
extern uint32_t* page_tables[1024];
extern bool pages_used[MAX_PAGES];

// Memory management functions
void* kmalloc(size_t size);
void kfree(void* ptr);
bool init_paging(void);
void enable_paging(uint32_t* directory);

#endif /* MEMORY_H */ 