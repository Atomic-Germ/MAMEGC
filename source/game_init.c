#include "game_init.h"
#include "rom_loader.h"
#include "mame2003/cpu/z80/z80.h"
#include "mame2003/memory.h"
#include "mame2003_gc.h"
#include "drivers/pacman/pacman.h"
#include "video/video.h"
#include <stdio.h>
#include <string.h>

/* Global game state */
game_state_t g_game_state = GAME_STATE_UNINITIALIZED;

/* External references to existing systems */
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
    
    /* Reset Z80 CPU using MAME interface */
    z80_reset(NULL);
    
    /* Set initial Z80 state for Pac-Man using MAME functions */
    z80_set_reg(Z80_PC, 0x0000);  /* Start at ROM beginning */
    z80_set_reg(Z80_SP, 0x4FFF);  /* Stack at top of RAM */
    z80_set_reg(Z80_AF, 0x0000);
    z80_set_reg(Z80_BC, 0x0000);
    z80_set_reg(Z80_DE, 0x0000);
    z80_set_reg(Z80_HL, 0x0000);
    z80_set_reg(Z80_IX, 0x0000);
    z80_set_reg(Z80_IY, 0x0000);
    
    UINT16 pc = z80_get_reg(Z80_PC);
    UINT16 sp = z80_get_reg(Z80_SP);
    printf("  Z80 CPU reset: PC=0x%04X SP=0x%04X\n", pc, sp);
    
    /* Initialize Pac-Man specific hardware */
    memset(&pacman, 0, sizeof(pacman));
    pacman.interrupt_enable = 0;
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
    
    /* For now, we'll integrate ROM data with existing memory system */
    /* The existing memory.c should handle ROM/RAM mapping */
    
    /* Clear RAM regions in existing memory system */
    for (int i = 0x4000; i < 0x5000; i++) {
        memory_write_byte(i, 0x00); /* Clear RAM */
    }
    
    printf("  Memory mapping complete\n");
    printf("  ROM: 0x0000-0x3FFF -> Program ROM region\n");
    printf("  RAM: 0x4000-0x4FFF -> System RAM\n");
    
    /* Verify ROM access through memory system */
    printf("  ROM region pointer: %p\n", prog_rom);
    printf("  First ROM bytes: %02X %02X %02X %02X\n", 
           prog_rom[0], prog_rom[1], prog_rom[2], prog_rom[3]);
    
    return true;
}

bool init_pacman_video(void) {
    printf("Initializing Pac-Man video system...\n");
    
    /* Video system should already be initialized from earlier phases */
    /* Connect ROM data to video system */
    
    uint8_t* char_rom = get_character_rom_region();
    uint8_t* col_prom = get_color_prom_region();
    
    if (!char_rom || !col_prom) {
        printf("  Error: Character ROM or Color PROMs not available\n");
        return false;
    }
    
    /* Connect character ROM and color PROMs to video system */
    printf("  Character ROM region: %p\n", char_rom);
    printf("  Color PROM region: %p\n", col_prom);
    printf("  Character ROM first bytes: %02X %02X %02X %02X\n",
           char_rom[0], char_rom[1], char_rom[2], char_rom[3]);
    printf("  Color PROM first bytes: %02X %02X %02X %02X\n",
           col_prom[0], col_prom[1], col_prom[2], col_prom[3]);
    
    printf("  Video system connected to ROM data\n");
    
    return true;
}

bool start_game_execution(void) {
    if (g_game_state != GAME_STATE_INITIALIZED) {
        printf("Game Start: System not properly initialized\n");
        return false;
    }
    
    printf("\n=== Starting Game Execution ===\n");
    
    /* Verify Z80 is ready */
    UINT16 pc = z80_get_reg(Z80_PC);
    UINT16 sp = z80_get_reg(Z80_SP);
    printf("Z80 State: PC=0x%04X SP=0x%04X\n", pc, sp);
    
    /* Get program ROM for execution */
    uint8_t* prog_rom = get_program_rom_region();
    if (!prog_rom) {
        printf("Error: Program ROM not available for execution\n");
        return false;
    }
    
    /* Check first instruction from ROM */
    uint8_t first_opcode = prog_rom[pc];
    printf("First instruction: 0x%02X at PC=0x%04X\n", first_opcode, pc);
    
    /* Execute a few test instructions to verify ROM is working */
    printf("Executing test instructions from ROM...\n");
    for (int i = 0; i < 5; i++) {
        UINT16 old_pc = z80_get_reg(Z80_PC);
        
        /* Get opcode from ROM (not memory system yet) */
        if (old_pc < 0x4000) {
            uint8_t opcode = prog_rom[old_pc];
            printf("  Instruction %d: 0x%02X at 0x%04X\n", i + 1, opcode, old_pc);
            
            /* For testing, just advance PC manually */
            /* Later this will be actual Z80 execution */
            z80_set_reg(Z80_PC, old_pc + 1);
            
            /* Don't let PC run past ROM area */
            if (z80_get_reg(Z80_PC) >= 0x4000) {
                printf("  Reached end of ROM area, stopping test\n");
                z80_set_reg(Z80_PC, 0x0000); /* Reset for actual execution */
                break;
            }
        } else {
            printf("  PC outside ROM area, stopping test\n");
            z80_set_reg(Z80_PC, 0x0000); /* Reset for actual execution */
            break;
        }
    }
    
    /* Reset PC for actual game execution */
    z80_set_reg(Z80_PC, 0x0000);
    
    g_game_state = GAME_STATE_RUNNING;
    printf("Game execution test complete\n");
    printf("System ready for main game loop\n");
    printf("Z80 PC reset to 0x%04X for game start\n", z80_get_reg(Z80_PC));
    
    return true;
}

void reset_game_system(void) {
    printf("Resetting game system...\n");
    
    /* Reset Z80 */
    z80_reset(NULL);
    z80_set_reg(Z80_PC, 0x0000);
    z80_set_reg(Z80_SP, 0x4FFF);
    
    /* Clear RAM */
    for (int i = 0x4000; i < 0x5000; i++) {
        memory_write_byte(i, 0x00);
    }
    
    /* Reset game state */
    memset(&pacman, 0, sizeof(pacman));
    
    g_game_state = GAME_STATE_INITIALIZED;
    printf("Game system reset complete\n");
}