# Phase 6D Implementation: Game Initialization

## Goal
Connect loaded ROM data to Z80 CPU execution and start running the actual Pac-Man game code.

## Prerequisites
✅ Phase 6A complete: SD card mounting working  
✅ Phase 6B complete: ROM file detection working  
✅ Phase 6C complete: ROM data loaded into memory  
✅ ROM verification shows valid data  

## Implementation Tasks

### 1. Create game_init.h

**New file: `source/game_init.h`**
```c
#ifndef GAME_INIT_H
#define GAME_INIT_H

#include <stdbool.h>
#include "rom_scanner.h"

/* Game initialization functions */
bool game_init_system(void);
bool init_pacman_game(const rom_set_t* rom_set);
bool start_game_execution(void);
void reset_game_system(void);

/* Game state */
typedef enum {
    GAME_STATE_UNINITIALIZED,
    GAME_STATE_ROMS_LOADED,
    GAME_STATE_INITIALIZED,
    GAME_STATE_RUNNING,
    GAME_STATE_ERROR
} game_state_t;

extern game_state_t g_game_state;

/* Hardware initialization */
bool init_pacman_hardware(void);
bool init_pacman_memory_map(void);
bool init_pacman_video(void);

#endif /* GAME_INIT_H */
```

### 2. Create game_init.c

