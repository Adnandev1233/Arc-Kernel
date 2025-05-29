#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

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

// Special key scancodes
#define KEY_ESC        0x01
#define KEY_BACKSPACE  0x0E
#define KEY_TAB        0x0F
#define KEY_ENTER      0x1C
#define KEY_LEFT_CTRL  0x1D
#define KEY_LEFT_SHIFT 0x2A
#define KEY_RIGHT_SHIFT 0x36
#define KEY_LEFT_ALT   0x38
#define KEY_CAPS_LOCK  0x3A
#define KEY_F1         0x3B
#define KEY_F2         0x3C
#define KEY_F3         0x3D
#define KEY_F4         0x3E
#define KEY_F5         0x3F
#define KEY_F6         0x40
#define KEY_F7         0x41
#define KEY_F8         0x42
#define KEY_F9         0x43
#define KEY_F10        0x44
#define KEY_NUM_LOCK   0x45
#define KEY_SCROLL_LOCK 0x46
#define KEY_HOME       0x47
#define KEY_UP         0x48
#define KEY_PAGE_UP    0x49
#define KEY_LEFT       0x4B
#define KEY_CENTER     0x4C
#define KEY_RIGHT      0x4D
#define KEY_END        0x4F
#define KEY_DOWN       0x50
#define KEY_PAGE_DOWN  0x51
#define KEY_INSERT     0x52
#define KEY_DELETE     0x53
#define KEY_F11        0x57
#define KEY_F12        0x58

// Key release bit
#define KEY_RELEASED   0x80

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
int keyboard_getline(char* buffer, int max_length);

#endif // KEYBOARD_H 