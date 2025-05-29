#include <stdint.h>
#include <string.h>
#include "include/terminal.h"
#include "include/splash.h"

// VGA constants
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

// Splash screen ASCII art
static const char* SPLASH_ART[] = {
    "    _    ____   ____  _____  ____  ",
    "   / \\  |  _ \\ / ___||_   _|/ ___| ",
    "  / _ \\ | |_) |\\___ \\  | |  \\___ \\ ",
    " / ___ \\|  _ < ___) | | |   ___) |",
    "/_/   \\_\\_| \\_\\____/  |_|  |____/ ",
    "                                  ",
    "     Welcome to ArcOS v0.1        ",
    "                                  "
};

// Loading animation frames
static const char* LOADING_FRAMES[] = {
    "[    ]",
    "[=   ]",
    "[==  ]",
    "[=== ]",
    "[====]",
    "[ ===]",
    "[  ==]",
    "[   =]"
};

#define SPLASH_LINES (sizeof(SPLASH_ART) / sizeof(SPLASH_ART[0]))
#define LOADING_FRAMES_COUNT (sizeof(LOADING_FRAMES) / sizeof(LOADING_FRAMES[0]))

// Display splash screen with loading animation
void display_splash_screen(void) {
    // Clear screen
    terminal_clear();
    
    // Center the splash screen vertically
    int start_y = (VGA_HEIGHT - SPLASH_LINES) / 2;
    
    // Display splash art
    for (size_t i = 0; i < SPLASH_LINES; i++) {
        // Calculate cursor position
        int x = (VGA_WIDTH - strlen(SPLASH_ART[i])) / 2;
        int y = start_y + i;
        
        // Move cursor to position
        for (int j = 0; j < y; j++) {
            terminal_putchar('\n');
        }
        for (int j = 0; j < x; j++) {
            terminal_putchar(' ');
        }
        
        terminal_write_string(SPLASH_ART[i]);
    }
    
    // Display loading animation
    int loading_y = start_y + SPLASH_LINES + 2;
    int frame = 0;
    
    // Animate for a few seconds
    for (int i = 0; i < 24; i++) {  // 3 seconds with 8 frames
        // Calculate cursor position
        int x = (VGA_WIDTH - strlen(LOADING_FRAMES[frame])) / 2;
        
        // Move cursor to position
        for (int j = 0; j < loading_y; j++) {
            terminal_putchar('\n');
        }
        for (int j = 0; j < x; j++) {
            terminal_putchar(' ');
        }
        
        terminal_write_string(LOADING_FRAMES[frame]);
        
        // Wait a bit
        for (volatile int j = 0; j < 1000000; j++);
        
        frame = (frame + 1) % LOADING_FRAMES_COUNT;
    }
    
    // Clear loading animation
    // Calculate cursor position
    int x = (VGA_WIDTH - strlen(LOADING_FRAMES[0])) / 2;
    
    // Move cursor to position
    for (int j = 0; j < loading_y; j++) {
        terminal_putchar('\n');
    }
    for (int j = 0; j < x; j++) {
        terminal_putchar(' ');
    }
    
    terminal_write_string("      ");
    
    // Move cursor to bottom
    for (int j = 0; j < VGA_HEIGHT - 1; j++) {
        terminal_putchar('\n');
    }
}

void splash_show(void) {
    // Basic splash screen
    terminal_clear();
    terminal_set_color(VGA_COLOR_LIGHT_GREEN);
    terminal_write_string("Welcome to ArcOS!\n");
    terminal_write_string("Loading...\n");
} 