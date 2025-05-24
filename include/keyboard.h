#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdbool.h>
#include <stdint.h>

// Keyboard buffer size
#define KEYBOARD_BUFFER_SIZE 256

// Keyboard scan codes
#define KEY_ESCAPE 0x01
#define KEY_BACKSPACE 0x0E
#define KEY_TAB 0x0F
#define KEY_ENTER 0x1C
#define KEY_LEFT_SHIFT 0x2A
#define KEY_RIGHT_SHIFT 0x36
#define KEY_LEFT_CTRL 0x1D
#define KEY_LEFT_ALT 0x38
#define KEY_CAPS_LOCK 0x3A
#define KEY_F1 0x3B
#define KEY_F2 0x3C
#define KEY_F3 0x3D
#define KEY_F4 0x3E
#define KEY_F5 0x3F
#define KEY_F6 0x40
#define KEY_F7 0x41
#define KEY_F8 0x42
#define KEY_F9 0x43
#define KEY_F10 0x44
#define KEY_F11 0x57
#define KEY_F12 0x58
#define KEY_NUM_LOCK 0x45
#define KEY_SCROLL_LOCK 0x46
#define KEY_HOME 0x47
#define KEY_UP 0x48
#define KEY_PAGE_UP 0x49
#define KEY_LEFT 0x4B
#define KEY_RIGHT 0x4D
#define KEY_END 0x4F
#define KEY_DOWN 0x50
#define KEY_PAGE_DOWN 0x51
#define KEY_INSERT 0x52
#define KEY_DELETE 0x53

// Function declarations
void keyboard_init(void);
char keyboard_getchar(void);
uint8_t keyboard_get_scancode(void);
bool keyboard_is_special_key(uint8_t scancode);
char get_ascii_from_scancode(uint8_t scancode);

#endif // KEYBOARD_H 