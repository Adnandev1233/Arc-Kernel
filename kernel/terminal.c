#include "include/terminal.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// VGA text mode constants
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000

// Function prototypes
void update_cursor(void);

// Terminal state
static size_t terminal_row = 0;
static size_t terminal_column = 0;
static uint8_t terminal_color = 0x0F; // White on black
static uint16_t* terminal_buffer = (uint16_t*) VGA_MEMORY;

// Initialize terminal
void terminal_initialize(void) {
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = VGA_COLOR_LIGHT_GREY;
    terminal_buffer = VGA_MEMORY;
    terminal_clear();
}

// Clear the entire screen
void terminal_clear(void) {
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = (uint16_t)' ' | (uint16_t)terminal_color << 8;
        }
    }
    terminal_row = 0;
    terminal_column = 0;
    update_cursor();
}

// Move cursor to specific position
void terminal_move_cursor(int x, int y) {
    if (x >= 0 && x < VGA_WIDTH && y >= 0 && y < VGA_HEIGHT) {
        terminal_column = x;
        terminal_row = y;
        update_cursor();
    }
}

// Set terminal color
void terminal_set_color(uint8_t color) {
    terminal_color = color;
}

// Put character at current position
void terminal_put_char(char c) {
    if (c == '\n') {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT) {
            terminal_row = 0;
        }
        return;
    }

    const size_t index = terminal_row * VGA_WIDTH + terminal_column;
    terminal_buffer[index] = (uint16_t)c | (uint16_t)terminal_color << 8;

    if (++terminal_column == VGA_WIDTH) {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT) {
            terminal_row = 0;
        }
    }
    update_cursor();
}

// Write string to terminal
void terminal_write_string(const char* str) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        terminal_put_char(str[i]);
    }
}

// Write decimal number to terminal
void terminal_write_dec(uint32_t num) {
    if (num == 0) {
        terminal_put_char('0');
        return;
    }

    char buffer[32];
    int i = 0;
    while (num > 0) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }

    while (--i >= 0) {
        terminal_put_char(buffer[i]);
    }
}

// Put pixel in graphics mode
void terminal_put_pixel(int x, int y, uint32_t color) {
    // For now, we'll just write to VGA memory directly
    // In a real implementation, this would use a proper graphics mode
    if (x >= 0 && x < VGA_WIDTH && y >= 0 && y < VGA_HEIGHT) {
        const size_t index = y * VGA_WIDTH + x;
        terminal_buffer[index] = (uint16_t)' ' | (uint16_t)(color & 0x0F) << 8;
    }
}

// Draw string in graphics mode
void terminal_draw_string(int x, int y, const char* str, uint32_t color) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        if (x + i >= VGA_WIDTH) break;
        const size_t index = y * VGA_WIDTH + (x + i);
        terminal_buffer[index] = (uint16_t)str[i] | (uint16_t)(color & 0x0F) << 8;
    }
}

// Update hardware cursor position
void update_cursor(void) {
    uint16_t pos = terminal_row * VGA_WIDTH + terminal_column;
    
    port_out_byte(0x3D4, 0x0F);
    port_out_byte(0x3D5, (uint8_t)(pos & 0xFF));
    port_out_byte(0x3D4, 0x0E);
    port_out_byte(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void terminal_move_cursor_left(void) {
    if (terminal_column > 0) {
        terminal_column--;
        update_cursor();
    }
    else if (terminal_row > 0) {
        terminal_row--;
        terminal_column = VGA_WIDTH - 1;
        update_cursor();
    }
}

void terminal_move_cursor_right(void) {
    if (terminal_column < VGA_WIDTH - 1) {
        terminal_column++;
        update_cursor();
    }
    else if (terminal_row < VGA_HEIGHT - 1) {
        terminal_row++;
        terminal_column = 0;
        update_cursor();
    }
}

void terminal_move_cursor_to(size_t row, size_t col) {
    if (row < VGA_HEIGHT && col < VGA_WIDTH) {
        terminal_row = row;
        terminal_column = col;
        update_cursor();
    }
}

void terminal_delete_char(void) {
    if (terminal_column < VGA_WIDTH && terminal_row < VGA_HEIGHT) {
        // Move all characters after cursor one position left
        for (size_t x = terminal_column; x < VGA_WIDTH - 1; x++) {
            const size_t index = terminal_row * VGA_WIDTH + x;
            terminal_buffer[index] = terminal_buffer[index + 1];
        }
        
        // Clear last character in line
        const size_t last = terminal_row * VGA_WIDTH + (VGA_WIDTH - 1);
        terminal_buffer[last] = (uint16_t)' ' | (uint16_t)terminal_color << 8;
    }
}

void terminal_clear_line(void) {
    if (terminal_row < VGA_HEIGHT) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = terminal_row * VGA_WIDTH + x;
            terminal_buffer[index] = (uint16_t)' ' | (uint16_t)terminal_color << 8;
        }
        terminal_column = 0;
        update_cursor();
    }
} 