**New file: `source/game_init.c`**
```c
#include "game_init.h"
#include "rom_loader.h"
#include "mame2003/cpu/z80/z80.h"
#include "mame2003/memory.h"
#include "mame2003_gc.h"
#include "drivers/pacman/pacman.h"
#include <stdio.h>
#include <string.h>

/* Global game state */
game_state_t g_game_state = GAME_STATE_UNINITIALIZED;

/* External references to existing systems */
extern z80_state_t z80_state;
extern pacman_state_t pacman;

bool game_init_system(void) {
    printf("\n=== Game System Initialization ===\n");
    
    /* Check ROM loading status */
    if (!g_rom_status.program_roms_loaded || 
        !g_rom_status.character_rom_loaded || 
        !g_rom_status.color_proms_loaded) {
        printf("Game Init: ROMs not loaded\n");
        g_game_state = GAME_STATE_ERROR;
        return false;
    }
    
    g_game_state = GAME_STATE_ROMS_LOADED;
    printf("Game Init: ROM loading verified\n");
    
    return true;
}

bool init_pacman_game(const rom_set_t* rom_set) {
    if (!rom_set || !rom_set->valid) {
        printf("Game Init: Invalid ROM set\n");
        return false;
    }
    
    printf("Game Init: Initializing Pac-Man game\n");
    
    /* Initialize hardware emulation */
    if (!init_pacman_hardware()) {
        printf("Game Init: Hardware initialization failed\n");
        return false;
    }
    
    /* Initialize memory mapping */
    if (!init_pacman_memory_map()) {
        printf("Game Init: Memory mapping failed\n");
        return false;
    }
    
    /* Initialize video system */
    if (!init_pacman_video()) {
        printf("Game Init: Video initialization failed\n");
        return false;
    }
    
    g_game_state = GAME_STATE_INITIALIZED;
    printf("Game Init: Pac-Man initialization complete\n");
    
    return true;
}

bool init_pacman_hardware(void) {
    printf("Initializing Pac-Man hardware emulation...\n");
    
    /* Reset Z80 CPU state */
    z80_reset(&z80_state);
    
    /* Set initial Z80 state for Pac-Man */
    z80_state.pc = 0x0000;  /* Start at ROM beginning */
    z80_state.sp = 0x4FFF;  /* Stack at top of RAM */
    z80_state.af = 0x0000;
    z80_state.bc = 0x0000;
    z80_state.de = 0x0000;
    z80_state.hl = 0x0000;
    z80_state.ix = 0x0000;
    z80_state.iy = 0x0000;
    
    printf("  Z80 CPU reset: PC=0x%04X SP=0x%04X\n", z80_state.pc, z80_state.sp);
    
    /* Initialize Pac-Man specific hardware */
    memset(&pacman, 0, sizeof(pacman));
    pacman.interrupt_enable = 0;
    pacman.sound_enable = 0;
    pacman.flip_screen = 0;
    
    printf("  Pac-Man hardware initialized\n");
    
    return true;
}

bool init_pacman_memory_map(void) {
    printf("Setting up Pac-Man memory mapping...\n");
    
    /* Pac-Man Memory Map:
     * 0x0000-0x3FFF: ROM (Program code)
     * 0x4000-0x47FF: RAM (Video RAM)
     * 0x4800-0x4BFF: RAM (Color RAM) 
     * 0x4C00-0x4FFF: RAM (General purpose)
     * 0x5000-0x5FFF: I/O and hardware registers
     */
    
    uint8_t* prog_rom = get_program_rom_region();
    if (!prog_rom) {
        printf("  Error: Program ROM not available\n");
        return false;
    }
    
    /* Map program ROM to 0x0000-0x3FFF */
    for (int i = 0; i < 0x4000; i++) {
        memory_set_bankptr(i, prog_rom + i);
    }
    
    /* Initialize RAM regions */
    for (int i = 0x4000; i < 0x5000; i++) {
        /* RAM is handled by existing memory system */
        memory_write(i, 0x00); /* Clear RAM */
    }
    
    printf("  Memory mapping complete\n");
    printf("  ROM: 0x0000-0x3FFF -> Program ROM\n");
    printf("  RAM: 0x4000-0x4FFF -> System RAM\n");
    
    /* Verify ROM mapping */
    uint8_t test_byte = memory_read(0x0000);
    printf("  Verification: ROM[0x0000] = 0x%02X\n", test_byte);
    
    return true;
}

bool init_pacman_video(void) {
    printf("Initializing Pac-Man video system...\n");
    
    /* Video system should already be initialized from earlier phases */
    /* Just need to connect it to Pac-Man specifics */
    
    uint8_t* char_rom = get_character_rom_region();
    uint8_t* col_prom = get_color_prom_region();
    
    if (!char_rom || !col_prom) {
        printf("  Error: Character ROM or Color PROMs not available\n");
        return false;
    }
    
    /* Set up character ROM for tile rendering */
    /* This connects to existing video system */
    video_set_character_rom(char_rom);
    video_set_color_proms(col_prom);
    
    printf("  Character ROM connected to video system\n");
    printf("  Color PROMs connected to palette system\n");
    
    return true;
}

bool start_game_execution(void) {
    if (g_game_state != GAME_STATE_INITIALIZED) {
        printf("Game Start: System not properly initialized\n");
        return false;
    }
    
    printf("\n=== Starting Game Execution ===\n");
    
    /* Verify Z80 is ready */
    printf("Z80 State: PC=0x%04X SP=0x%04X\n", z80_state.pc, z80_state.sp);
    
    /* Check first instruction */
    uint8_t first_opcode = memory_read(z80_state.pc);
    printf("First instruction: 0x%02X at PC=0x%04X\n", first_opcode, z80_state.pc);
    
    /* Execute a few test instructions to verify ROM is working */
    printf("Executing test instructions...\n");
    for (int i = 0; i < 5; i++) {
        uint16_t old_pc = z80_state.pc;
        uint8_t opcode = memory_read(z80_state.pc);
        
        /* Execute one Z80 instruction */
        int cycles = z80_execute_instruction(&z80_state);
        
        printf("  Instruction %d: 0x%02X at 0x%04X -> PC=0x%04X (%d cycles)\n",
               i + 1, opcode, old_pc, z80_state.pc, cycles);
        
        /* Safety check - don't let PC run away */
        if (z80_state.pc > 0x4000) {
            printf("  Warning: PC entered RAM region, stopping test\n");
            z80_state.pc = old_pc; /* Restore PC */
            break;
        }
    }
    
    g_game_state = GAME_STATE_RUNNING;
    printf("Game execution test complete\n");
    printf("System ready for main game loop\n");
    
    return true;
}

void reset_game_system(void) {
    printf("Resetting game system...\n");
    
    /* Reset Z80 */
    z80_reset(&z80_state);
    z80_state.pc = 0x0000;
    z80_state.sp = 0x4FFF;
    
    /* Clear RAM */
    for (int i = 0x4000; i < 0x5000; i++) {
        memory_write(i, 0x00);
    }
    
    /* Reset game state */
    memset(&pacman, 0, sizeof(pacman));
    
    g_game_state = GAME_STATE_INITIALIZED;
    printf("Game system reset complete\n");
}
```

### 3. Add missing video functions

**Add to `source/video/video.h`:**
```c
/* Video ROM integration functions */
void video_set_character_rom(uint8_t* char_rom);
void video_set_color_proms(uint8_t* color_proms);
```

**Add to `source/video/video.c`:**
```c
static uint8_t* character_rom_data = NULL;
static uint8_t* color_prom_data = NULL;

void video_set_character_rom(uint8_t* char_rom) {
    character_rom_data = char_rom;
    printf("Video: Character ROM connected\n");
}

void video_set_color_proms(uint8_t* color_proms) {
    color_prom_data = color_proms;
    printf("Video: Color PROMs connected\n");
}
```

