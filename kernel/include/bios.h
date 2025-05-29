#ifndef BIOS_H
#define BIOS_H

#include <stdint.h>
#include <stdbool.h>

// Function to switch to 32-bit mode and call BIOS interrupt
bool bios_call(uint8_t interrupt, uint16_t ax, uint16_t bx, uint16_t cx, uint16_t dx,
              uint16_t* out_ax, uint16_t* out_bx, uint16_t* out_cx, uint16_t* out_dx);

// Function to switch to 32-bit mode and call BIOS interrupt with memory buffer
bool bios_call_with_buffer(uint8_t interrupt, uint16_t ax, uint16_t bx, uint16_t cx, uint16_t dx,
                          void* buffer, uint16_t* out_ax);

#endif // BIOS_H 