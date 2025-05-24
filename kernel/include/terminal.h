#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Terminal dimensions
#define TERMINAL_WIDTH 80
#define TERMINAL_HEIGHT 25

// VGA colors
#define VGA_COLOR_BLACK 0
#define VGA_COLOR_BLUE 1
#define VGA_COLOR_GREEN 2
#define VGA_COLOR_CYAN 3
#define VGA_COLOR_RED 4
#define VGA_COLOR_MAGENTA 5
#define VGA_COLOR_BROWN 6
#define VGA_COLOR_LIGHT_GREY 7
#define VGA_COLOR_DARK_GREY 8
#define VGA_COLOR_LIGHT_BLUE 9
#define VGA_COLOR_LIGHT_GREEN 10
#define VGA_COLOR_LIGHT_CYAN 11
#define VGA_COLOR_LIGHT_RED 12
#define VGA_COLOR_LIGHT_MAGENTA 13
#define VGA_COLOR_LIGHT_BROWN 14
#define VGA_COLOR_WHITE 15

// Terminal functions
void terminal_initialize(void);
void terminal_clear(void);
void terminal_set_color(uint8_t color);
void terminal_put_char(char c);
void terminal_write_string(const char* str);
void terminal_write_dec(uint32_t num);
void terminal_put_pixel(int x, int y, uint32_t color);
void terminal_draw_string(int x, int y, const char* str, uint32_t color);

#endif // TERMINAL_H 