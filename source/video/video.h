/***************************************************************************
 * Video System for GameCube
 * 
 * Handles tile rendering, sprites, and GX integration
 ***************************************************************************/

#ifndef VIDEO_H
#define VIDEO_H

#include "../mame2003/osd_gc.h"
#include <gctypes.h>

/***************************************************************************
 * Video Configuration
 ***************************************************************************/

#define VIDEO_WIDTH         224
#define VIDEO_HEIGHT        288
#define VIDEO_TILE_WIDTH    8
#define VIDEO_TILE_HEIGHT   8
#define VIDEO_TILES_X       28  /* 224 / 8 */
#define VIDEO_TILES_Y       36  /* 288 / 8 */

/***************************************************************************
 * Color Palette
 ***************************************************************************/

typedef struct {
    u8 r, g, b, a;
} color_t;

#define PALETTE_SIZE 16

/***************************************************************************
 * Video State
 ***************************************************************************/

typedef struct {
    /* Framebuffer */
    u32* framebuffer;
    int fb_width;
    int fb_height;
    
    /* Palette */
    color_t palette[PALETTE_SIZE];
    
    /* Tile graphics data */
    u8* tile_gfx;
    int tile_count;
    
    /* Statistics */
    u32 frame_count;
    
} video_state_t;

/***************************************************************************
 * Video Functions
 ***************************************************************************/

/* Initialization */
int video_init(video_state_t* state, u32* framebuffer, int width, int height);
void video_shutdown(video_state_t* state);

/* Palette */
void video_set_palette(video_state_t* state, const color_t* palette);
void video_set_default_palette(video_state_t* state);

/* Tile rendering */
void video_render_tiles(video_state_t* state, 
                       const u8* vram, 
                       const u8* cram,
                       int flip_screen);

/* Frame management */
void video_begin_frame(video_state_t* state);
void video_end_frame(video_state_t* state);
void video_clear(video_state_t* state, color_t color);

#endif /* VIDEO_H */
