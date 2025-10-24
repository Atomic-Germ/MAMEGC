# MAME2003 GameCube Port - Development Roadmap

## Project Overview

MAME2003 is an arcade machine emulator that reproduces classic arcade hardware in software. This project ports MAME2003 to the Nintendo GameCube to run original arcade ROM files.

## Current Status: Phase 6 (SD Card ROM Loading) - CRITICAL BLOCKER

The emulator hardware is complete but **cannot run games** because it cannot load ROM files from SD card. This is the current development priority.

## Completed Phases

### Phase 1: Foundation ✅ COMPLETE
- GameCube build environment and Docker integration
- GitHub Actions CI/CD pipeline  
- Basic video initialization and controller input
- Profile-Guided Optimization support

### Phase 2: MAME2003 Core Integration ✅ COMPLETE
- MAME2003 source integrated and compiling for GameCube
- Z80 CPU emulator fully functional
- Memory management system (64KB address space)
- CPU interface layer working

### Phase 3: Video System ✅ COMPLETE  
- Tile-based rendering engine
- Color palette system
- RGB to YUV422 conversion for GameCube display
- 640x480 framebuffer output

### Phase 4: Input System ✅ COMPLETE
- GameCube controller integration
- Button mapping for arcade controls  
- Analog stick and multi-controller support

### Phase 5: Audio System ❌ ABANDONED
- Sound chip emulation proved extremely complex
- Deferred indefinitely after significant effort

---

## Current Priority: Phase 6

### Phase 6: SD Card ROM Loading 🚨 CRITICAL BLOCKER

## Project Phases

### Phase 1: Foundation ✅ (Current - Complete)

**Goal:** Establish build system and minimal GameCube application scaffold.

- [x] GameCube build environment with devkitPPC
- [x] Docker-based reproducible builds
- [x] GitHub Actions CI/CD pipeline
- [x] Basic video initialization and console output
- [x] Controller input polling
- [x] Profile-Guided Optimization (PGO) support
- [x] Project documentation and structure

**Deliverable:** Minimal "Hello World" DOL that runs on GameCube

---

### Phase 2: MAME2003 Core Integration (Next)

**Goal:** Integrate MAME2003 source and establish basic emulation framework.

**Tasks:**

1. **Core Source Integration**
   - [ ] Import MAME2003 core source (libretro core)
   - [ ] Adapt build system for MAME source tree
   - [ ] Resolve compilation issues for PowerPC/GameCube
   - [ ] Remove unnecessary drivers and dependencies

2. **Memory Management**
   - [ ] Implement GameCube-aware memory allocation
   - [ ] Set up memory pools for ROM/game data
   - [ ] Optimize for 24MB main RAM + 16MB ARAM

3. **Initial Driver Support**
   - [ ] Choose 1-2 simple arcade games (8-bit era)
   - [ ] Compile minimal driver set
   - [ ] Test basic game initialization

4. **Video Backend**
   - [ ] Implement framebuffer rendering via GX
   - [ ] Basic palette and tile rendering
   - [ ] Resolution scaling for GameCube TV output

**Deliverable:** Single hardcoded game runs on GameCube (proof of concept)

**Estimated Effort:** 4-6 weeks

---

### Phase 3: Filesystem and ROM Loading

**Goal:** Enable loading ROMs from SD card storage.

**Tasks:**

1. **SD Card Support**
   - [ ] Integrate libfat for SD Gecko/SD2SP2
   - [ ] Implement ROM file loading
   - [ ] Add ZIP file support for compressed ROMs
   - [ ] Error handling and validation

2. **ROM Management**
   - [ ] ROM scanning and detection
   - [ ] CRC verification
   - [ ] ROM set validation
   - [ ] Support for merged/split ROM sets

3. **Save States**
   - [ ] Implement save state to SD card
   - [ ] Load state functionality
   - [ ] Multiple save slots
   - [ ] SRAM/NVRAM persistence

**Deliverable:** Load and run multiple games from SD card

**Estimated Effort:** 3-4 weeks

---

### Phase 4: User Interface

**Goal:** Create user-friendly menu system for game selection and configuration.

**Tasks:**

1. **Main Menu**
   - [ ] Game browser/selector
   - [ ] Directory navigation
   - [ ] Game metadata display
   - [ ] Screenshot thumbnails (optional)

2. **Settings Menu**
   - [ ] Video options (scaling, filters)
   - [ ] Audio options (enable/disable, volume)
   - [ ] Input configuration
   - [ ] Emulation settings

3. **In-Game Menu**
   - [ ] Quick access via controller combo
   - [ ] Save/load states
   - [ ] Reset game
   - [ ] Return to menu

4. **UI Polish**
   - [ ] Controller navigation
   - [ ] Visual design
   - [ ] Loading screens
   - [ ] Error messages

**Deliverable:** Complete menu-driven experience

**Estimated Effort:** 3-4 weeks

---

### Phase 5: Audio Implementation

**Goal:** Add audio emulation and output.

**Tasks:**

