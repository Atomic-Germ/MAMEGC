# Phase 6C Implementation: ROM Loading Infrastructure

## Goal
Load detected ROM files into MAME memory system, building on Phase 6B's ROM detection.

## Prerequisites
✅ Phase 6A complete: SD card mounting working  
✅ Phase 6B complete: ROM file detection working  
✅ Pac-Man ROM set detected and validated  

## Implementation Tasks

### 1. Create rom_loader.h

**New file: `source/rom_loader.h`**
```c
#ifndef ROM_LOADER_H
#define ROM_LOADER_H

#include <stdbool.h>
#include <stdint.h>
#include "rom_scanner.h"

/* ROM loading functions */
bool rom_loader_init(void);
bool load_rom_file(const char* filepath, uint8_t* dest, size_t size);
bool load_pacman_roms(const rom_set_t* rom_set);
void verify_loaded_roms(void);

/* Memory region helpers */
uint8_t* get_program_rom_region(void);
uint8_t* get_character_rom_region(void);
uint8_t* get_color_prom_region(void);

/* ROM loading status */
typedef struct {
    bool program_roms_loaded;
    bool character_rom_loaded;
    bool color_proms_loaded;
    size_t total_bytes_loaded;
} rom_load_status_t;

extern rom_load_status_t g_rom_status;

#endif /* ROM_LOADER_H */
```

### 2. Create rom_loader.c

