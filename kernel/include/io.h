#ifndef _IO_H
#define _IO_H

#include <stdint.h>

// Port I/O functions
void port_out_byte(uint16_t port, uint8_t data);
uint8_t port_in_byte(uint16_t port);
void io_wait(void);

#endif /* _IO_H */ 