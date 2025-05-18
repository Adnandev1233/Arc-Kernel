#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>
#include "../kernel/include/multiboot.h"

// Function declarations
void kernel_main(uint32_t magic, struct multiboot_info* mboot_ptr);
void kernel_init(void);
void detect_memory(struct multiboot_info* mboot_ptr);
void handle_command(void);

#endif /* KERNEL_H */ 