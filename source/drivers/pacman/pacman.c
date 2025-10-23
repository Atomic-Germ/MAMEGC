/***************************************************************************
 * Pac-Man Driver Implementation
 ***************************************************************************/

#include "pacman.h"
#include "../../mame2003/memory.h"
#include "../../mame2003/cpu/z80/z80.h"
#include <string.h>
#include <stdio.h>

/***************************************************************************
 * Initialization
 ***************************************************************************/

int pacman_init(pacman_state_t* state) {
    printf("Initializing Pac-Man driver...\n");
    
    /* Clear state */
    memset(state, 0, sizeof(pacman_state_t));
    
    /* Allocate ROM region */
    if (memory_region_alloc(REGION_CPU1, PACMAN_ROM_SIZE, "Pac-Man ROM") != 0) {
        printf("ERROR: Failed to allocate ROM\n");
        return -1;
    }
    state->rom = memory_region_get_base(REGION_CPU1);
    
    /* Allocate Video RAM */
    if (memory_region_alloc(REGION_USER1, PACMAN_VIDEO_RAM_SIZE, "Video RAM") != 0) {
        printf("ERROR: Failed to allocate VRAM\n");
        return -1;
    }
    state->video_ram = memory_region_get_base(REGION_USER1);
    
    /* Allocate Color RAM */
    if (memory_region_alloc(REGION_USER1 + 1, PACMAN_COLOR_RAM_SIZE, "Color RAM") != 0) {
        printf("ERROR: Failed to allocate CRAM\n");
        return -1;
    }
    state->color_ram = memory_region_get_base(REGION_USER1 + 1);
    
    /* Allocate General RAM */
    if (memory_region_alloc(REGION_USER1 + 2, PACMAN_RAM_SIZE, "RAM") != 0) {
        printf("ERROR: Failed to allocate RAM\n");
        return -1;
    }
    state->ram = memory_region_get_base(REGION_USER1 + 2);
    
    printf("Pac-Man memory allocated:\n");
    printf("  ROM:   %p (%u bytes)\n", state->rom, PACMAN_ROM_SIZE);
    printf("  VRAM:  %p (%u bytes)\n", state->video_ram, PACMAN_VIDEO_RAM_SIZE);
    printf("  CRAM:  %p (%u bytes)\n", state->color_ram, PACMAN_COLOR_RAM_SIZE);
    printf("  RAM:   %p (%u bytes)\n", state->ram, PACMAN_RAM_SIZE);
    
    /* Initialize default state */
    state->interrupt_enable = 0;
    state->flip_screen = 0;
    state->frame_count = 0;
    
    /* Default input (no buttons pressed) */
    state->input_port0 = 0xFF;
    state->input_port1 = 0xFF;
    state->dip_switch1 = 0x00;
    state->dip_switch2 = 0x00;
    
    printf("Pac-Man driver initialized\n");
    return 0;
}

void pacman_shutdown(pacman_state_t* state) {
    printf("Shutting down Pac-Man driver...\n");
    
    /* Free memory regions */
    memory_region_free(REGION_CPU1);
    memory_region_free(REGION_USER1);
    memory_region_free(REGION_USER1 + 1);
    memory_region_free(REGION_USER1 + 2);
    
    memset(state, 0, sizeof(pacman_state_t));
}

void pacman_reset(pacman_state_t* state) {
    printf("Resetting Pac-Man...\n");
    
    /* Clear RAM */
    memset(state->video_ram, 0, PACMAN_VIDEO_RAM_SIZE);
    memset(state->color_ram, 0, PACMAN_COLOR_RAM_SIZE);
    memset(state->ram, 0, PACMAN_RAM_SIZE);
    
    /* Reset Z80 */
    z80_reset(NULL);
    
    /* Reset state */
    state->interrupt_enable = 0;
    state->flip_screen = 0;
    state->frame_count = 0;
}

/***************************************************************************
 * Memory Access
 ***************************************************************************/

UINT8 pacman_read_byte(pacman_state_t* state, UINT16 address) {
    /* ROM */
    if (address <= PACMAN_ROM_END) {
        return state->rom[address];
    }
    
    /* Video RAM */
    if (address >= PACMAN_VRAM_BASE && address <= PACMAN_VRAM_END) {
        return state->video_ram[address - PACMAN_VRAM_BASE];
    }
    
    /* Color RAM */
    if (address >= PACMAN_CRAM_BASE && address <= PACMAN_CRAM_END) {
        return state->color_ram[address - PACMAN_CRAM_BASE];
    }
    
    /* General RAM */
    if (address >= PACMAN_RAM_BASE && address <= PACMAN_RAM_END) {
        return state->ram[address - PACMAN_RAM_BASE];
    }
    
    /* Input ports */
    if (address == 0x5000) {
        return state->input_port0;
    }
    if (address == 0x5040) {
        return state->input_port1;
    }
    if (address == 0x5080) {
        return state->dip_switch1;
    }
    if (address == 0x50C0) {
        return state->dip_switch2;
    }
    
    /* Unmapped */
    return 0xFF;
}

void pacman_write_byte(pacman_state_t* state, UINT16 address, UINT8 data) {
    /* ROM is read-only */
    if (address <= PACMAN_ROM_END) {
        return;
    }
    
    /* Video RAM */
    if (address >= PACMAN_VRAM_BASE && address <= PACMAN_VRAM_END) {
        state->video_ram[address - PACMAN_VRAM_BASE] = data;
        return;
    }
    
    /* Color RAM */
    if (address >= PACMAN_CRAM_BASE && address <= PACMAN_CRAM_END) {
        state->color_ram[address - PACMAN_CRAM_BASE] = data;
        return;
    }
    
    /* General RAM */
    if (address >= PACMAN_RAM_BASE && address <= PACMAN_RAM_END) {
        state->ram[address - PACMAN_RAM_BASE] = data;
        return;
    }
    
    /* Control registers */
    if (address == 0x5000) {
        state->interrupt_enable = data & 1;
        return;
    }
    if (address == 0x5003) {
        state->flip_screen = data & 1;
        return;
    }
    
    /* Sprite coordinates */
    if (address >= 0x5060 && address <= 0x506F) {
        state->sprite_coords[address - 0x5060] = data;
        return;
    }
    
    /* Other I/O - ignore for now */
}

/***************************************************************************
 * Execution
 ***************************************************************************/

void pacman_run_frame(pacman_state_t* state) {
    /* Run Z80 for one frame (60 Hz)
       Z80 @ 3.072 MHz, 60 Hz = 51,200 cycles per frame */
    const int CYCLES_PER_FRAME = 51200;
    
    /* Execute Z80 */
    z80_execute(CYCLES_PER_FRAME);
    
    /* Generate V-blank interrupt if enabled */
    if (state->interrupt_enable) {
        z80_set_irq_line(0, ASSERT_LINE);
        z80_set_irq_line(0, CLEAR_LINE);
    }
    
    state->frame_count++;
}

/***************************************************************************
 * Input
 ***************************************************************************/

void pacman_update_input(pacman_state_t* state, UINT8 port0, UINT8 port1) {
    state->input_port0 = port0;
    state->input_port1 = port1;
}

/***************************************************************************
 * Video (stub for now)
 ***************************************************************************/

void pacman_render(pacman_state_t* state) {
    /* TODO: Render tiles and sprites to framebuffer */
    /* This will be implemented in the video system */
}
