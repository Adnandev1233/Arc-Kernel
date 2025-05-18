#include <io.h>
#include "keyboard.h"
#include "terminal.h"

// Keyboard ports
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

// Keyboard states
#define SHIFT_PRESSED 0x2A
#define SHIFT_RELEASED 0xAA
#define CAPS_PRESSED 0x3A
#define CTRL_PRESSED 0x1D
#define CTRL_RELEASED 0x9D
#define ALT_PRESSED 0x38
#define ALT_RELEASED 0xB8

// Keyboard state
static bool shift_pressed = false;
static bool caps_lock = false;
static bool ctrl_pressed = false;
static bool alt_pressed = false;

// US keyboard layout scancode tables
static const char scancode_to_ascii[] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' '
};

static const char scancode_to_ascii_shift[] = {
    0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0, ' '
};

void keyboard_init(void) {
    // Reset keyboard state
    shift_pressed = false;
    caps_lock = false;
    ctrl_pressed = false;
    alt_pressed = false;
    
    // Reset keyboard controller
    while ((port_in_byte(KEYBOARD_STATUS_PORT) & 1) != 0) {
        port_in_byte(KEYBOARD_DATA_PORT);
    }
}

bool keyboard_is_special_key(uint8_t scancode) {
    switch (scancode) {
        case KEY_UP:
        case KEY_DOWN:
        case KEY_LEFT:
        case KEY_RIGHT:
        case KEY_HOME:
        case KEY_END:
        case KEY_DELETE:
        case KEY_ESC:
            return true;
        default:
            return false;
    }
}

uint8_t keyboard_get_scancode(void) {
    // Wait for keyboard data with timeout
    int timeout = 100000;  // Arbitrary timeout value
    while (timeout > 0 && (port_in_byte(KEYBOARD_STATUS_PORT) & 1) == 0) {
        timeout--;
    }
    
    if (timeout == 0) return 0;  // Timeout occurred
    return port_in_byte(KEYBOARD_DATA_PORT);
}

bool keyboard_process_scancode(uint8_t scancode, char* c) {
    // Handle key release
    if (scancode & KEY_RELEASED) {
        scancode &= ~KEY_RELEASED;  // Clear release bit
        switch (scancode) {
            case SHIFT_PRESSED:
                shift_pressed = false;
                break;
            case CTRL_PRESSED:
                ctrl_pressed = false;
                break;
            case ALT_PRESSED:
                alt_pressed = false;
                break;
        }
        return false;
    }
    
    // Handle key press
    switch (scancode) {
        case SHIFT_PRESSED:
            shift_pressed = true;
            return false;
            
        case CAPS_PRESSED:
            caps_lock = !caps_lock;
            return false;
            
        case CTRL_PRESSED:
            ctrl_pressed = true;
            return false;
            
        case ALT_PRESSED:
            alt_pressed = true;
            return false;
            
        default:
            if (scancode < sizeof(scancode_to_ascii)) {
                if (shift_pressed || caps_lock) {
                    *c = scancode_to_ascii_shift[scancode];
                } else {
                    *c = scancode_to_ascii[scancode];
                }
                return (*c != 0);
            }
            return false;
    }
}

char keyboard_getchar(void) {
    char c;
    uint8_t scancode;
    
    while (1) {
        scancode = keyboard_get_scancode();
        if (scancode == 0) continue;  // Timeout or invalid scancode
        
        if (keyboard_process_scancode(scancode, &c)) {
            return c;
        }
    }
}

void keyboard_update_command_line(char* buffer, size_t* pos, size_t max_size) {
    uint8_t scancode = keyboard_get_scancode();
    char c;
    
    // Handle special keys
    if (keyboard_is_special_key(scancode)) {
        switch (scancode) {
            case KEY_LEFT:
                if (*pos > 0) {
                    (*pos)--;
                    terminal_move_cursor_left();
                }
                break;
                
            case KEY_RIGHT:
                if (buffer[*pos] != '\0') {
                    (*pos)++;
                    terminal_move_cursor_right();
                }
                break;
                
            case KEY_HOME:
                while (*pos > 0) {
                    (*pos)--;
                    terminal_move_cursor_left();
                }
                break;
                
            case KEY_END:
                while (buffer[*pos] != '\0') {
                    (*pos)++;
                    terminal_move_cursor_right();
                }
                break;
                
            case KEY_DELETE:
                if (buffer[*pos] != '\0') {
                    // Shift characters left
                    for (size_t i = *pos; buffer[i] != '\0'; i++) {
                        buffer[i] = buffer[i + 1];
                    }
                    terminal_delete_char();
                }
                break;
        }
        return;
    }
    
    // Handle regular character input
    if (keyboard_process_scancode(scancode, &c)) {
        if (c == '\b') {  // Backspace
            if (*pos > 0) {
                (*pos)--;
                // Shift characters left
                for (size_t i = *pos; buffer[i] != '\0'; i++) {
                    buffer[i] = buffer[i + 1];
                }
                terminal_putchar('\b');
            }
        }
        else if (c == '\n') {  // Enter
            terminal_putchar('\n');
            buffer[*pos] = '\0';
        }
        else if (*pos < max_size - 1) {  // Regular character
            // Shift characters right
            for (size_t i = max_size - 1; i > *pos; i--) {
                buffer[i] = buffer[i - 1];
            }
            buffer[*pos] = c;
            (*pos)++;
            terminal_putchar(c);
        }
    }
} 