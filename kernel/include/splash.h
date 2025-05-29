#ifndef SPLASH_H
#define SPLASH_H

#include <stdint.h>
#include <stdbool.h>

// Splash screen dimensions
#define SPLASH_WIDTH 640
#define SPLASH_HEIGHT 480

// Splash screen colors
#define SPLASH_COLOR_BACKGROUND 0x000000
#define SPLASH_COLOR_TEXT 0xFFFFFF
#define SPLASH_COLOR_LOGO 0x00FF00

// Splash screen timing (in milliseconds)
#define SPLASH_DISPLAY_TIME 3000

// Function declarations
void splash_init(void);
void splash_display(void);
void splash_draw_logo(int x, int y);
void splash_draw_text(const char* text, int x, int y, uint32_t color);
void splash_draw_progress_bar(int x, int y, int width, int height, int progress);
void splash_cleanup(void);
void splash_show(void);

#endif // SPLASH_H 