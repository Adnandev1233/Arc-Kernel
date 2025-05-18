#ifndef _INTERRUPT_H
#define _INTERRUPT_H

#include <stdint.h>

// Compiler-specific definitions
#if defined(__GNUC__)
    #define PACKED __attribute__((packed))
#else
    #define PACKED
#endif

// IDT entry structure
struct idt_entry {
    uint16_t base_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t flags;
    uint16_t base_high;
} PACKED;

// IDT pointer structure
struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} PACKED;

// Function declarations
void init_idt(void);
void idt_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags);

// External assembly functions
extern void setup_idt_entry(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags);

// ISR handlers (CPU exceptions)
extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);

// IRQ handlers
extern void keyboard_irq_handler(void);
extern void timer_irq_handler(void);

#endif 