#ifndef MOUSE_STATE_H
#define MOUSE_STATE_H

#include <stdbool.h>

// Function declarations
void get_mouse_state(int* x, int* y, bool* left_button);
void set_mouse_state(int x, int y, bool left_button);
void update_mouse_state(int dx, int dy, bool left_button);

#endif // MOUSE_STATE_H 