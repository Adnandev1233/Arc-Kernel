#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>
#include <stdbool.h>

// Color definitions (32-bit ARGB)
#define COLOR_BLACK     0xFF000000
#define COLOR_WHITE     0xFFFFFFFF
#define COLOR_RED       0xFFFF0000
#define COLOR_GREEN     0xFF00FF00
#define COLOR_BLUE      0xFF0000FF
#define COLOR_YELLOW    0xFFFFFF00
#define COLOR_CYAN      0xFF00FFFF
#define COLOR_MAGENTA   0xFFFF00FF

// VBE mode info structure
struct vbe_mode_info {
    uint16_t attributes;
    uint8_t window_a;
    uint8_t window_b;
    uint16_t granularity;
    uint16_t window_size;
    uint16_t segment_a;
    uint16_t segment_b;
    uint32_t win_func_ptr;
    uint16_t pitch;
    uint16_t width;
    uint16_t height;
    uint8_t w_char;
    uint8_t y_char;
    uint8_t planes;
    uint8_t bpp;
    uint8_t banks;
    uint8_t memory_model;
    uint8_t bank_size;
    uint8_t image_pages;
    uint8_t reserved0;
    uint8_t red_mask;
    uint8_t red_position;
    uint8_t green_mask;
    uint8_t green_position;
    uint8_t blue_mask;
    uint8_t blue_position;
    uint8_t reserved_mask;
    uint8_t reserved_position;
    uint8_t direct_color_attributes;
    uint32_t framebuffer;
    uint32_t off_screen_mem_off;
    uint16_t off_screen_mem_size;
    uint8_t reserved1[206];
} __attribute__((packed));

// Graphics driver functions
bool graphics_init(void);
void graphics_set_mode(uint16_t width, uint16_t height, uint8_t bpp);
void graphics_put_pixel(uint16_t x, uint16_t y, uint32_t color);
void graphics_draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color);
void graphics_draw_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
void graphics_fill_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
void graphics_draw_circle(uint16_t x, uint16_t y, uint16_t radius, uint32_t color);
void graphics_fill_circle(uint16_t x, uint16_t y, uint16_t radius, uint32_t color);
void graphics_clear(uint32_t color);
void graphics_swap_buffers(void);

// Font rendering
void graphics_draw_char(uint16_t x, uint16_t y, char c, uint32_t color);
void graphics_draw_string(uint16_t x, uint16_t y, const char* str, uint32_t color);

// Screen information
uint16_t graphics_get_width(void);
uint16_t graphics_get_height(void);
uint8_t graphics_get_bpp(void);
uint32_t* graphics_get_framebuffer(void);

#endif // GRAPHICS_H 