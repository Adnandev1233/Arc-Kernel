#ifndef _INTERRUPT_H
#define _INTERRUPT_H

#include <stdint.h>

// IDT entry structure
struct idt_entry {
    uint16_t base_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t flags;
    uint16_t base_high;
} __attribute__((packed));

// IDT pointer structure
struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

// Interrupt frame structure
struct interrupt_frame {
    uint32_t gs, fs, es, ds;                         // Pushed by us
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha
    uint32_t int_no, err_code;                       // Interrupt number and error code
    uint32_t eip, cs, eflags, useresp, ss;          // Pushed by CPU automatically
};

// Function declarations
void init_idt(void);
void setup_idt_entry(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags);
void isr_handler(struct interrupt_frame* frame);
void timer_handler(void);
void keyboard_handler(void);
void send_eoi(uint32_t int_no);
uint32_t get_timer_ticks(void);

// External assembly functions
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
extern void timer_irq_handler(void);
extern void keyboard_irq_handler(void);

#endif /* _INTERRUPT_H */ 