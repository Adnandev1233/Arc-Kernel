#include "include/memory.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// Simple memory block structure
typedef struct MemoryBlock {
    size_t size;
    bool used;
    struct MemoryBlock* next;
} MemoryBlock;

// Memory pool
static uint8_t memory_pool[PAGE_SIZE * MAX_PAGES];
static MemoryBlock* free_list = NULL;

// Initialize memory management
void memory_init(void) {
    // Initialize the first block to cover the entire pool
    free_list = (MemoryBlock*)memory_pool;
    free_list->size = sizeof(memory_pool) - sizeof(MemoryBlock);
    free_list->used = false;
    free_list->next = NULL;
}

// Allocate memory
void* kmalloc(size_t size) {
    if (size == 0) return NULL;
    
    // Add alignment padding
    size = (size + 7) & ~7;
    
    MemoryBlock* current = free_list;
    while (current) {
        if (!current->used && current->size >= size) {
            // Split block if it's large enough
            if (current->size >= size + sizeof(MemoryBlock) + 8) {
                MemoryBlock* new_block = (MemoryBlock*)((uint8_t*)current + sizeof(MemoryBlock) + size);
                new_block->size = current->size - size - sizeof(MemoryBlock);
                new_block->used = false;
                new_block->next = current->next;
                
                current->size = size;
                current->next = new_block;
            }
            
            current->used = true;
            return (void*)((uint8_t*)current + sizeof(MemoryBlock));
        }
        current = current->next;
    }
    
    return NULL; // Out of memory
}

// Free memory
void kfree(void* ptr) {
    if (!ptr) return;
    
    MemoryBlock* block = (MemoryBlock*)((uint8_t*)ptr - sizeof(MemoryBlock));
    block->used = false;
    
    // Merge adjacent free blocks
    MemoryBlock* current = free_list;
    while (current) {
        if (!current->used && current->next && !current->next->used) {
            current->size += sizeof(MemoryBlock) + current->next->size;
            current->next = current->next->next;
        }
        current = current->next;
    }
} 