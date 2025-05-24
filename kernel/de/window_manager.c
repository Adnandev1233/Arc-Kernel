#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "../include/terminal.h"
#include "../include/keyboard.h"
#include "../include/mouse.h"
#include "../include/mouse_state.h"
#include "../include/memory.h"
#include "window_manager.h"

// Window structure
typedef struct {
    int x, y;
    int width, height;
    char title[32];
    bool is_active;
    bool is_minimized;
    uint32_t* buffer;
} Window;

// Window list
#define MAX_WINDOWS 10
static Window windows[MAX_WINDOWS];
static int active_window = -1;
static int window_count = 0;

// Color definitions
#define COLOR_BACKGROUND 0x2C2C2C
#define COLOR_WINDOW_BG 0xFFFFFF
#define COLOR_TITLE_BAR 0x3C3C3C
#define COLOR_TITLE_TEXT 0xFFFFFF
#define COLOR_BORDER 0x808080

// Initialize window manager
void wm_init(void) {
    // Initialize window list
    memset(windows, 0, sizeof(windows));
}

// Create a new window
int wm_create_window(const char* title, int x, int y, int width, int height) {
    if (window_count >= MAX_WINDOWS) return -1;
    
    Window* win = &windows[window_count];
    win->x = x;
    win->y = y;
    win->width = width;
    win->height = height;
    strncpy(win->title, title, sizeof(win->title) - 1);
    win->is_active = false;
    win->is_minimized = false;
    
    // Allocate window buffer
    win->buffer = (uint32_t*)kmalloc(width * height * sizeof(uint32_t));
    if (!win->buffer) return -1;
    
    // Clear window buffer
    memset(win->buffer, COLOR_WINDOW_BG, width * height * sizeof(uint32_t));
    
    // Draw window
    wm_draw_window(window_count);
    
    // Make it active
    wm_set_active(window_count);
    
    return window_count++;
}

// Draw a window
void wm_draw_window(int window_id) {
    if (window_id < 0 || window_id >= window_count) return;
    
    Window* win = &windows[window_id];
    
    // Draw title bar
    for (int x = win->x; x < win->x + win->width; x++) {
        for (int y = win->y; y < win->y + 20; y++) {
            terminal_put_pixel(x, y, COLOR_TITLE_BAR);
        }
    }
    
    // Draw title text
    terminal_draw_string(win->x + 5, win->y + 3, win->title, COLOR_TITLE_TEXT);
    
    // Draw window content
    for (int x = 0; x < win->width; x++) {
        for (int y = 0; y < win->height; y++) {
            terminal_put_pixel(win->x + x, win->y + 20 + y, win->buffer[y * win->width + x]);
        }
    }
    
    // Draw border
    for (int x = win->x; x < win->x + win->width; x++) {
        terminal_put_pixel(x, win->y, COLOR_BORDER);
        terminal_put_pixel(x, win->y + win->height + 20, COLOR_BORDER);
    }
    for (int y = win->y; y < win->y + win->height + 20; y++) {
        terminal_put_pixel(win->x, y, COLOR_BORDER);
        terminal_put_pixel(win->x + win->width, y, COLOR_BORDER);
    }
}

// Set active window
void wm_set_active(int window_id) {
    if (window_id < 0 || window_id >= window_count) return;
    
    // Deactivate current window
    if (active_window >= 0) {
        windows[active_window].is_active = false;
        wm_draw_window(active_window);
    }
    
    // Activate new window
    active_window = window_id;
    windows[window_id].is_active = true;
    wm_draw_window(window_id);
}

// Handle mouse events
void wm_handle_mouse(int x, int y, bool left_button) {
    if (left_button) {
        // Check if click is in title bar of any window
        for (int i = 0; i < window_count; i++) {
            Window* win = &windows[i];
            if (x >= win->x && x < win->x + win->width &&
                y >= win->y && y < win->y + 20) {
                wm_set_active(i);
                break;
            }
        }
    }
}

// Main window manager loop
void wm_main_loop(void) {
    while (1) {
        // Handle mouse input
        int x, y;
        bool left_button;
        get_mouse_state(&x, &y, &left_button);
        wm_handle_mouse(x, y, left_button);
        
        // Handle keyboard input
        char c = keyboard_getchar();
        if (c) {
            // Handle keyboard input for active window
            if (active_window >= 0) {
                // TODO: Send keyboard input to window
            }
        }
    }
} 