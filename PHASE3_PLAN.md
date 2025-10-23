# Phase 3: Pac-Man Driver & Video System

## Goal
Get Pac-Man running on GameCube with basic video output

## Milestones

### Milestone 1: Pac-Man Driver Foundation (Week 1)
- [ ] Study Pac-Man hardware architecture
- [ ] Create driver structure
- [ ] Hardcode Pac-Man ROM data
- [ ] Initialize Pac-Man hardware
- [ ] Map memory regions

### Milestone 2: Video System (Week 1-2)
- [ ] Implement tile renderer
- [ ] Character/sprite system
- [ ] Color palette (Pac-Man uses 16 colors)
- [ ] GX integration for display
- [ ] Frame buffer management

### Milestone 3: Game Logic (Week 2)
- [ ] Main game loop
- [ ] Input handling (GameCube controller → Pac-Man controls)
- [ ] Interrupt handling (V-blank)
- [ ] Game state management

### Milestone 4: Polish & Optimization (Week 2-3)
- [ ] Performance tuning
- [ ] Sound stubs
- [ ] UI/menu system
- [ ] Save states

## Technical Details

### Pac-Man Hardware Specs
- **CPU:** Z80 @ 3.072 MHz ✅ (we have this!)
- **Display:** 224×288 pixels, 60 Hz
- **Tiles:** 8×8 pixels, 256 characters
- **Sprites:** 8 sprites, 16×16 pixels
- **Colors:** 16-color palette
- **ROM:** 16KB program ROM
- **RAM:** 1KB video RAM, 1KB color RAM, 2KB general RAM

### Memory Map
```
0x0000-0x3FFF: ROM (16KB)
0x4000-0x43FF: Video RAM (1KB)
0x4400-0x47FF: Color RAM (1KB)
0x4800-0x4BFF: General RAM (1KB)
0x4C00-0x4FEF: General RAM (1KB)
0x5000-0x503F: Sprite positions
0x5040-0x507F: I/O, interrupts, sound
```

### Implementation Strategy

**Phase 3.1: Start Small**
1. Hardcode a minimal Pac-Man ROM snippet
2. Initialize video RAM with a test pattern
3. Render tiles to GX framebuffer
4. Display on GameCube screen

**Phase 3.2: Add Game Logic**
1. Load full Pac-Man ROM (hardcoded)
2. Run game initialization
3. Execute game loop
4. Handle input

**Phase 3.3: Polish**
1. Optimize rendering
2. Add sound (basic)
3. Save states
4. Menu system

## Files to Create

```
source/drivers/
├── pacman/
│   ├── pacman.c         - Main driver
│   ├── pacman.h         - Driver header
│   ├── pacman_rom.h     - Hardcoded ROM data
│   └── pacman_gfx.h     - Graphics data
│
source/video/
├── video.c              - Video system
├── video.h              - Video header
├── tiles.c              - Tile renderer
├── tiles.h              - Tile header
├── palette.c            - Color palette
└── palette.h            - Palette header
```

## Success Criteria

- [ ] Pac-Man ROM boots
- [ ] Z80 executes Pac-Man code
- [ ] Video RAM accessible
- [ ] Tiles render to screen
- [ ] Maze visible on GameCube
- [ ] Pac-Man sprite visible
- [ ] Game runs at 60 FPS

## Timeline

**Week 1:**
- Day 1-2: Driver structure + ROM integration
- Day 3-4: Video system foundation
- Day 5: First pixels on screen

**Week 2:**
- Day 1-3: Full rendering system
- Day 4-5: Game loop + input

**Week 3:**
- Day 1-2: Polish + optimization
- Day 3: Testing + documentation

## Starting Point

We have:
- ✅ Z80 CPU working
- ✅ Memory system operational
- ✅ Build system ready
- ✅ GameCube app framework

We need:
- ⏳ Pac-Man driver
- ⏳ Video rendering
- ⏳ Tile system
- ⏳ Color palette
- ⏳ Input handling

Let's begin! 🚀
