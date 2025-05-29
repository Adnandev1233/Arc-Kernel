#include "include/mouse.h"
#include "include/io.h"

#define MOUSE_DATA_PORT 0x60
#define MOUSE_STATUS_PORT 0x64
#define MOUSE_COMMAND_PORT 0x64

// Mouse state
static mouse_state_t mouse_state = {0, 0, false, false, false};

// Initialize mouse
void mouse_init(void) {
    uint8_t status;
    
    // Enable auxiliary device
    port_out_byte(MOUSE_COMMAND_PORT, 0xA8);
    
    // Enable interrupts
    port_out_byte(MOUSE_COMMAND_PORT, 0x20);
    status = port_in_byte(MOUSE_DATA_PORT);
    status |= 2;
    port_out_byte(MOUSE_COMMAND_PORT, 0x60);
    port_out_byte(MOUSE_DATA_PORT, status);
    
    // Use default settings
    port_out_byte(MOUSE_COMMAND_PORT, 0xD4);
    port_out_byte(MOUSE_DATA_PORT, 0xF6);
    
    // Enable the mouse
    port_out_byte(MOUSE_COMMAND_PORT, 0xD4);
    port_out_byte(MOUSE_DATA_PORT, 0xF4);
}

// Get mouse state
void get_mouse_state(mouse_state_t* state) {
    *state = mouse_state;
}

// Set mouse position
void mouse_set_position(int x, int y) {
    mouse_state.x = x;
    mouse_state.y = y;
}

// Mouse interrupt handler
void mouse_handle_interrupt(void) {
    static uint8_t cycle = 0;
    static uint8_t bytes[3];
    
    switch (cycle) {
        case 0:
            bytes[0] = port_in_byte(MOUSE_DATA_PORT);
            if (bytes[0] & 0x08) {
                cycle = 1;
            }
            break;
            
        case 1:
            bytes[1] = port_in_byte(MOUSE_DATA_PORT);
            cycle = 2;
            break;
            
        case 2:
            bytes[2] = port_in_byte(MOUSE_DATA_PORT);
            
            // Update mouse state
            mouse_state.left_button = bytes[0] & 0x01;
            mouse_state.right_button = bytes[0] & 0x02;
            mouse_state.middle_button = bytes[0] & 0x04;
            
            // Update position
            if (bytes[0] & 0x10) {
                mouse_state.x -= (bytes[1] | 0xFFFFFF00);
            } else {
                mouse_state.x += bytes[1];
            }
            
            if (bytes[0] & 0x20) {
                mouse_state.y += (bytes[2] | 0xFFFFFF00);
            } else {
                mouse_state.y -= bytes[2];
            }
            
            cycle = 0;
            break;
    }
} 