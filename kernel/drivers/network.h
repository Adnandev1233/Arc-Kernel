#ifndef NETWORK_H
#define NETWORK_H

#include <stdint.h>
#include <stdbool.h>

// Ethernet frame structure
typedef struct {
    uint8_t dest_mac[6];
    uint8_t src_mac[6];
    uint16_t ethertype;
    uint8_t payload[1500];  // Maximum payload size
    uint16_t payload_length;
} EthernetFrame;

// Network packet structure
typedef struct {
    uint8_t data[1518];  // Maximum Ethernet frame size
    uint16_t length;     // Actual packet length
} NetworkPacket;

// Network device structure
typedef struct {
    char name[32];
    uint8_t mac_address[6];
    bool is_initialized;
    bool is_up;
    bool is_running;
    void (*send_packet)(NetworkPacket* packet);
    void (*receive_packet)(NetworkPacket* packet);
} NetworkDevice;

// Initialize network subsystem
bool network_init(void);

// Shutdown network subsystem
void network_shutdown(void);

// Send packet through network
bool network_send_packet(NetworkDevice* device, NetworkPacket* packet);

// Receive packet from network
bool network_receive_packet(NetworkDevice* device, NetworkPacket* packet);

// Handle network interrupt
void network_handle_interrupt(NetworkDevice* device);

// Get MAC address
void network_get_mac_address(NetworkDevice* device, uint8_t* mac_address);

// Set MAC address
void network_set_mac_address(NetworkDevice* device, const uint8_t* mac_address);

// Enable/disable network device
void network_set_device_state(NetworkDevice* device, bool enabled);

// Check if network device is up
bool network_is_device_up(NetworkDevice* device);

// Network driver functions that must be implemented by specific drivers
typedef struct {
    // Initialize the network hardware
    bool (*init)(NetworkDevice* device);
    
    // Shutdown the network hardware
    void (*shutdown)(NetworkDevice* device);
    
    // Send a packet through the hardware
    bool (*send)(NetworkDevice* device, NetworkPacket* packet);
    
    // Receive a packet from the hardware
    bool (*receive)(NetworkDevice* device, NetworkPacket* packet);
    
    // Handle hardware interrupts
    void (*handle_interrupt)(NetworkDevice* device);
} NetworkDriver;

#endif // NETWORK_H 