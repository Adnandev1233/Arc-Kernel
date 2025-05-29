#ifndef IO_H
#define IO_H

#include <stdint.h>

// I/O initialization
void io_init(void);

// Output a byte to a port
void port_out_byte(uint16_t port, uint8_t value);

// Input a byte from a port
uint8_t port_in_byte(uint16_t port);

// Output a word to a port
void port_out_word(uint16_t port, uint16_t value);

// Input a word from a port
uint16_t port_in_word(uint16_t port);

// Output a double word to a port
void port_out_dword(uint16_t port, uint32_t value);

// Input a double word from a port
uint32_t port_in_dword(uint16_t port);

// Wait for a short time
void io_wait(void);

#endif // IO_H 