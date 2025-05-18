#include <io.h>
#include <pic.h>

#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

// PIC initialization constants
#define ICW1_ICW4   0x01
#define ICW1_SINGLE 0x02
#define ICW1_INTERVAL4  0x04
#define ICW1_LEVEL  0x08
#define ICW1_INIT   0x10

#define ICW4_8086   0x01
#define ICW4_AUTO   0x02
#define ICW4_BUF_SLAVE  0x08
#define ICW4_BUF_MASTER 0x0C
#define ICW4_SFNM   0x10

// Remap PIC interrupts to not conflict with CPU exceptions
void init_pic(void) {
    uint8_t a1, a2;

    // Save masks
    a1 = port_in_byte(PIC1_DATA);
    a2 = port_in_byte(PIC2_DATA);

    // Start initialization sequence
    port_out_byte(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    port_out_byte(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);

    // Set vector offsets
    port_out_byte(PIC1_DATA, 0x20);  // IRQ 0-7: interrupts 32-39
    port_out_byte(PIC2_DATA, 0x28);  // IRQ 8-15: interrupts 40-47

    // Tell Master PIC that there is a slave PIC at IRQ2
    port_out_byte(PIC1_DATA, 4);
    // Tell Slave PIC its cascade identity
    port_out_byte(PIC2_DATA, 2);

    // Set 8086 mode
    port_out_byte(PIC1_DATA, ICW4_8086);
    port_out_byte(PIC2_DATA, ICW4_8086);

    // Mask all interrupts except keyboard (IRQ1)
    port_out_byte(PIC1_DATA, 0xFD);  // 1111 1101 - only enable IRQ1
    port_out_byte(PIC2_DATA, 0xFF);  // Mask all interrupts on slave PIC

    // Restore saved masks
    port_out_byte(PIC1_DATA, a1);
    port_out_byte(PIC2_DATA, a2);
}

// Send end of interrupt signal to PIC
void send_eoi(void) {
    port_out_byte(PIC1_COMMAND, 0x20);
    port_out_byte(PIC2_COMMAND, 0x20);  // Send to both PICs to be safe
} 