#ifndef MOUSE_STATE_H
#define MOUSE_STATE_H

#include <stdbool.h>
#include "mouse.h"  // Include mouse.h to get mouse_state_t definition

// Function declarations
void set_mouse_state(int x, int y, bool left_button);
void update_mouse_state(int dx, int dy, bool left_button);

#endif // MOUSE_STATE_H 