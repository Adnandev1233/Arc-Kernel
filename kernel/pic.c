#include "include/pic.h"
#include "include/io.h"

// PIC ports
#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

// ICW1 commands
#define ICW1_ICW4       0x01
#define ICW1_SINGLE     0x02
#define ICW1_INTERVAL4  0x04
#define ICW1_LEVEL      0x08
#define ICW1_INIT       0x10

// ICW4 commands
#define ICW4_8086       0x01
#define ICW4_AUTO       0x02
#define ICW4_BUF_SLAVE  0x08
#define ICW4_BUF_MASTER 0x0C
#define ICW4_SFNM       0x10

void init_pic(void) {
    // Save masks
    uint8_t mask1 = port_in_byte(PIC1_DATA);
    uint8_t mask2 = port_in_byte(PIC2_DATA);

    // Start initialization sequence
    port_out_byte(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    port_out_byte(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();

    // ICW2: Set vector offsets
    port_out_byte(PIC1_DATA, 0x20);  // IRQ 0-7: interrupts 32-39
    io_wait();
    port_out_byte(PIC2_DATA, 0x28);  // IRQ 8-15: interrupts 40-47
    io_wait();

    // ICW3: Tell Master PIC that there is a slave PIC at IRQ2
    port_out_byte(PIC1_DATA, 4);
    io_wait();
    // ICW3: Tell Slave PIC its cascade identity
    port_out_byte(PIC2_DATA, 2);
    io_wait();

    // ICW4: Set 8086 mode
    port_out_byte(PIC1_DATA, ICW4_8086);
    io_wait();
    port_out_byte(PIC2_DATA, ICW4_8086);
    io_wait();

    // Restore saved masks
    port_out_byte(PIC1_DATA, mask1);
    port_out_byte(PIC2_DATA, mask2);
} 