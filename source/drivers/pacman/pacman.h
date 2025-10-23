/***************************************************************************
 * Pac-Man Driver for GameCube
 * 
 * Based on MAME2003 Pac-Man driver
 * Simplified for GameCube port
 ***************************************************************************/

#ifndef PACMAN_H
#define PACMAN_H

#include "../../mame2003/osd_gc.h"

/***************************************************************************
 * Pac-Man Hardware Specifications
 ***************************************************************************/

/* Display */
#define PACMAN_SCREEN_WIDTH     224
#define PACMAN_SCREEN_HEIGHT    288
#define PACMAN_TILE_WIDTH       8
#define PACMAN_TILE_HEIGHT      8
#define PACMAN_TILES_X          28  /* 224 / 8 */
#define PACMAN_TILES_Y          36  /* 288 / 8 */

/* Memory regions */
#define PACMAN_ROM_SIZE         0x4000  /* 16KB */
#define PACMAN_VIDEO_RAM_SIZE   0x0400  /* 1KB */
#define PACMAN_COLOR_RAM_SIZE   0x0400  /* 1KB */
#define PACMAN_RAM_SIZE         0x1000  /* 4KB total RAM */

/* Memory map */
#define PACMAN_ROM_BASE         0x0000
#define PACMAN_ROM_END          0x3FFF
#define PACMAN_VRAM_BASE        0x4000
#define PACMAN_VRAM_END         0x43FF
#define PACMAN_CRAM_BASE        0x4400
#define PACMAN_CRAM_END         0x47FF
#define PACMAN_RAM_BASE         0x4800
#define PACMAN_RAM_END          0x4FFF
#define PACMAN_SPRITE_BASE      0x5000
#define PACMAN_SPRITE_END       0x503F
#define PACMAN_IO_BASE          0x5040
#define PACMAN_IO_END           0x507F

/* Colors */
#define PACMAN_NUM_COLORS       16

/* Sprites */
#define PACMAN_NUM_SPRITES      8
#define PACMAN_SPRITE_WIDTH     16
#define PACMAN_SPRITE_HEIGHT    16

/***************************************************************************
 * Pac-Man State
 ***************************************************************************/

typedef struct {
    /* Memory regions */
    UINT8* rom;
    UINT8* video_ram;
    UINT8* color_ram;
    UINT8* ram;
    
    /* Video state */
    UINT8 flip_screen;
    UINT8 interrupt_enable;
    
    /* Sprite data */
    UINT8 sprite_coords[PACMAN_NUM_SPRITES * 2];  /* x,y pairs */
    
    /* Frame counter */
    UINT32 frame_count;
    
    /* Input state */
    UINT8 input_port0;
    UINT8 input_port1;
    UINT8 dip_switch1;
    UINT8 dip_switch2;
    
} pacman_state_t;

/***************************************************************************
 * Pac-Man Functions
 ***************************************************************************/

/* Initialization */
int pacman_init(pacman_state_t* state);
void pacman_shutdown(pacman_state_t* state);
void pacman_reset(pacman_state_t* state);

/* Execution */
void pacman_run_frame(pacman_state_t* state);

/* Memory handlers */
UINT8 pacman_read_byte(pacman_state_t* state, UINT16 address);
void pacman_write_byte(pacman_state_t* state, UINT16 address, UINT8 data);

/* Input */
void pacman_update_input(pacman_state_t* state, UINT8 port0, UINT8 port1);

/* Video */
void pacman_render(pacman_state_t* state);

#endif /* PACMAN_H */