**New file: `source/rom_loader.c`**
```c
#include "rom_loader.h"
#include "mame2003/memory.h"
#include "mame2003/osd_gc.h"
#include <stdio.h>
#include <string.h>

/* Global ROM loading status */
rom_load_status_t g_rom_status = {false, false, false, 0};

/* ROM memory regions (defined in MAME memory system) */
extern uint8_t* program_rom;     /* Program ROM region */
extern uint8_t* character_rom;   /* Character ROM region */
extern uint8_t* color_prom;      /* Color PROM region */

bool rom_loader_init(void) {
    /* Clear ROM loading status */
    memset(&g_rom_status, 0, sizeof(g_rom_status));
    
    printf("ROM Loader: Initialized\n");
    return true;
}

bool load_rom_file(const char* filepath, uint8_t* dest, size_t size) {
    if (!filepath || !dest || size == 0) {
        printf("ROM Loader: Invalid parameters\n");
        return false;
    }
    
    FILE* file = fopen(filepath, "rb");
    if (!file) {
        printf("ROM Loader: Cannot open %s\n", filepath);
        return false;
    }
    
    /* Get file size */
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    if (file_size != (long)size) {
        printf("ROM Loader: Size mismatch for %s (expected %zu, got %ld)\n", 
               filepath, size, file_size);
        fclose(file);
        return false;
    }
    
    /* Read file data */
    size_t bytes_read = fread(dest, 1, size, file);
    fclose(file);
    
    if (bytes_read != size) {
        printf("ROM Loader: Read error for %s (read %zu of %zu bytes)\n",
               filepath, bytes_read, size);
        return false;
    }
    
    printf("ROM Loader: Loaded %s (%zu bytes)\n", filepath, size);
    g_rom_status.total_bytes_loaded += size;
    return true;
}

bool load_pacman_roms(const rom_set_t* rom_set) {
    if (!rom_set || !rom_set->valid) {
        printf("ROM Loader: Invalid ROM set\n");
        return false;
    }
    
    printf("ROM Loader: Loading Pac-Man ROMs from %s\n", rom_set->directory);
    
    /* Get memory regions from MAME memory system */
    uint8_t* prog_rom = get_program_rom_region();
    uint8_t* char_rom = get_character_rom_region();
    uint8_t* col_prom = get_color_prom_region();
    
    if (!prog_rom || !char_rom || !col_prom) {
        printf("ROM Loader: Memory regions not initialized\n");
        return false;
    }
    
    /* Load program ROMs (16KB total: 4 x 4KB files) */
    printf("Loading program ROMs...\n");
    for (int i = 0; i < 4; i++) {
        const rom_file_t* rom_file = &rom_set->files[i]; /* pacman.6e, 6f, 6h, 6j */
        uint8_t* dest = prog_rom + (i * 0x1000); /* 4KB chunks */
        
        if (!load_rom_file(rom_file->full_path, dest, 0x1000)) {
            printf("ROM Loader: Failed to load program ROM %d\n", i);
            return false;
        }
    }
    g_rom_status.program_roms_loaded = true;
    
    /* Load character ROM (4KB) */
    printf("Loading character ROM...\n");
    const rom_file_t* char_file = &rom_set->files[4]; /* pacman.5e */
    if (!load_rom_file(char_file->full_path, char_rom, 0x1000)) {
        printf("ROM Loader: Failed to load character ROM\n");
        return false;
    }
    g_rom_status.character_rom_loaded = true;
    
    /* Load color PROMs */
    printf("Loading color PROMs...\n");
    const rom_file_t* prom1 = &rom_set->files[5]; /* 82s123.7f (32 bytes) */
    const rom_file_t* prom2 = &rom_set->files[6]; /* 82s126.4a (256 bytes) */
    
    if (!load_rom_file(prom1->full_path, col_prom, 32)) {
        printf("ROM Loader: Failed to load color PROM 1\n");
        return false;
    }
    
    if (!load_rom_file(prom2->full_path, col_prom + 32, 256)) {
        printf("ROM Loader: Failed to load color PROM 2\n");
        return false;
    }
    g_rom_status.color_proms_loaded = true;
    
    printf("ROM Loader: All Pac-Man ROMs loaded successfully (%zu bytes total)\n",
           g_rom_status.total_bytes_loaded);
    
    /* Verify loading */
    verify_loaded_roms();
    
    return true;
}

void verify_loaded_roms(void) {
    printf("\n=== ROM Loading Verification ===\n");
    
    uint8_t* prog_rom = get_program_rom_region();
    uint8_t* char_rom = get_character_rom_region();
    uint8_t* col_prom = get_color_prom_region();
    
    if (g_rom_status.program_roms_loaded && prog_rom) {
        printf("Program ROM: LOADED\n");
        printf("  First bytes: %02X %02X %02X %02X\n", 
               prog_rom[0], prog_rom[1], prog_rom[2], prog_rom[3]);
        printf("  Reset vector: 0x%04X\n", prog_rom[0] | (prog_rom[1] << 8));
    }
    
    if (g_rom_status.character_rom_loaded && char_rom) {
        printf("Character ROM: LOADED\n");
        printf("  First bytes: %02X %02X %02X %02X\n",
               char_rom[0], char_rom[1], char_rom[2], char_rom[3]);
    }
    
    if (g_rom_status.color_proms_loaded && col_prom) {
        printf("Color PROMs: LOADED\n");
        printf("  PROM1 first bytes: %02X %02X %02X %02X\n",
               col_prom[0], col_prom[1], col_prom[2], col_prom[3]);
        printf("  PROM2 first bytes: %02X %02X %02X %02X\n",
               col_prom[32], col_prom[33], col_prom[34], col_prom[35]);
    }
    
    printf("Total ROM data loaded: %zu bytes\n", g_rom_status.total_bytes_loaded);
    printf("================================\n\n");
}

/* Memory region getters - these need to interface with existing memory system */
uint8_t* get_program_rom_region(void) {
    /* This should return pointer to MAME's program ROM region */
    /* Need to check existing memory.c implementation */
    return memory_region(REGION_CPU1); /* Assuming this exists */
}

uint8_t* get_character_rom_region(void) {
    /* This should return pointer to MAME's character ROM region */
    return memory_region(REGION_GFX1); /* Assuming this exists */
}

uint8_t* get_color_prom_region(void) {
    /* This should return pointer to MAME's color PROM region */
    return memory_region(REGION_PROMS); /* Assuming this exists */
}
```

### 3. Check existing memory system integration

**Check `source/mame2003/memory.h` for memory region definitions:**
```c
/* Look for existing memory region functions */
/* May need to adapt based on actual MAME memory system */
```