### 4. Check memory system functions

**Verify these functions exist in `source/mame2003/memory.c` or add them:**
```c
/* Memory banking function - may need to implement */
void memory_set_bankptr(uint16_t address, uint8_t* ptr) {
    /* Implementation depends on existing memory system */
    /* This maps ROM data to specific addresses */
}
```

### 5. Integrate with main.c

**Add to main.c includes:**
```c
#include "game_init.h"
```

**Add game initialization after ROM loading:**
```c
/* Test game initialization */
if (load_pacman_roms(&rom_sets[0])) { /* Assuming ROM loading succeeded */
    printf("\n=== Game Initialization Test ===\n");
    
    if (game_init_system()) {
        if (init_pacman_game(&rom_sets[0])) {
            if (start_game_execution()) {
                printf("\nPac-Man game ready!\n");
                printf("Z80 is executing ROM code\n");
                printf("Press START to continue to main game loop\n");
            } else {
                printf("Game execution failed\n");
            }
        } else {
            printf("Game initialization failed\n");
        }
    } else {
        printf("System initialization failed\n");
    }
}
```

## Testing Steps

### Build Test:
```bash
./docker-build.sh build gc
```

### Hardware Test:
1. Ensure ROM files loaded successfully from Phase 6C
2. Copy updated `mamegc-gc.dol` to SD card
3. Boot GameCube with Swiss
4. Run DOL file
5. Check console output for game initialization

### Expected Output:
```
=== Game Initialization Test ===

=== Game System Initialization ===
Game Init: ROM loading verified

Game Init: Initializing Pac-Man game
Initializing Pac-Man hardware emulation...
  Z80 CPU reset: PC=0x0000 SP=0x4FFF
  Pac-Man hardware initialized
Setting up Pac-Man memory mapping...
  Memory mapping complete
  ROM: 0x0000-0x3FFF -> Program ROM
  RAM: 0x4000-0x4FFF -> System RAM
  Verification: ROM[0x0000] = 0xC3
Initializing Pac-Man video system...
Video: Character ROM connected
Video: Color PROMs connected
  Character ROM connected to video system
  Color PROMs connected to palette system
Game Init: Pac-Man initialization complete

=== Starting Game Execution ===
Z80 State: PC=0x0000 SP=0x4FFF
First instruction: 0xC3 at PC=0x0000
Executing test instructions...
  Instruction 1: 0xC3 at 0x0000 -> PC=0x1003 (10 cycles)
  Instruction 2: 0xFB at 0x1003 -> PC=0x1004 (4 cycles)
  Instruction 3: 0x21 at 0x1004 -> PC=0x1007 (10 cycles)
  Instruction 4: 0xFF at 0x1007 -> PC=0x1008 (7 cycles)
  Instruction 5: 0x4F at 0x1008 -> PC=0x1009 (4 cycles)
Game execution test complete
System ready for main game loop

Pac-Man game ready!
Z80 is executing ROM code
Press START to continue to main game loop
```

## Success Criteria for Phase 6D:

✅ Code compiles without errors  
✅ Game system initializes successfully  
✅ Z80 CPU reset to correct state (PC=0x0000)  
✅ Memory mapping established (ROM at 0x0000-0x3FFF)  
✅ Character ROM and color PROMs connected to video  
✅ Z80 executes first instructions from ROM  
✅ Program counter advances correctly  
✅ No crashes or hangs during initialization  
✅ System ready for main game loop  

## Troubleshooting:

**If Z80 doesn't execute:**
- Check ROM data is correctly loaded
- Verify memory mapping is working
- Check Z80 state initialization

**If PC jumps to wrong address:**
- Check first instruction (should be JP instruction 0xC3)
- Verify ROM files are correct Pac-Man dumps
- Check endianness issues

**If memory mapping fails:**
- Verify memory_set_bankptr() function exists
- Check memory system initialization
- Ensure ROM regions are allocated

**If video connection fails:**
- Check video system initialization from earlier phases
- Verify character ROM and color PROM pointers are valid
- Check video function implementations

## Next Steps:
Once Phase 6D is complete and the Z80 is executing ROM code correctly, you can:
1. Proceed to Phase 6E (Multi-Game Support Framework)
2. Or integrate with the existing main game loop to start running Pac-Man
3. Test video output to see if game graphics appear

The critical milestone is Z80 executing instructions from loaded ROM files - this means the emulator is now running actual arcade game code rather than test patterns!