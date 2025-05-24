#include "../include/io.h"
#include "../include/terminal.h"
#include "../include/keyboard.h"
#include "../include/interrupt_handlers.h"
#include "../include/irq.h"
#include "../kernel/include/interrupt.h"
#include <stdint.h>
#include "kernel.h"

// External function declarations
extern void init_terminal(void);
extern void update_cursor(void);
extern void terminal_writestring(const char* data);
extern void itoa(int value, char* str, int base);
extern void init_pic(void);  // Declare external init_pic

// Exception messages for CPU exceptions
static const char* exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt"
};

// System timer counter
static volatile uint32_t timer_ticks = 0;

// Send EOI (End of Interrupt) to PIC
void send_eoi(uint32_t int_no) {
    if (int_no >= 40) {
        port_out_byte(0xA0, 0x20); // Send EOI to slave PIC
    }
    port_out_byte(0x20, 0x20); // Send EOI to master PIC
}

// ISR handler
void isr_handler(struct interrupt_frame* frame) {
    // Handle CPU exceptions (interrupts 0-31)
    if (frame->int_no < 32) {
        terminal_writestring("Exception: ");
        if (frame->int_no < 16) {
            terminal_writestring(exception_messages[frame->int_no]);
        } else {
            terminal_writestring("Reserved Exception");
        }
        terminal_writestring("\n");
        
        // Halt the system
        for(;;);
    }
    // Handle hardware interrupts (32+)
    else {
        irq_handler((struct regs*)frame);
    }
}

// IRQ handler
void irq_handler(struct regs* r) {
    // Always send EOI first to prevent interrupt storms
    send_eoi(r->int_no);

    // Handle timer interrupt (IRQ0)
    if (r->int_no == 32) {
        timer_ticks++;
        if (timer_ticks % 100 == 0) {  // Update cursor every second (assuming 100Hz timer)
            update_cursor();
        }
    }
    // Handle keyboard interrupt (IRQ1)
    else if (r->int_no == 33) {
        // Read keyboard scan code
        uint8_t scancode = port_in_byte(0x60);
        
        // Handle key press/release
        if (scancode < 0x80) {
            // Key press
            char c = keyboard_getchar();
            if (c) {
                terminal_putchar(c);
            }
        }
    }
    // Handle other IRQs (just acknowledge them)
    else {
        // Unhandled IRQ - just acknowledge it
        terminal_writestring("Unhandled IRQ: ");
        char irq_str[4];
        itoa(r->int_no - 32, irq_str, 10);
        terminal_writestring(irq_str);
        terminal_writestring("\n");
    }
}

// Get system uptime in ticks
uint32_t get_timer_ticks(void) {
    return timer_ticks;
} 