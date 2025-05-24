#ifndef IRQ_H
#define IRQ_H

#include <stdint.h>

// Structure to hold register values during interrupt
struct regs {
    uint32_t gs, fs, es, ds;                         // Segment registers
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // General purpose registers
    uint32_t int_no, err_code;                       // Interrupt number and error code
    uint32_t eip, cs, eflags, useresp, ss;          // Pushed by CPU automatically
};

// Function declarations
void init_pic(void);  // Initialize PIC and enable interrupts
void irq_handler(struct regs* r);

#endif // IRQ_H 