1. **Audio Core**
   - [ ] Integrate MAME2003 audio subsystem
   - [ ] GameCube audio output via AUDIO library
   - [ ] Audio buffer management
   - [ ] Sample rate conversion

2. **Audio Options**
   - [ ] Enable/disable audio
   - [ ] Volume control
   - [ ] Audio sync options

**Deliverable:** Games with working audio

**Estimated Effort:** 2-3 weeks

---

### Phase 6: Input System

**Goal:** Comprehensive controller mapping and support.

**Tasks:**

1. **Input Mapping**
   - [ ] GameCube controller to arcade mapping
   - [ ] Configurable button layouts
   - [ ] Support for different game types (joystick, paddle, etc.)
   - [ ] Multiple controller support

2. **Special Inputs**
   - [ ] Coin/credit buttons
   - [ ] Service menu access
   - [ ] Analog stick support

**Deliverable:** Flexible input configuration

**Estimated Effort:** 2 weeks

---

### Phase 7: Performance Optimization

**Goal:** Maximize performance on GameCube hardware.

**Tasks:**

1. **CPU Optimization**
   - [ ] Profile CPU-intensive code paths
   - [ ] Optimize emulation loops
   - [ ] Assembly optimizations for critical paths
   - [ ] Use PGO for real-world game scenarios

2. **Frame Management**
   - [ ] Frame skip implementation
   - [ ] Auto frame skip based on performance
   - [ ] Frame rate target options
   - [ ] VSync options

3. **Memory Optimization**
   - [ ] Reduce memory footprint
   - [ ] Efficient ROM loading
   - [ ] Cache optimization
   - [ ] ARAM utilization

4. **Graphics Optimization**
   - [ ] GX rendering optimization
   - [ ] Efficient texture handling
   - [ ] Minimize overdraw
   - [ ] Culling and clipping

**Deliverable:** Smooth gameplay for supported games

**Estimated Effort:** 4-6 weeks

---

### Phase 8: Expanded Game Support

**Goal:** Add more arcade games and drivers.

**Tasks:**

1. **Driver Expansion**
   - [ ] Add popular 8-bit arcade games
   - [ ] Add select 16-bit games
   - [ ] Test and verify compatibility
   - [ ] Document supported games

2. **Hardware Support**
   - [ ] Additional sound chips
   - [ ] Additional graphics hardware
   - [ ] Special controller types
   - [ ] Network/link cable (future)

**Deliverable:** Library of 50+ supported games

**Estimated Effort:** Ongoing

---

### Phase 9: Polish and Release

**Goal:** Prepare for public release.

**Tasks:**

1. **Documentation**
   - [ ] User manual
   - [ ] Compatible game list
   - [ ] Troubleshooting guide
   - [ ] ROM setup instructions

2. **Testing**
   - [ ] Hardware testing on real GameCube
   - [ ] Compatibility testing
   - [ ] Performance testing
   - [ ] Bug fixes

3. **Release Preparation**
   - [ ] Version 1.0 release
   - [ ] Release notes
   - [ ] Distribution package
   - [ ] Project website/wiki

**Deliverable:** Public release v1.0

**Estimated Effort:** 2-3 weeks

---

## Long-term Goals (Post-1.0)

### Future Enhancements

- **Wii Support:** Port to Nintendo Wii with Wii Remote support
- **Netplay:** Network multiplayer via BBA (Broadband Adapter)
- **Cheats:** Cheat code support
- **Filters:** CRT filters, scanlines, etc.
- **Themes:** Customizable UI themes
- **Advanced Features:** Fast-forward, slow-motion, rewind
- **USB Support:** Load ROMs from USB storage

### Community

- Homebrew community feedback
- User contributions
- Bug reports and feature requests
- Game compatibility database

---

## Technical Constraints

### GameCube Hardware Limits

| Resource | Limit | Impact |
|----------|-------|--------|
| Main RAM | 24MB | Limits ROM size and buffers |
| ARAM | 16MB | Can store audio or textures |
| CPU | 486 MHz Gekko | Requires optimization |
| GPU | 162 MHz Flipper | Adequate for 2D |
| Storage | SD Card | Slower than disc/cartridge |

### MAME2003 Compatibility

- Focus on pre-1985 arcade games (8-bit era)
- Select 1985-1990 games (16-bit, simpler)
- Avoid complex 3D games
- Avoid games with massive ROM sets

### Development Approach

- **Iterative:** Build in small, testable increments
- **Pragmatic:** Focus on common games first
- **Community-driven:** Listen to user feedback
- **Quality:** Prioritize stability over features

---

## Success Metrics

- ✅ Successful build on CI/CD
- ⏳ 50+ games running at full speed
- ⏳ Stable save states
- ⏳ User-friendly interface
- ⏳ Positive community reception
- ⏳ Active user base

---

## Get Involved

Interested in contributing? See [CONTRIBUTING.md](.github/CONTRIBUTING.md) for guidelines.

Current priority: **Phase 2 - MAME2003 Core Integration**
