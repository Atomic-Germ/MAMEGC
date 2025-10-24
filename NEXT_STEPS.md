# NEXT STEPS: SD Card ROM Loading Implementation

## Why This Is Critical
The MAME2003 GameCube port currently has all hardware emulation working (Z80 CPU, video, input) but **cannot run any games** because it cannot load ROM files. The ROM files in `/roms/` contain the actual game code that needs to be loaded into the emulated arcade machine's memory.

## Implementation Plan

### Step 1: libfat Integration
```c
// Add to main.c
#include <fat.h>

int main() {
    // Initialize FAT filesystem for SD card
    if (!fatInitDefault()) {
        printf("Failed to initialize SD card\n");
        return -1;
    }
    
    // Continue with existing initialization...
}
```

### Step 2: ROM File Detection
```c
// New file: rom_loader.c
bool scan_for_roms(const char* rom_path) {
    DIR* dir = opendir(rom_path);
    if (!dir) return false;
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        // Look for .zip files or ROM directories
        // Check against known MAME ROM sets
    }
    closedir(dir);
    return true;
}
```

### Step 3: ROM Loading into MAME Memory
```c
// Integration with existing memory.c
bool load_rom_file(const char* filename, uint32_t address, size_t size) {
    FILE* file = fopen(filename, "rb");
    if (!file) return false;
    
    // Read ROM data into MAME memory system
    size_t read = fread(memory_region(address), 1, size, file);
    fclose(file);
    
    return read == size;
}
```

### Step 4: Game Driver Connection
```c
// Connect to existing Pac-Man driver
bool load_pacman_roms(const char* rom_directory) {
    // Load each ROM file that Pac-Man needs:
    // - pacman.6e, pacman.6f, pacman.6h, pacman.6j (program ROMs)
    // - pacman.5e (character ROM)
    // - 82s123.7f, 82s126.4a (color PROMs)
    
    if (!load_rom_file("pacman.6e", 0x0000, 0x1000)) return false;
    if (!load_rom_file("pacman.6f", 0x1000, 0x1000)) return false;
    // ... etc for all required ROMs
    
    return true;
}
```

## Required Files to Modify/Create

1. **Modify `Makefile.gc`** - Add `-lfat` linker flag
2. **Modify `main.c`** - Add FAT initialization and ROM loading calls
3. **Create `rom_loader.h/c`** - ROM file detection and loading functions
4. **Modify driver files** - Connect ROM loading to game drivers
5. **Add error handling** - Graceful failure when ROMs missing

## Testing Plan

1. Build with libfat integration
2. Test SD card detection on GameCube hardware  
3. Place Pac-Man ROM files in `/roms/pacman/` on SD card
4. Verify ROM files are detected and loaded into memory
5. Confirm game starts running (even without audio)

## Success Criteria

✅ SD card mounts successfully on GameCube  
✅ ROM files detected in `/roms/` directory  
✅ ROM data loaded into MAME memory system  
✅ Pac-Man game code begins executing  
✅ Video output shows game graphics (not just test patterns)

This will transform the emulator from a hardware test into a functional arcade game player.