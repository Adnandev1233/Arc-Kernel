#include <stdint.h>
#include <string.h>
#include <terminal.h>

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
        terminal_move_cursor((VGA_WIDTH - strlen(SPLASH_ART[i])) / 2, start_y + i);
        terminal_writestring(SPLASH_ART[i]);
    }
    
    // Display loading animation
    int loading_y = start_y + SPLASH_LINES + 2;
    int frame = 0;
    
    // Animate for a few seconds
    for (int i = 0; i < 24; i++) {  // 3 seconds with 8 frames
        terminal_move_cursor((VGA_WIDTH - strlen(LOADING_FRAMES[frame])) / 2, loading_y);
        terminal_writestring(LOADING_FRAMES[frame]);
        
        // Wait a bit
        for (volatile int j = 0; j < 1000000; j++);
        
        frame = (frame + 1) % LOADING_FRAMES_COUNT;
    }
    
    // Clear loading animation
    terminal_move_cursor((VGA_WIDTH - strlen(LOADING_FRAMES[0])) / 2, loading_y);
    terminal_writestring("      ");
    
    // Move cursor to bottom
    terminal_move_cursor(0, VGA_HEIGHT - 1);
} 