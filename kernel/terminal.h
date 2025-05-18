#ifndef TERMINAL_H
#define TERMINAL_H

#include <stddef.h>
#include <stdint.h>

// Terminal functions
void init_terminal(void);
void terminal_putchar(char c);
void terminal_write(const char* data, size_t size);
void terminal_writestring(const char* data);

// Cursor movement functions
void terminal_move_cursor_left(void);
void terminal_move_cursor_right(void);
void terminal_move_cursor_to(size_t row, size_t col);
void terminal_delete_char(void);
void terminal_clear_line(void);
void update_cursor(void);

// Terminal properties
void terminal_set_color(uint8_t color);
void terminal_get_cursor(size_t* row, size_t* col);

#endif /* TERMINAL_H */ 