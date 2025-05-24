#ifndef MENU_BAR_H
#define MENU_BAR_H

// Initialize menu bar
void menu_bar_init(void);

// Add menu to menu bar
// Returns menu ID or -1 on error
int menu_bar_add_menu(const char* name);

// Add item to menu
// Returns item ID or -1 on error
int menu_bar_add_item(int menu_id, const char* name, void (*callback)(void));

// Draw menu bar
void menu_bar_draw(void);

// Handle mouse click on menu bar
void menu_bar_handle_click(int x, int y);

#endif // MENU_BAR_H 