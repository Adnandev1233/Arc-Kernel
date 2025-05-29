#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Kernel version
#define KERNEL_VERSION "1.0.0"
#define KERNEL_NAME "ArcOS"

// Memory management
#define PAGE_SIZE 4096
#define MAX_PAGES 1024

// Multiboot information structure
typedef struct {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t syms[4];
    uint32_t mmap_length;
    uint32_t mmap_addr;
    uint32_t drives_length;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint32_t vbe_mode;
    uint32_t vbe_interface_seg;
    uint32_t vbe_interface_off;
    uint32_t vbe_interface_len;
} MultibootInfo;

// Function declarations
void kernel_main(uint32_t magic, MultibootInfo* info);
void kernel_init(void);
void* kmalloc(size_t size);
void kfree(void* ptr);
bool init_paging(void);

// External symbols
extern uint64_t start;
extern uint64_t end;

#endif // KERNEL_H 