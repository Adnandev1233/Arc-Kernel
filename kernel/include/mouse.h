#ifndef MOUSE_H
#define MOUSE_H

#include <stdint.h>
#include <stdbool.h>

// Mouse ports
#define MOUSE_DATA_PORT 0x60
#define MOUSE_STATUS_PORT 0x64
#define MOUSE_COMMAND_PORT 0x64

// Mouse commands
#define MOUSE_CMD_ENABLE 0xA8
#define MOUSE_CMD_DISABLE 0xA7
#define MOUSE_CMD_USE_DEFAULT 0xF6
#define MOUSE_CMD_USE_STREAM 0xEA
#define MOUSE_CMD_READ_DATA 0xEB
#define MOUSE_CMD_SET_SAMPLE_RATE 0xF3
#define MOUSE_CMD_SET_RESOLUTION 0xE8

// Mouse state structure
typedef struct {
    int x;
    int y;
    bool left_button;
    bool right_button;
    bool middle_button;
} mouse_state_t;

// Function declarations
void mouse_init(void);
void mouse_handle_packet(uint8_t packet[3]);
void mouse_get_position(int* x, int* y);
bool mouse_get_button_state(uint8_t button);
void mouse_set_sample_rate(uint8_t rate);
void mouse_set_resolution(uint8_t resolution);
void get_mouse_state(mouse_state_t* state);
void mouse_set_position(int x, int y);

#endif // MOUSE_H 