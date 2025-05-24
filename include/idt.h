#ifndef IDT_H
#define IDT_H

#include <stdint.h>

// IDT entry structure
struct idt_entry {
    uint16_t base_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t flags;
    uint16_t base_high;
};

// IDT pointer structure
struct idt_ptr {
    uint16_t limit;
    uint32_t base;
};

// IDT functions
void init_idt(void);
void idt_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags);

#endif // IDT_H 