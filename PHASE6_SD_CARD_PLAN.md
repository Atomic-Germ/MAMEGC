# Phase 6: SD Card ROM Loading Implementation

## Understanding GameCube Storage
- **Swiss**: Multi-purpose loader that mounts SD cards for homebrew
- **SD Gecko/SD2SP2**: Hardware adapters in memory card slots A/B
- **Mount Points**: `carda:/` (Slot A), `cardb:/` (Slot B) when using SD adapters
- **libfat**: Already integrated in Makefile.gc (`-lfat`)

## Current State Analysis
✅ libfat already linked in Makefile.gc  
✅ Basic FAT init in osd_gc.c (but wrong mount point)  
✅ ROMs exist in `/roms/pacman/` directory  
❌ Uses `sd:/` instead of `carda:/cardb:/`  
❌ No actual ROM file loading implementation  
❌ No connection to MAME memory system  

---

## Phase 6A: SD Card Detection & Mounting (Week 1)

### Goal: Detect and mount SD cards from either slot

### Tasks:
1. **Fix FAT initialization in osd_gc.c**
   ```c
   // Replace fatInitDefault() with proper slot detection
   bool sd_available = false;
   if (fatMountSimple("carda", 0)) {
       printf("SD card detected in Slot A\n");
       sd_available = true;
   } else if (fatMountSimple("cardb", 1)) {
       printf("SD card detected in Slot B\n");
       sd_available = true;
   } else {
       printf("No SD card detected\n");
   }
   ```

2. **Add SD card status display**
   - Show mount status on screen during boot
   - Display available storage space
   - Handle graceful fallback when no SD card

3. **Test file system access**
   - Create simple file listing function
   - Test basic file read operations
   - Verify directory traversal works

### Deliverable: SD card mounts successfully, file system accessible

---

## Phase 6B: ROM File Detection (Week 2)

### Goal: Scan SD card for MAME ROM files and directories

### Tasks:
1. **Create rom_scanner.c/h**
   ```c
   typedef struct {
       char name[64];
       char path[256];
       bool valid;
   } rom_set_t;
   
   bool scan_for_roms(const char* base_path, rom_set_t* roms, int max_roms);
   ```

2. **Implement ROM directory scanning**
   - Check `/roms/` directory on SD card
   - Look for subdirectories (e.g., `/roms/pacman/`)
   - Detect .zip files and ROM file collections
   - Validate basic file structure

3. **Add ROM set validation**
   - Check for required files (pacman.6e, pacman.6f, etc.)
   - Verify file sizes match expected values
   - Display found ROM sets to user

### Deliverable: ROM detection working, shows available games

---

## Phase 6C: ROM Loading Infrastructure (Week 3)

### Goal: Load ROM files into MAME memory system

### Tasks:
1. **Create rom_loader.c/h**
   ```c
   bool load_rom_file(const char* filepath, uint8_t* dest, size_t size);
   bool load_pacman_roms(const char* rom_dir);
   ```

2. **Integrate with existing memory system**
   - Use existing memory.c functions
   - Map ROM data to correct memory regions
   - Handle different ROM types (program, character, color)

3. **Implement Pac-Man ROM loading**
   ```c
   // Pac-Man ROM layout (from MAME driver):
   // 0x0000-0x0FFF: pacman.6e (Program ROM 1)
   // 0x1000-0x1FFF: pacman.6f (Program ROM 2)  
   // 0x2000-0x2FFF: pacman.6h (Program ROM 3)
   // 0x3000-0x3FFF: pacman.6j (Program ROM 4)
   // Character ROM at separate address
   // Color PROMs for palette
   ```

4. **Add error handling**
   - Handle missing files gracefully
   - Verify checksums if available
   - Report loading progress

### Deliverable: ROM files load into MAME memory correctly

---

## Phase 6D: Game Initialization (Week 4)

### Goal: Connect loaded ROMs to game execution

### Tasks:
1. **Modify main.c game startup**
   - Replace embedded ROM data with loaded files
   - Initialize game driver with loaded ROMs
   - Verify memory mapping is correct

2. **Test Z80 execution with loaded ROMs**
   - Verify PC starts at correct address (0x0000)
   - Check first few instructions execute properly
   - Monitor memory access patterns

3. **Debug ROM loading issues**
   - Add memory dump utilities
   - Verify ROM data matches original
   - Check for endianness issues

### Deliverable: Pac-Man game code executes from loaded ROMs

---

## Phase 6E: Multi-Game Support Framework (Week 5)

### Goal: Support loading different games

### Tasks:
1. **Create game driver database**
   ```c
   typedef struct {
       char name[32];
       char rom_dir[64];
       rom_layout_t* layout;
       driver_init_func init;
   } game_driver_t;
   ```

2. **Implement driver selection**
   - Detect game type from ROM directory name
   - Load appropriate driver configuration
   - Initialize correct memory layout

3. **Add basic game selection menu**
   - List available ROM sets
   - Allow user to select game
   - Launch selected game

### Deliverable: Framework for multiple games ready

---

## Phase 6F: Save State Support (Week 6)

### Goal: Save/load game states to SD card

### Tasks:
1. **Implement save state structure**
   ```c
   typedef struct {
       z80_state_t cpu;
       uint8_t memory[65536];
       video_state_t video;
       uint32_t checksum;
   } save_state_t;
   ```

2. **Add save/load functions**
   - Save state to `/saves/[game]/state[N].sav`
   - Load state from file
   - Verify state integrity

3. **Integrate with input system**
   - Controller combo to save (L+R+Z?)
   - Controller combo to load
   - State slot selection

### Deliverable: Save states working

---

## Testing Strategy

### After each phase, run existing tests first:
1. **Build test**: `./docker-build.sh build gc`
2. **Hardware test**: Verify DOL boots on GameCube
3. **Input test**: Verify existing controller input works
4. **Video test**: Verify existing display output works

### Then test new functionality:
- SD card mounting and file access
- ROM detection and listing  
- ROM loading and memory verification
- Game execution with loaded ROMs

---

## Success Criteria

### Phase 6A Complete:
✅ SD card mounts from carda: or cardb:  
✅ File system access works  
✅ Directory listing functional  

### Phase 6B Complete:
✅ ROM directories detected on SD card  
✅ ROM file validation working  
✅ Available games displayed  

### Phase 6C Complete:
✅ ROM files load into memory  
✅ Memory mapping correct  
✅ No loading errors  

### Phase 6D Complete:
✅ Pac-Man executes from loaded ROMs  
✅ Game starts properly  
✅ No crashes or hangs  

### Phase 6E Complete:
✅ Multiple ROM sets supported  
✅ Game selection working  
✅ Driver framework ready for expansion  

### Phase 6F Complete:
✅ Save states work reliably  
✅ State loading restores game properly  
✅ File corruption handling  

---

## Implementation Notes

1. **File Paths**: Use `carda:/roms/pacman/` or `cardb:/roms/pacman/`
2. **Error Handling**: Always check file operations, provide user feedback
3. **Memory Management**: Use existing MAME memory system, don't duplicate
4. **Performance**: ROM loading is one-time cost, optimize for reliability over speed
5. **Compatibility**: Test with different SD card sizes and formats

This transforms the emulator from a hardware demo into a functional game player that can load and run arcade ROMs from SD card storage.