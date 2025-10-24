#include "input.h"
#include <stdio.h>
#include <string.h>

// MAME input port state (simplified)
// In full MAME, this would be much more complex
static uint32_t mame_input_ports[8];

void mame_input_init(void) {
    printf("Initializing MAME input mapping...\n");
    memset(mame_input_ports, 0, sizeof(mame_input_ports));
    printf("MAME input mapping initialized!\n");
}

void mame_input_update(void) {
    // Update GameCube controller state first
    input_update();
    
    // Clear MAME input ports
    memset(mame_input_ports, 0, sizeof(mame_input_ports));
    
    // Map Player 1 (controller 0)
    // Port 0: Player 1 inputs
    if (input_button_held(0, INPUT_UP))
        mame_input_ports[0] |= (1 << 0);  // P1 Up
    if (input_button_held(0, INPUT_DOWN))
        mame_input_ports[0] |= (1 << 1);  // P1 Down
    if (input_button_held(0, INPUT_LEFT))
        mame_input_ports[0] |= (1 << 2);  // P1 Left
    if (input_button_held(0, INPUT_RIGHT))
        mame_input_ports[0] |= (1 << 3);  // P1 Right
    if (input_button_held(0, INPUT_BUTTON1))
        mame_input_ports[0] |= (1 << 4);  // P1 Button 1
    if (input_button_held(0, INPUT_BUTTON2))
        mame_input_ports[0] |= (1 << 5);  // P1 Button 2
    if (input_button_held(0, INPUT_BUTTON3))
        mame_input_ports[0] |= (1 << 6);  // P1 Button 3
    
    // Port 1: System inputs
    if (input_button_held(0, INPUT_START))
        mame_input_ports[1] |= (1 << 0);  // P1 Start
    if (input_button_held(0, INPUT_COIN))
        mame_input_ports[1] |= (1 << 2);  // Coin 1
    
    // Map Player 2 (controller 1)
    if (input_button_held(1, INPUT_UP))
        mame_input_ports[2] |= (1 << 0);  // P2 Up
    if (input_button_held(1, INPUT_DOWN))
        mame_input_ports[2] |= (1 << 1);  // P2 Down
    if (input_button_held(1, INPUT_LEFT))
        mame_input_ports[2] |= (1 << 2);  // P2 Left
    if (input_button_held(1, INPUT_RIGHT))
        mame_input_ports[2] |= (1 << 3);  // P2 Right
    if (input_button_held(1, INPUT_BUTTON1))
        mame_input_ports[2] |= (1 << 4);  // P2 Button 1
    if (input_button_held(1, INPUT_BUTTON2))
        mame_input_ports[2] |= (1 << 5);  // P2 Button 2
    if (input_button_held(1, INPUT_BUTTON3))
        mame_input_ports[2] |= (1 << 6);  // P2 Button 3
    
    if (input_button_held(1, INPUT_START))
        mame_input_ports[1] |= (1 << 1);  // P2 Start
    if (input_button_held(1, INPUT_COIN))
        mame_input_ports[1] |= (1 << 3);  // Coin 2
}

uint32_t mame_input_port_read(int port) {
    if (port < 0 || port >= 8)
        return 0;
    return mame_input_ports[port];
}

// Test function to display input state
void mame_input_test_display(void) {
    printf("\n=== Input Test ===\n");
    
    for (int i = 0; i < 2; i++) {
        if (input_state[i].buttons == 0 && input_state[i].analog_x == 0 && input_state[i].analog_y == 0)
            continue;
        
        printf("P%d: ", i + 1);
        
        if (input_button_held(i, INPUT_UP)) printf("UP ");
        if (input_button_held(i, INPUT_DOWN)) printf("DOWN ");
        if (input_button_held(i, INPUT_LEFT)) printf("LEFT ");
        if (input_button_held(i, INPUT_RIGHT)) printf("RIGHT ");
        if (input_button_held(i, INPUT_BUTTON1)) printf("A ");
        if (input_button_held(i, INPUT_BUTTON2)) printf("B ");
        if (input_button_held(i, INPUT_BUTTON3)) printf("X ");
        if (input_button_held(i, INPUT_BUTTON4)) printf("Y ");
        if (input_button_held(i, INPUT_BUTTON5)) printf("L ");
        if (input_button_held(i, INPUT_BUTTON6)) printf("R ");
        if (input_button_held(i, INPUT_START)) printf("START ");
        if (input_button_held(i, INPUT_COIN)) printf("COIN ");
        
        int8_t x, y;
        input_get_analog(i, &x, &y);
        if (x != 0 || y != 0) {
            printf("Analog(%d,%d) ", x, y);
        }
        
        printf("\n");
    }
}
