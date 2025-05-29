#include "include/graphics.h"
#include "include/io.h"
#include <string.h>
#include <stdlib.h>  // For abs()

// VBE constants
#define VBE_DISPI_IOPORT_INDEX 0x01CE
#define VBE_DISPI_IOPORT_DATA  0x01CF

#define VBE_DISPI_INDEX_ID              0x0
#define VBE_DISPI_INDEX_XRES           0x1
#define VBE_DISPI_INDEX_YRES           0x2
#define VBE_DISPI_INDEX_BPP            0x3
#define VBE_DISPI_INDEX_ENABLE         0x4
#define VBE_DISPI_INDEX_BANK           0x5
#define VBE_DISPI_INDEX_VIRT_WIDTH     0x6
#define VBE_DISPI_INDEX_VIRT_HEIGHT    0x7
#define VBE_DISPI_INDEX_X_OFFSET       0x8
#define VBE_DISPI_INDEX_Y_OFFSET       0x9

#define VBE_DISPI_DISABLED             0x00
#define VBE_DISPI_ENABLED              0x01
#define VBE_DISPI_GETCAPS              0x02
#define VBE_DISPI_8BIT_DAC             0x20
#define VBE_DISPI_LFB_ENABLED          0x40
#define VBE_DISPI_NOCLEARMEM           0x80

// Global variables
static uint32_t* framebuffer = NULL;
static uint16_t screen_width = 0;
static uint16_t screen_height = 0;
static uint8_t screen_bpp = 0;
static uint16_t screen_pitch = 0;

// Internal functions
static int abs(int x) {
    return (x < 0) ? -x : x;
}

static void vbe_write(uint16_t index, uint16_t value) {
    port_out_word(VBE_DISPI_IOPORT_INDEX, index);
    port_out_word(VBE_DISPI_IOPORT_DATA, value);
}

static uint16_t vbe_read(uint16_t index) {
    port_out_word(VBE_DISPI_IOPORT_INDEX, index);
    return port_in_word(VBE_DISPI_IOPORT_DATA);
}

// Initialize graphics driver
bool graphics_init(void) {
    // Check if VBE is available
    uint16_t vbe_version = vbe_read(VBE_DISPI_INDEX_ID);
    if (vbe_version < 0xB0C0) {
        return false;
    }

    // Set default mode (800x600x32)
    graphics_set_mode(800, 600, 32);
    return true;
}

// Set video mode
void graphics_set_mode(uint16_t width, uint16_t height, uint8_t bpp) {
    // Disable VBE
    vbe_write(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);

    // Set resolution
    vbe_write(VBE_DISPI_INDEX_XRES, width);
    vbe_write(VBE_DISPI_INDEX_YRES, height);
    vbe_write(VBE_DISPI_INDEX_BPP, bpp);

    // Enable VBE with LFB
    vbe_write(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED);

    // Update global variables
    screen_width = width;
    screen_height = height;
    screen_bpp = bpp;
    screen_pitch = width * (bpp / 8);
    framebuffer = (uint32_t*)0xFD000000; // LFB address
}

// Draw a pixel
void graphics_put_pixel(uint16_t x, uint16_t y, uint32_t color) {
    if (x >= screen_width || y >= screen_height) return;
    framebuffer[y * screen_width + x] = color;
}

// Draw a line using Bresenham's algorithm
void graphics_draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        graphics_put_pixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

// Draw a rectangle
void graphics_draw_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
    graphics_draw_line(x, y, x + width - 1, y, color);
    graphics_draw_line(x, y + height - 1, x + width - 1, y + height - 1, color);
    graphics_draw_line(x, y, x, y + height - 1, color);
    graphics_draw_line(x + width - 1, y, x + width - 1, y + height - 1, color);
}

// Fill a rectangle
void graphics_fill_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
    for (uint16_t i = y; i < y + height; i++) {
        for (uint16_t j = x; j < x + width; j++) {
            graphics_put_pixel(j, i, color);
        }
    }
}

// Draw a circle using Bresenham's algorithm
void graphics_draw_circle(uint16_t x, uint16_t y, uint16_t radius, uint32_t color) {
    int f = 1 - radius;
    int ddF_x = 1;
    int ddF_y = -2 * radius;
    int x1 = 0;
    int y1 = radius;

    graphics_put_pixel(x, y + radius, color);
    graphics_put_pixel(x, y - radius, color);
    graphics_put_pixel(x + radius, y, color);
    graphics_put_pixel(x - radius, y, color);

    while (x1 < y1) {
        if (f >= 0) {
            y1--;
            ddF_y += 2;
            f += ddF_y;
        }
        x1++;
        ddF_x += 2;
        f += ddF_x;

        graphics_put_pixel(x + x1, y + y1, color);
        graphics_put_pixel(x - x1, y + y1, color);
        graphics_put_pixel(x + x1, y - y1, color);
        graphics_put_pixel(x - x1, y - y1, color);
        graphics_put_pixel(x + y1, y + x1, color);
        graphics_put_pixel(x - y1, y + x1, color);
        graphics_put_pixel(x + y1, y - x1, color);
        graphics_put_pixel(x - y1, y - x1, color);
    }
}

// Fill a circle
void graphics_fill_circle(uint16_t x, uint16_t y, uint16_t radius, uint32_t color) {
    for (int i = -radius; i <= radius; i++) {
        for (int j = -radius; j <= radius; j++) {
            if (i*i + j*j <= radius*radius) {
                graphics_put_pixel(x + i, y + j, color);
            }
        }
    }
}

// Clear screen
void graphics_clear(uint32_t color) {
    for (uint32_t i = 0; i < screen_width * screen_height; i++) {
        framebuffer[i] = color;
    }
}

// Double buffering (if supported)
void graphics_swap_buffers(void) {
    // For now, we're using a single buffer
    // TODO: Implement double buffering
}

// Draw a character (8x16 font)
void graphics_draw_char(uint16_t x, uint16_t y, char c, uint32_t color) {
    (void)c;  // Silence unused parameter warning
    // TODO: Implement font rendering
    // For now, just draw a placeholder
    graphics_fill_rect(x, y, 8, 16, color);
}

// Draw a string
void graphics_draw_string(uint16_t x, uint16_t y, const char* str, uint32_t color) {
    while (*str) {
        graphics_draw_char(x, y, *str++, color);
        x += 8;
    }
}

// Get screen width
uint16_t graphics_get_width(void) {
    return screen_width;
}

// Get screen height
uint16_t graphics_get_height(void) {
    return screen_height;
}

// Get bits per pixel
uint8_t graphics_get_bpp(void) {
    return screen_bpp;
}

// Get framebuffer address
uint32_t* graphics_get_framebuffer(void) {
    return framebuffer;
} 