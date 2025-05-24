#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "../include/terminal.h"
#include "menu_bar.h"

// Screen dimensions
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

// Menu item structure
typedef struct {
    char name[32];
    bool is_enabled;
    void (*callback)(void);
} MenuItem;

// Menu structure
typedef struct {
    char name[32];
    MenuItem items[10];
    int item_count;
    bool is_open;
} Menu;

// Menu bar configuration
#define MENU_BAR_HEIGHT 20
#define MENU_PADDING 10
#define MAX_MENUS 5

// Menu bar state
static Menu menus[MAX_MENUS];
static int menu_count = 0;
static int active_menu = -1;

// Colors
#define COLOR_MENU_BAR_BG 0x2C2C2C
#define COLOR_MENU_BG 0x3C3C3C
#define COLOR_MENU_TEXT 0xFFFFFF
#define COLOR_MENU_DISABLED 0x808080
#define COLOR_MENU_HOVER 0x4C4C4C

// Initialize menu bar
void menu_bar_init(void) {
    // Clear menus
    memset(menus, 0, sizeof(menus));
    
    // Add default menus
    menu_bar_add_menu("File");
    menu_bar_add_menu("Edit");
    menu_bar_add_menu("View");
    menu_bar_add_menu("Window");
    menu_bar_add_menu("Help");
}

// Add menu to menu bar
int menu_bar_add_menu(const char* name) {
    if (menu_count >= MAX_MENUS) return -1;
    
    Menu* menu = &menus[menu_count];
    strncpy(menu->name, name, sizeof(menu->name) - 1);
    menu->item_count = 0;
    menu->is_open = false;
    
    return menu_count++;
}

// Add item to menu
int menu_bar_add_item(int menu_id, const char* name, void (*callback)(void)) {
    if (menu_id < 0 || menu_id >= menu_count) return -1;
    
    Menu* menu = &menus[menu_id];
    if (menu->item_count >= 10) return -1;
    
    MenuItem* item = &menu->items[menu->item_count];
    strncpy(item->name, name, sizeof(item->name) - 1);
    item->is_enabled = true;
    item->callback = callback;
    
    return menu->item_count++;
}

// Draw menu bar
void menu_bar_draw(void) {
    // Draw menu bar background
    for (int x = 0; x < SCREEN_WIDTH; x++) {
        for (int y = 0; y < MENU_BAR_HEIGHT; y++) {
            terminal_put_pixel(x, y, COLOR_MENU_BAR_BG);
        }
    }
    
    // Draw menus
    int x = MENU_PADDING;
    for (int i = 0; i < menu_count; i++) {
        Menu* menu = &menus[i];
        
        // Draw menu name
        terminal_draw_string(x, 3, menu->name, COLOR_MENU_TEXT);
        
        // Draw menu items if menu is open
        if (menu->is_open) {
            int menu_x = x;
            int menu_y = MENU_BAR_HEIGHT;
            int max_width = 0;
            
            // Calculate menu width
            for (int j = 0; j < menu->item_count; j++) {
                int width = strlen(menu->items[j].name) * 8;
                if (width > max_width) max_width = width;
            }
            max_width += 20; // Padding
            
            // Draw menu background
            for (int mx = menu_x; mx < menu_x + max_width; mx++) {
                for (int my = menu_y; my < menu_y + (menu->item_count * 20); my++) {
                    terminal_put_pixel(mx, my, COLOR_MENU_BG);
                }
            }
            
            // Draw menu items
            for (int j = 0; j < menu->item_count; j++) {
                MenuItem* item = &menu->items[j];
                uint32_t text_color = item->is_enabled ? COLOR_MENU_TEXT : COLOR_MENU_DISABLED;
                
                terminal_draw_string(menu_x + 10,
                                   menu_y + (j * 20) + 3,
                                   item->name,
                                   text_color);
            }
        }
        
        x += strlen(menu->name) * 8 + MENU_PADDING * 2;
    }
}

// Handle mouse click on menu bar
void menu_bar_handle_click(int x, int y) {
    if (y >= MENU_BAR_HEIGHT) {
        // Check if click is in open menu
        if (active_menu >= 0) {
            Menu* menu = &menus[active_menu];
            if (menu->is_open) {
                int menu_x = MENU_PADDING;
                for (int i = 0; i < active_menu; i++) {
                    menu_x += strlen(menus[i].name) * 8 + MENU_PADDING * 2;
                }
                
                int menu_y = MENU_BAR_HEIGHT;
                int max_width = 0;
                for (int j = 0; j < menu->item_count; j++) {
                    int width = strlen(menu->items[j].name) * 8;
                    if (width > max_width) max_width = width;
                }
                max_width += 20;
                
                if (x >= menu_x && x < menu_x + max_width &&
                    y >= menu_y && y < menu_y + (menu->item_count * 20)) {
                    int item_index = (y - menu_y) / 20;
                    if (item_index >= 0 && item_index < menu->item_count) {
                        MenuItem* item = &menu->items[item_index];
                        if (item->is_enabled && item->callback) {
                            item->callback();
                        }
                    }
                }
                
                menu->is_open = false;
                active_menu = -1;
            }
        }
        return;
    }
    
    // Check if click is on menu name
    int menu_x = MENU_PADDING;
    for (int i = 0; i < menu_count; i++) {
        Menu* menu = &menus[i];
        int menu_width = strlen(menu->name) * 8 + MENU_PADDING * 2;
        
        if (x >= menu_x && x < menu_x + menu_width) {
            // Close other menus
            for (int j = 0; j < menu_count; j++) {
                menus[j].is_open = false;
            }
            
            // Toggle clicked menu
            menu->is_open = !menu->is_open;
            active_menu = menu->is_open ? i : -1;
            break;
        }
        
        menu_x += menu_width;
    }
    
    menu_bar_draw();
} 