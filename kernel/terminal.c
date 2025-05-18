#include <terminal.h>
#include <io.h>

// VGA text mode constants
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000

// Terminal state
static size_t terminal_row = 0;
static size_t terminal_column = 0;
static uint8_t terminal_color = 0x0F; // White on black
static uint16_t* terminal_buffer = (uint16_t*) VGA_MEMORY;

// Initialize terminal
void init_terminal(void) {
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = 0x0F;  // White on black
    terminal_buffer = (uint16_t*)VGA_MEMORY;
    
    // Clear screen
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = (uint16_t)' ' | (uint16_t)terminal_color << 8;
        }
    }
}

// Write a single character
void terminal_putchar(char c) {
    if (c == '\n') {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT) {
            // Scroll screen
            for (size_t y = 0; y < VGA_HEIGHT - 1; y++) {
                for (size_t x = 0; x < VGA_WIDTH; x++) {
                    terminal_buffer[y * VGA_WIDTH + x] = terminal_buffer[(y + 1) * VGA_WIDTH + x];
                }
            }
            // Clear last line
            for (size_t x = 0; x < VGA_WIDTH; x++) {
                terminal_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = (uint16_t)' ' | (uint16_t)terminal_color << 8;
            }
            terminal_row = VGA_HEIGHT - 1;
        }
    }
    else if (c == '\b') {
        if (terminal_column > 0) {
            terminal_column--;
            const size_t index = terminal_row * VGA_WIDTH + terminal_column;
            terminal_buffer[index] = (uint16_t)' ' | (uint16_t)terminal_color << 8;
        }
    }
    else {
        const size_t index = terminal_row * VGA_WIDTH + terminal_column;
        terminal_buffer[index] = (uint16_t)c | (uint16_t)terminal_color << 8;
        if (++terminal_column == VGA_WIDTH) {
            terminal_column = 0;
            if (++terminal_row == VGA_HEIGHT) {
                // Scroll screen
                for (size_t y = 0; y < VGA_HEIGHT - 1; y++) {
                    for (size_t x = 0; x < VGA_WIDTH; x++) {
                        terminal_buffer[y * VGA_WIDTH + x] = terminal_buffer[(y + 1) * VGA_WIDTH + x];
                    }
                }
                // Clear last line
                for (size_t x = 0; x < VGA_WIDTH; x++) {
                    terminal_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = (uint16_t)' ' | (uint16_t)terminal_color << 8;
                }
                terminal_row = VGA_HEIGHT - 1;
            }
        }
    }
    update_cursor();
}

// Write a string of given size
void terminal_write(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        terminal_putchar(data[i]);
    }
}

// Write a null-terminated string
void terminal_writestring(const char* data) {
    for (size_t i = 0; data[i] != '\0'; i++) {
        terminal_putchar(data[i]);
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