#ifndef E1000_H
#define E1000_H

#include <stdint.h>
#include <stdbool.h>
#include "network.h"

// E1000 device structure
typedef struct {
    uint32_t io_base;      // I/O base address
    uint32_t mem_base;     // Memory-mapped base address
    uint8_t mac_address[6];
    bool is_initialized;
} E1000Device;

// Initialize E1000 device
bool e1000_init(E1000Device* device, uint32_t io_base, uint32_t mem_base);

// Shutdown E1000 device
void e1000_shutdown(E1000Device* device);

// Send packet through E1000
bool e1000_send_packet(E1000Device* device, NetworkPacket* packet);

// Receive packet from E1000
bool e1000_receive_packet(E1000Device* device, NetworkPacket* packet);

// Handle E1000 interrupt
void e1000_handle_interrupt(E1000Device* device);

// Get E1000 MAC address
void e1000_get_mac_address(E1000Device* device, uint8_t* mac_address);

// Set E1000 MAC address
void e1000_set_mac_address(E1000Device* device, const uint8_t* mac_address);

// Enable/disable E1000 device
void e1000_set_device_state(E1000Device* device, bool enabled);

// Check if E1000 device is up
bool e1000_is_device_up(E1000Device* device);

#endif // E1000_H 