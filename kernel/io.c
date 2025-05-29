#include "include/io.h"

#ifdef __GNUC__
#define ASM __asm__ __volatile__
#else
#define ASM asm volatile
#endif

// Initialize I/O ports
void io_init(void) {
    // Initialize PIC
    port_out_byte(0x20, 0x11);  // ICW1: edge triggered, cascade mode, ICW4 needed
    port_out_byte(0xA0, 0x11);
    port_out_byte(0x21, 0x20);  // ICW2: IRQ0-7 mapped to interrupts 0x20-0x27
    port_out_byte(0xA1, 0x28);  // ICW2: IRQ8-15 mapped to interrupts 0x28-0x2F
    port_out_byte(0x21, 0x04);  // ICW3: slave PIC at IRQ2
    port_out_byte(0xA1, 0x02);
    port_out_byte(0x21, 0x01);  // ICW4: 8086 mode
    port_out_byte(0xA1, 0x01);
    
    // Mask all interrupts except keyboard
    port_out_byte(0x21, 0xFD);  // Enable IRQ1 (keyboard)
    port_out_byte(0xA1, 0xFF);  // Disable all slave IRQs
}

// Output a byte to a port
void port_out_byte(uint16_t port, uint8_t value) {
    __asm__ __volatile__("outb %b0, %w1" : : "a"(value), "Nd"(port));
}

// Input a byte from a port
uint8_t port_in_byte(uint16_t port) {
    uint8_t result;
    __asm__ __volatile__("inb %w1, %b0" : "=a"(result) : "Nd"(port));
    return result;
}

// Output a word to a port
void port_out_word(uint16_t port, uint16_t value) {
    __asm__ __volatile__("outw %w0, %w1" : : "a"(value), "Nd"(port));
}

// Input a word from a port
uint16_t port_in_word(uint16_t port) {
    uint16_t result;
    __asm__ __volatile__("inw %w1, %w0" : "=a"(result) : "Nd"(port));
    return result;
}

// Output a double word to a port
void port_out_dword(uint16_t port, uint32_t value) {
    __asm__ __volatile__("outl %0, %w1" : : "a"(value), "Nd"(port));
}

// Input a double word from a port
uint32_t port_in_dword(uint16_t port) {
    uint32_t result;
    __asm__ __volatile__("inl %w1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

// Wait for a short time
void io_wait(void) {
    port_out_byte(0x80, 0);
} 