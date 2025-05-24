#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

// Initialize window manager
void wm_init(void);

// Create a new window
// Returns window ID or -1 on error
int wm_create_window(const char* title, int x, int y, int width, int height);

// Draw a window
void wm_draw_window(int window_id);

// Set active window
void wm_set_active(int window_id);

// Handle mouse events
void wm_handle_mouse(int x, int y, bool left_button);

// Main window manager loop
void wm_main_loop(void);

#endif // WINDOW_MANAGER_H 