#include "../include/idt.h"
#include <stdint.h>
#include <io.h>

// IDT entries array
static struct idt_entry idt[256];
static struct idt_ptr idtp;

// External assembly functions
extern void load_idt(struct idt_ptr* ptr);
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

// Set an IDT gate
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].selector = sel;
    idt[num].zero = 0;
    idt[num].flags = flags;
}

// Initialize the IDT
void init_idt(void) {
    // Set up IDT pointer
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base = (uint32_t)&idt;

    // Clear IDT
    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, 0, 0, 0);
    }

    // Set up CPU exception handlers (ISRs 0-15)
    idt_set_gate(0, (uint32_t)isr0, 0x08, 0x8E);   // Divide by zero
    idt_set_gate(1, (uint32_t)isr1, 0x08, 0x8E);   // Debug
    idt_set_gate(2, (uint32_t)isr2, 0x08, 0x8E);   // Non-maskable interrupt
    idt_set_gate(3, (uint32_t)isr3, 0x08, 0x8E);   // Breakpoint
    idt_set_gate(4, (uint32_t)isr4, 0x08, 0x8E);   // Overflow
    idt_set_gate(5, (uint32_t)isr5, 0x08, 0x8E);   // Bound range exceeded
    idt_set_gate(6, (uint32_t)isr6, 0x08, 0x8E);   // Invalid opcode
    idt_set_gate(7, (uint32_t)isr7, 0x08, 0x8E);   // Device not available
    idt_set_gate(8, (uint32_t)isr8, 0x08, 0x8E);   // Double fault
    idt_set_gate(9, (uint32_t)isr9, 0x08, 0x8E);   // Coprocessor segment overrun
    idt_set_gate(10, (uint32_t)isr10, 0x08, 0x8E); // Invalid TSS
    idt_set_gate(11, (uint32_t)isr11, 0x08, 0x8E); // Segment not present
    idt_set_gate(12, (uint32_t)isr12, 0x08, 0x8E); // Stack segment fault
    idt_set_gate(13, (uint32_t)isr13, 0x08, 0x8E); // General protection fault
    idt_set_gate(14, (uint32_t)isr14, 0x08, 0x8E); // Page fault
    idt_set_gate(15, (uint32_t)isr15, 0x08, 0x8E); // Reserved

    // Set up hardware interrupts (IRQs)
    idt_set_gate(32, (uint32_t)timer_irq_handler, 0x08, 0x8E);  // Timer (IRQ0)
    idt_set_gate(33, (uint32_t)keyboard_irq_handler, 0x08, 0x8E); // Keyboard (IRQ1)

    // Load IDT
    load_idt(&idtp);

    // Clear any pending interrupts
    port_out_byte(0x20, 0x20);  // Send EOI to master PIC
    port_out_byte(0xA0, 0x20);  // Send EOI to slave PIC
} 