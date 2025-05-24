#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

// Keyboard buffer size
#define KEYBOARD_BUFFER_SIZE 256

// Special key codes
#define KEY_UP      0x48
#define KEY_DOWN    0x50
#define KEY_LEFT    0x4B
#define KEY_RIGHT   0x4D
#define KEY_HOME    0x47
#define KEY_END     0x4F
#define KEY_ENTER   0x1C
#define KEY_BACKSPACE 0x0E
#define KEY_TAB     0x0F
#define KEY_DELETE  0x53
#define KEY_ESC     0x01

// Keyboard states
#define KEY_RELEASED 0x80

// Function declarations
void keyboard_init(void);
char keyboard_getchar(void);
bool keyboard_is_special_key(uint8_t scancode);
uint8_t keyboard_get_scancode(void);
bool keyboard_process_scancode(uint8_t scancode, char* c);
void keyboard_update_command_line(char* buffer, size_t* pos, size_t max_size);

// Command history functions
void keyboard_history_up(char* buffer, size_t* pos, size_t max_size);
void keyboard_history_down(char* buffer, size_t* pos, size_t max_size);

#endif /* KEYBOARD_H */ 