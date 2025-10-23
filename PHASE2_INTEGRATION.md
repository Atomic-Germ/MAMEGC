# Phase 2: MAME2003 Core Integration - Implementation Plan

## Overview

This document tracks the integration of MAME2003 (MAME 0.78) into the GameCube port.

**Started:** 2025-10-23  
**Status:** In Progress

## Integration Strategy

We'll use an incremental approach:
1. Core infrastructure (memory, CPU interface, driver framework)
2. Single CPU core (Z80 for Pac-Man)
3. Single driver (Pac-Man)
4. Video rendering (basic framebuffer)
5. Test and iterate

## Source Analysis

### MAME2003 libretro Structure
```
mame2003-libretro/src/
├── cpu/              # CPU emulation cores (39 different CPUs)
├── drivers/          # Game drivers (715 files!)
├── machine/          # Hardware abstractions
├── vidhrdw/          # Video hardware
├── sndhrdw/          # Sound hardware  
├── sound/            # Sound cores
├── includes/         # Driver headers
└── Core files:
    - driver.c        # Driver table and management
    - cpuintrf.c      # CPU interface
    - cpuexec.c       # CPU execution
    - memory.c        # Memory management
    - drawgfx.c       # Graphics drawing
    - common.c        # Common utilities
```

### Files We Need (Minimal Set)

**Core Infrastructure:**
- [ ] common.c/h - Common utilities and definitions
- [ ] driver.c/h - Driver framework
- [ ] cpuintrf.c/h - CPU interface
- [ ] cpuexec.c/h - CPU execution
- [ ] memory.c/h - Memory management (critical for GameCube)
- [ ] fileio.c/h - File I/O (adapt for SD card)
- [ ] state.c/h - Save states (adapt for SD)

**Z80 CPU Core (for Pac-Man):**
- [ ] cpu/z80/z80.c/h - Z80 emulation
- [ ] cpu/z80/z80daisy.c/h - Z80 daisy chain

**Pac-Man Driver:**
- [ ] drivers/pacman.c - Pac-Man driver
- [ ] includes/pacman.h - Pac-Man header
- [ ] vidhrdw/pacman.c - Pac-Man video
- [ ] sndhrdw/pacman.c - Pac-Man sound (initially stub)
- [ ] machine/pacman.c - Pac-Man machine (if exists)

**Graphics:**
- [ ] drawgfx.c/h - Graphics primitives
- [ ] palette.c/h - Palette management
- [ ] tilemap.c/h - Tilemap rendering

**Minimal Support:**
- [ ] inptport.c/h - Input handling (adapt for GameCube controller)
- [ ] timer.c/h - Timing system
- [ ] artwork.c/h - Artwork (stub for now)
- [ ] cheat.c/h - Cheats (stub for now)

## Adaptations Needed

### Memory Management
MAME2003 uses malloc/free extensively. GameCube has:
- 24MB main RAM
- 16MB ARAM (audio RAM, slower access)

**Strategy:**
- Pre-allocate memory pools
- Use MEM1 for critical data
- Use ARAM for audio samples
- Minimize dynamic allocation

### File I/O
MAME2003 expects:
- ROM files
- Configuration files
- Save states

**Strategy:**
- Adapt fileio.c for libfat (SD card)
- ROM loading from SD card
- Save states to SD card
- Hardcode initial ROM for testing

### Video Rendering
MAME2003 renders to a framebuffer. GameCube uses GX API.

**Strategy:**
- MAME renders to internal buffer
- Convert buffer to GX texture
- Display via GX blit
- Handle palette conversion
- Support 240p/288p output

### Audio
MAME2003 uses audio streams. GameCube uses AUDIO library.

**Strategy:**
- Initially stub out audio
- Later: adapt audio mixer
- Use ARAM for samples
- Audio callback integration

### Input
MAME2003 has generic input system. GameCube has PAD library.

**Strategy:**
- Map GameCube buttons to arcade inputs
- A = Button 1
- B = Button 2  
- X = Button 3
- Y = Coin
- Start = Start
- D-pad = Joystick

## Integration Steps

