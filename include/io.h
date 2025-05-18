#ifndef _IO_H
#define _IO_H

#include <stdint.h>

// Port I/O functions
uint8_t port_in_byte(uint16_t port);
void port_out_byte(uint16_t port, uint8_t data);

#endif /* _IO_H */ 