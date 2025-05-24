#ifndef DOCK_H
#define DOCK_H

// Initialize dock
void dock_init(void);

// Add item to dock
// Returns item ID or -1 on error
int dock_add_item(const char* name);

// Set item running state
void dock_set_running(int item_id, bool running);

// Set item active state
void dock_set_active(int item_id, bool active);

// Draw dock
void dock_draw(void);

// Handle mouse click on dock
// Returns clicked item ID or -1 if no item clicked
int dock_handle_click(int x, int y);

#endif // DOCK_H 