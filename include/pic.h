#ifndef _PIC_H
#define _PIC_H

#include <io.h>

// PIC ports
#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

void init_pic(void);
void send_eoi(void);

#endif /* _PIC_H */ 