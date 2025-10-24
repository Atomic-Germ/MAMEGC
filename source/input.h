#ifndef INPUT_H
#define INPUT_H

#include <gccore.h>
#include <stdint.h>

// Input button masks (matching MAME conventions)
#define INPUT_UP        (1 << 0)
#define INPUT_DOWN      (1 << 1)
#define INPUT_LEFT      (1 << 2)
#define INPUT_RIGHT     (1 << 3)
#define INPUT_BUTTON1   (1 << 4)  // A button
#define INPUT_BUTTON2   (1 << 5)  // B button
#define INPUT_BUTTON3   (1 << 6)  // X button
#define INPUT_BUTTON4   (1 << 7)  // Y button
#define INPUT_BUTTON5   (1 << 8)  // L button
#define INPUT_BUTTON6   (1 << 9)  // R button
#define INPUT_START     (1 << 10) // START button
#define INPUT_COIN      (1 << 11) // Z button (coin)

// Analog stick threshold for digital conversion
#define ANALOG_THRESHOLD 20

// Input state for one player
typedef struct {
    uint32_t buttons;      // Current button state (bitmask)
    uint32_t buttons_old;  // Previous button state
    uint32_t buttons_pressed;  // Newly pressed this frame
    uint32_t buttons_released; // Newly released this frame
    int8_t analog_x;       // Analog stick X (-128 to 127)
    int8_t analog_y;       // Analog stick Y (-128 to 127)
} input_state_t;

// Global input state (up to 4 players)
extern input_state_t input_state[4];

// Initialize input system
void input_init(void);

// Update input state (call once per frame)
void input_update(void);

// Check if button is currently held
int input_button_held(int player, uint32_t button_mask);

// Check if button was just pressed this frame
int input_button_pressed(int player, uint32_t button_mask);

// Check if button was just released this frame
int input_button_released(int player, uint32_t button_mask);

// Get analog stick values
void input_get_analog(int player, int8_t *x, int8_t *y);

#endif // INPUT_H
