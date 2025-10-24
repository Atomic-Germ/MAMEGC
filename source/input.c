#include "input.h"
#include <gccore.h>
#include <string.h>
#include <stdio.h>

// Global input state
input_state_t input_state[4];

void input_init(void) {
    printf("Initializing input system...\n");
    
    // Clear input state
    memset(input_state, 0, sizeof(input_state));
    
    // Initialize PAD library
    PAD_Init();
    
    printf("Input system initialized!\n");
}

void input_update(void) {
    PADStatus pad_status[4];
    
    // Scan for controller changes
    PAD_ScanPads();
    
    // Read all controller states
    for (int i = 0; i < 4; i++) {
        // Save old button state
        input_state[i].buttons_old = input_state[i].buttons;
        
        // Check if controller is connected
        if (PAD_Read(pad_status) < 0 || !(PAD_ButtonsHeld(i) || PAD_ButtonsDown(i) || PAD_ButtonsUp(i))) {
            // Controller not connected, clear state
            input_state[i].buttons = 0;
            input_state[i].analog_x = 0;
            input_state[i].analog_y = 0;
            continue;
        }
        
        // Get controller state
        uint16_t gc_buttons = PAD_ButtonsHeld(i);
        
        // Clear current button state
        input_state[i].buttons = 0;
        
        // Map D-Pad
        if (gc_buttons & PAD_BUTTON_UP)
            input_state[i].buttons |= INPUT_UP;
        if (gc_buttons & PAD_BUTTON_DOWN)
            input_state[i].buttons |= INPUT_DOWN;
        if (gc_buttons & PAD_BUTTON_LEFT)
            input_state[i].buttons |= INPUT_LEFT;
        if (gc_buttons & PAD_BUTTON_RIGHT)
            input_state[i].buttons |= INPUT_RIGHT;
        
        // Map face buttons
        if (gc_buttons & PAD_BUTTON_A)
            input_state[i].buttons |= INPUT_BUTTON1;
        if (gc_buttons & PAD_BUTTON_B)
            input_state[i].buttons |= INPUT_BUTTON2;
        if (gc_buttons & PAD_BUTTON_X)
            input_state[i].buttons |= INPUT_BUTTON3;
        if (gc_buttons & PAD_BUTTON_Y)
            input_state[i].buttons |= INPUT_BUTTON4;
        
        // Map shoulder buttons
        if (gc_buttons & PAD_TRIGGER_L)
            input_state[i].buttons |= INPUT_BUTTON5;
        if (gc_buttons & PAD_TRIGGER_R)
            input_state[i].buttons |= INPUT_BUTTON6;
        
        // Map special buttons
        if (gc_buttons & PAD_BUTTON_START)
            input_state[i].buttons |= INPUT_START;
        if (gc_buttons & PAD_TRIGGER_Z)
            input_state[i].buttons |= INPUT_COIN;
        
        // Read analog stick
        int8_t stick_x = PAD_StickX(i);
        int8_t stick_y = PAD_StickY(i);
        
        input_state[i].analog_x = stick_x;
        input_state[i].analog_y = stick_y;
        
        // Convert analog to digital if over threshold
        if (stick_x < -ANALOG_THRESHOLD)
            input_state[i].buttons |= INPUT_LEFT;
        else if (stick_x > ANALOG_THRESHOLD)
            input_state[i].buttons |= INPUT_RIGHT;
        
        if (stick_y < -ANALOG_THRESHOLD)
            input_state[i].buttons |= INPUT_DOWN;
        else if (stick_y > ANALOG_THRESHOLD)
            input_state[i].buttons |= INPUT_UP;
        
        // Calculate pressed/released buttons
        input_state[i].buttons_pressed = input_state[i].buttons & ~input_state[i].buttons_old;
        input_state[i].buttons_released = ~input_state[i].buttons & input_state[i].buttons_old;
    }
}

int input_button_held(int player, uint32_t button_mask) {
    if (player < 0 || player >= 4)
        return 0;
    return (input_state[player].buttons & button_mask) != 0;
}

int input_button_pressed(int player, uint32_t button_mask) {
    if (player < 0 || player >= 4)
        return 0;
    return (input_state[player].buttons_pressed & button_mask) != 0;
}

int input_button_released(int player, uint32_t button_mask) {
    if (player < 0 || player >= 4)
        return 0;
    return (input_state[player].buttons_released & button_mask) != 0;
}

void input_get_analog(int player, int8_t *x, int8_t *y) {
    if (player < 0 || player >= 4) {
        if (x) *x = 0;
        if (y) *y = 0;
        return;
    }
    if (x) *x = input_state[player].analog_x;
    if (y) *y = input_state[player].analog_y;
}
