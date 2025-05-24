#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "../include/terminal.h"
#include "../include/desktop.h"
#include "dock.h"

// Dock item structure
typedef struct {
    char name[32];
    bool is_active;
    bool is_running;
} DockItem;

// Dock configuration
#define DOCK_HEIGHT 60
#define DOCK_ITEM_SIZE 48
#define DOCK_PADDING 10
#define DOCK_MAX_ITEMS 10

// Dock state
static DockItem dock_items[DOCK_MAX_ITEMS];
static int dock_item_count = 0;
static int dock_x = 0;
static int dock_y = 0;

// Colors
#define COLOR_DOCK_BG 0x2C2C2C
#define COLOR_DOCK_ITEM_BG 0x3C3C3C
#define COLOR_DOCK_ITEM_ACTIVE 0x4C4C4C
#define COLOR_DOCK_ITEM_RUNNING 0x5C5C5C

// Initialize dock
void dock_init(void) {
    // Position dock at bottom center of screen
    dock_x = (DESKTOP_WIDTH - (DOCK_MAX_ITEMS * (DOCK_ITEM_SIZE + DOCK_PADDING))) / 2;
    dock_y = DESKTOP_HEIGHT - DOCK_HEIGHT - 10;
    
    // Clear dock items
    memset(dock_items, 0, sizeof(dock_items));
}

// Add item to dock
int dock_add_item(const char* name) {
    if (dock_item_count >= DOCK_MAX_ITEMS) return -1;
    
    DockItem* item = &dock_items[dock_item_count];
    strncpy(item->name, name, sizeof(item->name) - 1);
    item->is_active = false;
    item->is_running = false;
    
    dock_item_count++;
    dock_draw();
    
    return dock_item_count - 1;
}

// Set item running state
void dock_set_running(int item_id, bool running) {
    if (item_id < 0 || item_id >= dock_item_count) return;
    
    dock_items[item_id].is_running = running;
    dock_draw();
}

// Set item active state
void dock_set_active(int item_id, bool active) {
    if (item_id < 0 || item_id >= dock_item_count) return;
    
    dock_items[item_id].is_active = active;
    dock_draw();
}

// Draw dock
void dock_draw(void) {
    // Draw dock background
    for (int x = dock_x - 20; x < dock_x + (DOCK_MAX_ITEMS * (DOCK_ITEM_SIZE + DOCK_PADDING)) + 20; x++) {
        for (int y = dock_y; y < dock_y + DOCK_HEIGHT; y++) {
            terminal_put_pixel(x, y, COLOR_DOCK_BG);
        }
    }
    
    // Draw dock items
    for (int i = 0; i < dock_item_count; i++) {
        DockItem* item = &dock_items[i];
        int item_x = dock_x + (i * (DOCK_ITEM_SIZE + DOCK_PADDING));
        int item_y = dock_y + (DOCK_HEIGHT - DOCK_ITEM_SIZE) / 2;
        
        // Draw item background
        uint32_t bg_color = COLOR_DOCK_ITEM_BG;
        if (item->is_active) bg_color = COLOR_DOCK_ITEM_ACTIVE;
        if (item->is_running) bg_color = COLOR_DOCK_ITEM_RUNNING;
        
        for (int x = 0; x < DOCK_ITEM_SIZE; x++) {
            for (int y = 0; y < DOCK_ITEM_SIZE; y++) {
                terminal_put_pixel(item_x + x, item_y + y, bg_color);
            }
        }
        
        // Draw item name
        terminal_draw_string(item_x + (DOCK_ITEM_SIZE - strlen(item->name) * 8) / 2,
                           item_y + DOCK_ITEM_SIZE + 5,
                           item->name,
                           0xFFFFFF);
    }
}

// Handle mouse click on dock
int dock_handle_click(int x, int y) {
    if (y < dock_y || y > dock_y + DOCK_HEIGHT) return -1;
    
    for (int i = 0; i < dock_item_count; i++) {
        int item_x = dock_x + (i * (DOCK_ITEM_SIZE + DOCK_PADDING));
        int item_y = dock_y + (DOCK_HEIGHT - DOCK_ITEM_SIZE) / 2;
        
        if (x >= item_x && x < item_x + DOCK_ITEM_SIZE &&
            y >= item_y && y < item_y + DOCK_ITEM_SIZE) {
            return i;
        }
    }
    
    return -1;
} 