### Step 1: Core Infrastructure (Week 1)
- [ ] Copy and adapt common.c/h
- [ ] Create mame2003.h with core definitions
- [ ] Adapt osd.h (OS-dependent layer)
- [ ] Create memory management wrappers
- [ ] Basic driver framework
- [ ] Build system updates

### Step 2: Z80 CPU Core (Week 1-2)
- [ ] Copy Z80 core files
- [ ] Adapt for PowerPC/GameCube
- [ ] Test with simple program
- [ ] Memory interface
- [ ] Interrupt handling

### Step 3: Minimal Driver (Week 2-3)
- [ ] Copy Pac-Man driver
- [ ] Stub video/sound
- [ ] ROM loading (hardcoded data initially)
- [ ] Basic initialization
- [ ] Test driver loads

### Step 4: Video Rendering (Week 3-4)
- [ ] Framebuffer management
- [ ] Palette handling
- [ ] Tile/sprite rendering
- [ ] GX backend
- [ ] Test output

### Step 5: Input System (Week 4)
- [ ] GameCube controller mapping
- [ ] Input port definitions
- [ ] Test controls

### Step 6: Integration Testing (Week 5-6)
- [ ] End-to-end test
- [ ] Performance profiling
- [ ] Bug fixes
- [ ] Documentation

## File Size Estimates

**Core files:** ~500KB source
**Z80 core:** ~100KB source
**Pac-Man driver:** ~50KB source
**Graphics:** ~200KB source
**Support:** ~150KB source

**Total source:** ~1MB
**Compiled:** ~300-500KB (optimized)

## Memory Budget

**Target allocation:**
- Code: ~500KB
- ROM data: ~32KB (Pac-Man)
- Video buffers: ~200KB
- Work RAM: ~200KB
- CPU state: ~10KB
- Tilemap cache: ~100KB
- Sprite cache: ~50KB
- Audio buffers: ~100KB (in ARAM)

**Total:** ~1.2MB (well within 24MB)

## Build System Changes

### Makefile.gc Updates

```makefile
SOURCES := source source/mame2003 \
           source/mame2003/cpu/z80 \
           source/mame2003/drivers \
           source/mame2003/vidhrdw

CFLAGS += -DMAME_GC \
          -DLSB_FIRST \
          -DINLINE="static __inline__" \
          -Wno-unused-variable
```

## Testing Strategy

### Unit Tests
- Z80 instruction tests
- Memory management tests
- Palette conversion tests

### Integration Tests
- Driver initialization
- ROM loading
- Frame rendering
- Input handling

### Hardware Tests
- Run on GameCube
- Performance measurement
- Memory usage validation

## Success Criteria

- [ ] Pac-Man driver initializes
- [ ] ROM loads successfully
- [ ] Z80 executes code
- [ ] Video renders (even if glitchy)
- [ ] Maintains 60fps
- [ ] Uses < 2MB RAM
- [ ] Binary < 1MB

## Risks and Mitigation

**Risk:** Performance insufficient
- Mitigation: Profile early, optimize hot paths, use PGO

**Risk:** Memory overflow
- Mitigation: Pre-allocate, monitor usage, use ARAM

**Risk:** Endianness issues (PowerPC is big-endian)
- Mitigation: Test thoroughly, byte swap where needed

**Risk:** Compiler incompatibilities
- Mitigation: Use portable C, test incrementally

## Progress Tracking

### Week 1
- [x] Repository structure created
- [x] MAME2003 source cloned
- [x] Integration plan documented
- [ ] Core files copied
- [ ] Build system updated

### Week 2
- [ ] Z80 core integrated
- [ ] Basic driver framework
- [ ] First successful build

### Week 3-4
- [ ] Video rendering working
- [ ] ROM loading functional
- [ ] First game visible

### Week 5-6
- [ ] Input working
- [ ] Playable game
- [ ] Performance optimized

## Next Actions

1. Copy core MAME2003 files
2. Create GameCube-specific osd layer
3. Update build system
4. Test compilation
5. Begin Z80 integration

---

**Last Updated:** 2025-10-23  
**Next Review:** After Step 1 completion
