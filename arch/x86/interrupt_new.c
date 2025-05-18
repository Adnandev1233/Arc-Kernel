#include <interrupt.h>
#include <io.h>

// Global IDT table and pointer
struct idt_entry idt[256];
struct idt_ptr idtp;

void init_idt(void) {
    // Set up IDT pointer
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base = (uint32_t)&idt;

    // Clear IDT
    for(int i = 0; i < 256; i++) {
        setup_idt_entry(i, 0, 0, 0);
    }

    // Set up CPU exception handlers (ISRs 0-15)
    setup_idt_entry(0, (uint32_t)isr0, 0x08, 0x8E);   // Divide by zero
    setup_idt_entry(1, (uint32_t)isr1, 0x08, 0x8E);   // Debug
    setup_idt_entry(2, (uint32_t)isr2, 0x08, 0x8E);   // Non-maskable interrupt
    setup_idt_entry(3, (uint32_t)isr3, 0x08, 0x8E);   // Breakpoint
    setup_idt_entry(4, (uint32_t)isr4, 0x08, 0x8E);   // Overflow
    setup_idt_entry(5, (uint32_t)isr5, 0x08, 0x8E);   // Bound range exceeded
    setup_idt_entry(6, (uint32_t)isr6, 0x08, 0x8E);   // Invalid opcode
    setup_idt_entry(7, (uint32_t)isr7, 0x08, 0x8E);   // Device not available
    setup_idt_entry(8, (uint32_t)isr8, 0x08, 0x8E);   // Double fault
    setup_idt_entry(9, (uint32_t)isr9, 0x08, 0x8E);   // Coprocessor segment overrun
    setup_idt_entry(10, (uint32_t)isr10, 0x08, 0x8E); // Invalid TSS
    setup_idt_entry(11, (uint32_t)isr11, 0x08, 0x8E); // Segment not present
    setup_idt_entry(12, (uint32_t)isr12, 0x08, 0x8E); // Stack segment fault
    setup_idt_entry(13, (uint32_t)isr13, 0x08, 0x8E); // General protection fault
    setup_idt_entry(14, (uint32_t)isr14, 0x08, 0x8E); // Page fault
    setup_idt_entry(15, (uint32_t)isr15, 0x08, 0x8E); // Reserved

    // Set up hardware interrupts (IRQs)
    setup_idt_entry(32, (uint32_t)timer_irq_handler, 0x08, 0x8E);  // Timer (IRQ0)
    setup_idt_entry(33, (uint32_t)keyboard_irq_handler, 0x08, 0x8E); // Keyboard (IRQ1)

    // Load IDT
    __asm__ __volatile__("lidt %0" : : "m"(idtp));

    // Clear any pending interrupts
    port_out_byte(0x20, 0x20);
    port_out_byte(0xA0, 0x20);
} 