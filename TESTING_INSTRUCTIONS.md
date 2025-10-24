# Phase 4 Testing Instructions

## What's New in v0.4.0
- ✅ Full GameCube controller support
- ✅ Interactive input test mode
- ✅ 4-player controller support
- ✅ Analog stick + D-Pad controls

## How to Test

### 1. Load the Binary
- Copy `executables/mamegc-gc.dol` to your SD card
- Launch via your preferred homebrew loader

### 2. Initial Boot
You'll see:
```
================================
MAME2003 GameCube Port - v0.4.0
================================
Status: Phase 4 - Input System
```

The system will initialize and show the video test pattern with colored bars.

### 3. Input Test Mode
After the video displays, you'll enter **Input Test Mode**:

```
*** Input Test Mode ***
Press buttons to test input
Press START twice to continue
```

### 4. Test Your Controls

Try each button and watch the display:

**Movement:**
- Press D-Pad Up/Down/Left/Right → See "UP", "DOWN", "LEFT", "RIGHT"
- Move Control Stick → See "Analog(x,y)" values
- Stick past ±20 threshold → Also triggers directional inputs

**Action Buttons:**
- Press A → See "A"
- Press B → See "B"
- Press X → See "X"
- Press Y → See "Y"
- Press L → See "L"
- Press R → See "R"

**Special Buttons:**
- Press Z → See "COIN"
- Press START → See "START" and counter increments

### 5. Continue
- Press START twice to exit input test
- Press START once more to exit the program

## Expected Output

### Good Signs ✅
- All buttons respond immediately
- Analog stick shows values from -128 to 127
- Moving stick past ±20 triggers digital directions
- START counter increases on each press
- Display updates in real-time

### Test with Multiple Controllers 🎮🎮
If you have multiple controllers:
1. Plug in Controller 2
2. Press buttons on Controller 2
3. You should see "Player 2: ..." appear

## Button Mapping Reference

```
GameCube       MAME          Pac-Man Use
--------       ----          -----------
D-Pad          Movement      Move Pac-Man
Stick          Movement      Move Pac-Man (alt)
A              Button 1      (unused in Pac-Man)
B              Button 2      (unused in Pac-Man)
X              Button 3      (unused in Pac-Man)
Y              Button 4      (unused in Pac-Man)
L              Button 5      (unused in Pac-Man)
R              Button 6      (unused in Pac-Man)
Z              Coin          Insert Coin
START          Start         Start Game
```

## Technical Details

### Analog Stick Behavior
- **Dead Zone**: ±20 (15% of range)
- **Range**: -128 to +127
- **Digital Conversion**: Automatic when past threshold
- **Works with D-Pad**: Both inputs combine

### Button States
The system tracks three states:
- **Held**: Button is currently down
- **Pressed**: Button was just pressed this frame
- **Released**: Button was just released this frame

### Performance
- **Polling Rate**: 60 Hz (once per frame)
- **Latency**: ~16ms (1 frame)
- **Buffer**: None (immediate response)

## Known Behavior

### By Design
- Input test screen doesn't clear by itself (press buttons to see updates)
- START must be pressed twice (prevents accidental skips)
- Analog values may jitter slightly when stick is centered (normal)

### Not Implemented Yet
- No button remapping
- No analog stick calibration
- No turbo button support
- No input recording/playback

## Next Phase: Audio

Phase 5 will add:
- Sound effects
- Music playback
- Audio mixing
- YM2149 PSG emulation

## Troubleshooting

**Problem**: No input response
- **Solution**: Make sure controller is plugged in before boot

**Problem**: Analog stick always shows 0,0
- **Solution**: Try unplugging and re-plugging controller

**Problem**: Can't exit input test
- **Solution**: Press START button twice (watch counter: 0/2 → 1/2 → exits)

**Problem**: Display doesn't update
- **Solution**: This is normal - press more buttons to see updates

## Feedback

Please test and report:
1. ✅ Do all buttons work?
2. ✅ Does analog stick respond smoothly?
3. ✅ Does digital conversion feel natural?
4. ✅ Any buttons not working?
5. ✅ Multiple controllers working?

## Files to Test
- **GameCube**: `executables/mamegc-gc.dol` (481 KB)
- **Dolphin**: Also works in Dolphin emulator

---

**Have fun testing!** 🎮✨