**If memory_region() doesn't exist, create wrapper in memory.c:**
```c
/* Add to memory.c if needed */
static uint8_t program_rom_data[0x4000];    /* 16KB program ROM */
static uint8_t character_rom_data[0x1000];  /* 4KB character ROM */
static uint8_t color_prom_data[0x0120];     /* 288 bytes color PROMs */

uint8_t* memory_region(int region) {
    switch (region) {
        case REGION_CPU1:   return program_rom_data;
        case REGION_GFX1:   return character_rom_data;
        case REGION_PROMS:  return color_prom_data;
        default:            return NULL;
    }
}
```

### 4. Integrate with main.c

**Add to main.c includes:**
```c
#include "rom_loader.h"
```

**Add ROM loading test after ROM scanning:**
```c
/* Test ROM loading */
if (rom_count > 0) {
    printf("\n=== ROM Loading Test ===\n");
    
    if (rom_loader_init()) {
        /* Find first valid Pac-Man ROM set */
        for (int i = 0; i < rom_count; i++) {
            if (rom_sets[i].valid && is_pacman_rom_set(rom_sets[i].directory)) {
                printf("Loading Pac-Man ROM set...\n");
                
                if (load_pacman_roms(&rom_sets[i])) {
                    printf("ROM loading: SUCCESS\n");
                    printf("Ready to start game execution\n");
                } else {
                    printf("ROM loading: FAILED\n");
                }
                break;
            }
        }
    }
} else {
    printf("No ROM sets found - skipping ROM loading test\n");
}
```

## Testing Steps

### Build Test:
```bash
./docker-build.sh build gc
```

### Hardware Test:
1. Ensure ROM files are on SD card from Phase 6B
2. Copy updated `mamegc-gc.dol` to SD card
3. Boot GameCube with Swiss
4. Run DOL file
5. Check console output for ROM loading

### Expected Output:
```
=== ROM Loading Test ===
ROM Loader: Initialized
Loading Pac-Man ROM set...
ROM Loader: Loading Pac-Man ROMs from carda:/roms/pacman/
Loading program ROMs...
ROM Loader: Loaded carda:/roms/pacman/pacman.6e (4096 bytes)
ROM Loader: Loaded carda:/roms/pacman/pacman.6f (4096 bytes)
ROM Loader: Loaded carda:/roms/pacman/pacman.6h (4096 bytes)
ROM Loader: Loaded carda:/roms/pacman/pacman.6j (4096 bytes)
Loading character ROM...
ROM Loader: Loaded carda:/roms/pacman/pacman.5e (4096 bytes)
Loading color PROMs...
ROM Loader: Loaded carda:/roms/pacman/82s123.7f (32 bytes)
ROM Loader: Loaded carda:/roms/pacman/82s126.4a (256 bytes)
ROM Loader: All Pac-Man ROMs loaded successfully (20512 bytes total)

=== ROM Loading Verification ===
Program ROM: LOADED
  First bytes: C3 00 10 FB
  Reset vector: 0x10C3
Character ROM: LOADED
  First bytes: 00 00 00 00
Color PROMs: LOADED
  PROM1 first bytes: 0F 1F 2F 3F
  PROM2 first bytes: 01 02 03 04
Total ROM data loaded: 20512 bytes
================================

ROM loading: SUCCESS
Ready to start game execution
```

## Success Criteria for Phase 6C:

✅ Code compiles without errors  
✅ ROM loader initializes successfully  
✅ All 7 ROM files load without errors  
✅ ROM data mapped to correct memory regions  
✅ Memory verification shows valid data  
✅ Total bytes loaded = 20,512 bytes  
✅ Reset vector points to valid address  
✅ Existing functionality still works  

## Troubleshooting:

**If ROM loading fails:**
- Check file permissions on SD card
- Verify ROM files are not corrupted
- Check memory regions are properly allocated

**If memory regions not found:**
- Check existing memory.c implementation
- May need to adapt memory_region() function
- Ensure MAME memory system is initialized

**If verification shows wrong data:**
- Check endianness issues
- Verify ROM files are correct dumps
- Check for file corruption

## Next Phase:
Once Phase 6C is complete and ROM data is loaded into memory, proceed to Phase 6D (Game Initialization).