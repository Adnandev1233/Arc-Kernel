#ifndef DESKTOP_H
#define DESKTOP_H

#include <stdint.h>
#include <stdbool.h>

// Desktop dimensions
#define DESKTOP_WIDTH 1024
#define DESKTOP_HEIGHT 768

// Desktop colors
#define DESKTOP_COLOR_BACKGROUND 0x000033
#define DESKTOP_COLOR_TASKBAR 0x000066
#define DESKTOP_COLOR_WINDOW_BG 0xFFFFFF
#define DESKTOP_COLOR_WINDOW_BORDER 0x000000
#define DESKTOP_COLOR_TASKBAR_TEXT 0xFFFFFF
#define DESKTOP_COLOR_TASKBAR_TEXT_DIM 0xAAAAAA

// Window constants
#define MAX_WINDOWS 10
#define WINDOW_TITLE_HEIGHT 20
#define WINDOW_BORDER_WIDTH 2
#define WINDOW_MIN_WIDTH 200
#define WINDOW_MIN_HEIGHT 150

// Menu bar constants
#define MENU_BAR_HEIGHT 25
#define MENU_BAR_COLOR 0x000066
#define MENU_BAR_TEXT_COLOR 0xFFFFFF

// Taskbar constants
#define TASKBAR_HEIGHT 30

// Window structure
typedef struct {
    int x;
    int y;
    int width;
    int height;
    char title[32];
    bool is_active;
    bool is_visible;
} window_t;

// Function declarations
void desktop_init(void);
void desktop_main(void);
void desktop_draw(void);
void desktop_draw_background(void);
void desktop_draw_taskbar(void);
void desktop_handle_mouse(int x, int y, bool left_button);
void desktop_handle_keyboard(char key);
void desktop_create_window(const char* title, int x, int y, int width, int height);
void desktop_close_window(int window_id);
void desktop_focus_window(int window_id);
void desktop_move_window(int window_id, int x, int y);
void desktop_resize_window(int window_id, int width, int height);

#endif // DESKTOP_H 