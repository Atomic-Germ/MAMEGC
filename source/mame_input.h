#ifndef MAME_INPUT_H
#define MAME_INPUT_H

#include <stdint.h>

// Initialize MAME input mapping
void mame_input_init(void);

// Update MAME input state from GameCube controllers
void mame_input_update(void);

// Read MAME input port
uint32_t mame_input_port_read(int port);

// Test function to display input state
void mame_input_test_display(void);

#endif // MAME_INPUT_H
