#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "../include/terminal.h"
#include "network.h"

// Maximum number of network devices
#define MAX_NETWORK_DEVICES 4

// Network device list
static NetworkDevice devices[MAX_NETWORK_DEVICES];
static int device_count = 0;

// Initialize network subsystem
bool network_init(void) {
    // Clear device list
    memset(devices, 0, sizeof(devices));
    device_count = 0;
    
    // TODO: Initialize network hardware
    // This would typically involve:
    // 1. Detecting network cards
    // 2. Initializing each card
    // 3. Setting up DMA
    // 4. Configuring interrupts
    
    return true; // Return true if initialization was successful
}

// Register a network device
int network_register_device(NetworkDevice* device) {
    if (device_count >= MAX_NETWORK_DEVICES) return -1;
    
    // Copy device information
    memcpy(&devices[device_count], device, sizeof(NetworkDevice));
    
    // Initialize device
    device->is_up = false;
    device->is_running = false;
    
    return device_count++;
}

// Send a packet through the specified device
bool network_send_packet(NetworkDevice* device, NetworkPacket* packet) {
    if (!device || !device->is_up || !device->is_running) return false;
    
    // Call device's send function
    if (device->send_packet) {
        device->send_packet(packet);
        return true;
    }
    
    return false;
}

// Receive a packet from the specified device
bool network_receive_packet(NetworkDevice* device, NetworkPacket* packet) {
    if (!device || !device->is_up || !device->is_running) return false;
    
    // Call device's receive function
    if (device->receive_packet) {
        device->receive_packet(packet);
        return true;
    }
    
    return false;
}

// Get device MAC address
void network_get_mac_address(NetworkDevice* device, uint8_t* mac_address) {
    if (!device) return;
    
    memcpy(mac_address, device->mac_address, 6);
}

// Set device MAC address
void network_set_mac_address(NetworkDevice* device, const uint8_t* mac_address) {
    if (!device) return;
    
    memcpy(device->mac_address, mac_address, 6);
}

// Enable/disable network device
void network_set_device_state(NetworkDevice* device, bool enabled) {
    if (!device) return;
    
    device->is_up = enabled;
    
    // TODO: Update hardware state
    // This would typically involve:
    // 1. Enabling/disabling the network card
    // 2. Configuring interrupts
    // 3. Setting up DMA
}

// Check if device is up
bool network_is_device_up(NetworkDevice* device) {
    if (!device) return false;
    
    return device->is_up;
}

// Handle network interrupt
void network_handle_interrupt(NetworkDevice* device) {
    if (!device) return;
    
    // TODO: Handle hardware interrupt
    // This would typically involve:
    // 1. Reading interrupt status
    // 2. Handling received packets
    // 3. Handling transmission completion
    // 4. Handling errors
} 