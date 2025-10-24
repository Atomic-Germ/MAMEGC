# Phase 4: Input System

## Goal
Add GameCube controller support with proper button mapping for arcade games.

## Components to Implement

### 1. Input Header (`source/input.h`)
- Input state structure
- Button definitions
- Function prototypes

### 2. Input Implementation (`source/input.c`)
- PAD library initialization
- Controller polling
- Button state management
- Analog stick handling

### 3. MAME Input Integration (`source/mame_input.c`)
- Map GameCube buttons to MAME inputs
- Handle player 1 and player 2
- Coin/start buttons
- Directional input

### 4. Main Integration (`source/main.c`)
- Initialize input system
- Poll inputs in main loop
- Display input state

## GameCube Controller Mapping

### Digital Buttons:
- **D-Pad**: Movement (up/down/left/right)
- **A**: Button 1 (primary action)
- **B**: Button 2 (secondary action)
- **X**: Button 3
- **Y**: Button 4
- **L/R**: Button 5/6
- **Z**: Coin insert
- **START**: Start game / Pause

### Analog Controls:
- **Control Stick**: Movement (alternative to D-Pad)
- **C-Stick**: Not used initially

## Testing
Test Pac-Man controls:
- D-Pad movement
- Analog stick movement
- START to begin game
- Z to insert coin
