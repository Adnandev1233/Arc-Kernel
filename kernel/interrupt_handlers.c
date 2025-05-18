#include <stdint.h>
#include <io.h>
#include "kernel.h"
#include "include/interrupt.h"  // Using the kernel version which has interrupt_frame defined
#include <terminal.h>

// External function declarations
extern void init_terminal(void);
extern void update_cursor(void);
extern void terminal_writestring(const char* data);
extern void itoa(int value, char* str, int base);

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
        // Timer interrupt (IRQ0)
        if (frame->int_no == 32) {
            timer_handler();
        }
        // Send EOI for hardware interrupts
        send_eoi(frame->int_no);
    }
}

// Timer handler
void timer_handler(void) {
    timer_ticks++;
    if (timer_ticks % 100 == 0) {  // Update cursor every second (assuming 100Hz timer)
        update_cursor();
    }
}

// Get system uptime in ticks
uint32_t get_timer_ticks(void) {
    return timer_ticks;
} 