#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include <stdbool.h>

// Keyboard buffer size
#define KEYBOARD_BUFFER_SIZE 256

// Keyboard ports
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

// Keyboard commands
#define KEYBOARD_CMD_LED 0xED
#define KEYBOARD_CMD_SCANCODE 0xF0
#define KEYBOARD_CMD_TYPEMATIC 0xF3

// Keyboard LED bits
#define KEYBOARD_LED_SCROLL_LOCK 0x01
#define KEYBOARD_LED_NUM_LOCK    0x02
#define KEYBOARD_LED_CAPS_LOCK   0x04

// Function declarations
void keyboard_init(void);
char keyboard_getchar(void);
void keyboard_set_leds(uint8_t leds);
void keyboard_set_typematic(uint8_t delay, uint8_t repeat_rate);
bool keyboard_is_shift_pressed(void);
bool keyboard_is_ctrl_pressed(void);
bool keyboard_is_alt_pressed(void);
bool keyboard_is_caps_lock(void);
bool keyboard_is_num_lock(void);
bool keyboard_is_scroll_lock(void);

#endif // KEYBOARD_H 