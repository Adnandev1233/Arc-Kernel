#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdint.h>

// Terminal colors
enum vga_color {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15,
};

// Terminal functions
void init_terminal(void);
void terminal_putchar(char c);
void terminal_writestring(const char* data);
void terminal_setcolor(uint8_t color);
void terminal_clear(void);
void terminal_move_cursor(int x, int y);
void terminal_move_cursor_left(void);
void terminal_move_cursor_right(void);
void terminal_delete_char(void);

#endif /* TERMINAL_H */ 