#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "../include/desktop.h"
#include "../include/terminal.h"
#include "../include/mouse.h"
#include "../include/mouse_state.h"
#include "../include/keyboard.h"
#include "window_manager.h"
#include "dock.h"
#include "menu_bar.h"

// Global variables
static window_t windows[MAX_WINDOWS];
static int active_window = -1;
static bool is_running = true;

// Initialize desktop environment
void desktop_init(void) {
    // Initialize components
    wm_init();
    dock_init();
    menu_bar_init();
    
    // Add some default dock items
    dock_add_item("Terminal");
    dock_add_item("File Manager");
    dock_add_item("Settings");
    
    // Add some menu items
    int file_menu = 0; // File menu is first
    menu_bar_add_item(file_menu, "New Window", NULL);
    menu_bar_add_item(file_menu, "Open...", NULL);
    menu_bar_add_item(file_menu, "Save", NULL);
    menu_bar_add_item(file_menu, "Exit", NULL);
    
    int edit_menu = 1; // Edit menu is second
    menu_bar_add_item(edit_menu, "Cut", NULL);
    menu_bar_add_item(edit_menu, "Copy", NULL);
    menu_bar_add_item(edit_menu, "Paste", NULL);
    
    // Initialize windows array
    for (int i = 0; i < MAX_WINDOWS; i++) {
        windows[i].is_visible = false;
        windows[i].is_active = false;
    }
    
    // Draw initial desktop
    desktop_draw();
}

// Draw desktop
void desktop_draw(void) {
    // Draw background
    desktop_draw_background();
    
    // Draw taskbar
    desktop_draw_taskbar();
    
    // Draw windows
    for (int i = 0; i < MAX_WINDOWS; i++) {
        if (windows[i].is_visible) {
            // Draw window border
            for (int x = windows[i].x; x < windows[i].x + windows[i].width; x++) {
                for (int y = windows[i].y; y < windows[i].y + windows[i].height; y++) {
                    if (x == windows[i].x || x == windows[i].x + windows[i].width - 1 ||
                        y == windows[i].y || y == windows[i].y + windows[i].height - 1) {
                        terminal_put_pixel(x, y, DESKTOP_COLOR_WINDOW_BORDER);
                    } else {
                        terminal_put_pixel(x, y, DESKTOP_COLOR_WINDOW_BG);
                    }
                }
            }
            
            // Draw window title
            terminal_draw_string(windows[i].x + 5, windows[i].y + 2, windows[i].title, 
                               windows[i].is_active ? DESKTOP_COLOR_WINDOW_BORDER : DESKTOP_COLOR_WINDOW_BG);
        }
    }
}

// Draw desktop background
void desktop_draw_background(void) {
    // Fill screen with background color
    for (int x = 0; x < DESKTOP_WIDTH; x++) {
        for (int y = 0; y < DESKTOP_HEIGHT; y++) {
            terminal_put_pixel(x, y, DESKTOP_COLOR_BACKGROUND);
        }
    }
}

// Draw taskbar
void desktop_draw_taskbar(void) {
    // Draw taskbar background
    for (int x = 0; x < DESKTOP_WIDTH; x++) {
        for (int y = DESKTOP_HEIGHT - TASKBAR_HEIGHT; y < DESKTOP_HEIGHT; y++) {
            terminal_put_pixel(x, y, DESKTOP_COLOR_TASKBAR);
        }
    }
    
    // Draw start button
    terminal_draw_string(5, DESKTOP_HEIGHT - TASKBAR_HEIGHT + 5, "Start", DESKTOP_COLOR_TASKBAR_TEXT);
    
    // Draw window buttons
    int button_x = 100;
    for (int i = 0; i < MAX_WINDOWS; i++) {
        if (windows[i].is_visible) {
            terminal_draw_string(button_x, DESKTOP_HEIGHT - TASKBAR_HEIGHT + 5, 
                               windows[i].title, 
                               windows[i].is_active ? DESKTOP_COLOR_TASKBAR_TEXT : DESKTOP_COLOR_TASKBAR_TEXT_DIM);
            button_x += 150; // Space between window buttons
        }
    }
}

// Handle mouse input
void desktop_handle_mouse(int x, int y, bool left_button) {
    (void)left_button; // Silence unused parameter warning
    
    // Check if click is in menu bar
    if (y < MENU_BAR_HEIGHT) {
        // Handle menu bar clicks
        menu_bar_handle_click(x, y);
        return;
    }
    
    // Check if click is in any window
    for (int i = 0; i < MAX_WINDOWS; i++) {
        if (windows[i].is_visible &&
            x >= windows[i].x && x < windows[i].x + windows[i].width &&
            y >= windows[i].y && y < windows[i].y + windows[i].height) {
            
            // Activate window
            desktop_focus_window(i);
            return;
        }
    }
}

// Main desktop loop
void desktop_main(void) {
    int x, y;
    bool left_button;
    mouse_state_t state;
    get_mouse_state(&state);
    x = state.x;
    y = state.y;
    left_button = state.left_button;
    
    while (is_running) {
        // Handle mouse input
        if (left_button) {
            desktop_handle_mouse(x, y, left_button);
        }
        
        // Handle keyboard input
        char c = keyboard_getchar();
        if (c) {
            // TODO: Handle keyboard input
        }
        
        // Redraw desktop
        desktop_draw();
    }
}

// Create a new window
void desktop_create_window(const char* title, int x, int y, int width, int height) {
    // Find free window slot
    for (int i = 0; i < MAX_WINDOWS; i++) {
        if (!windows[i].is_visible) {
            windows[i].x = x;
            windows[i].y = y;
            windows[i].width = width;
            windows[i].height = height;
            strncpy(windows[i].title, title, sizeof(windows[i].title) - 1);
            windows[i].is_visible = true;
            windows[i].is_active = true;
            active_window = i;
            return;
        }
    }
}

// Close a window
void desktop_close_window(int window_id) {
    if (window_id >= 0 && window_id < MAX_WINDOWS) {
        windows[window_id].is_visible = false;
        windows[window_id].is_active = false;
        if (active_window == window_id) {
            active_window = -1;
        }
    }
}

// Focus a window
void desktop_focus_window(int window_id) {
    if (window_id >= 0 && window_id < MAX_WINDOWS && windows[window_id].is_visible) {
        // Deactivate current active window
        if (active_window >= 0) {
            windows[active_window].is_active = false;
        }
        
        // Activate new window
        windows[window_id].is_active = true;
        active_window = window_id;
    }
}

// Move a window
void desktop_move_window(int window_id, int x, int y) {
    if (window_id >= 0 && window_id < MAX_WINDOWS && windows[window_id].is_visible) {
        windows[window_id].x = x;
        windows[window_id].y = y;
    }
}

// Resize a window
void desktop_resize_window(int window_id, int width, int height) {
    if (window_id >= 0 && window_id < MAX_WINDOWS && windows[window_id].is_visible) {
        windows[window_id].width = width;
        windows[window_id].height = height;
    }
} 