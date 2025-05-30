#ifndef CPU_H
#define CPU_H

#include <stdint.h>

// CPU control functions
#ifdef __GNUC__
static inline void enable_interrupts(void) {
    asm volatile ("sti");
}

static inline void disable_interrupts(void) {
    asm volatile ("cli");
}

static inline void halt_cpu(void) {
    asm volatile ("hlt");
}
#else
void enable_interrupts(void);
void disable_interrupts(void);
void halt_cpu(void);
#endif

// Paging control
void enable_paging(uint32_t* page_dir);

// CPU feature detection
void detect_cpu_features(void);

#endif /* CPU_H */ 