#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>
#include <stdbool.h>

// Function declarations
void enable_paging(uint32_t* page_dir);
bool init_paging(void);

#endif // PAGING_H 