/***************************************************************************
 * Video System Implementation
 ***************************************************************************/

#include "video.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

/***************************************************************************
 * Pac-Man Default Palette
 * 
 * Colors from original Pac-Man arcade hardware
 ***************************************************************************/

static const color_t pacman_palette[PALETTE_SIZE] = {
    {0x00, 0x00, 0x00, 0xFF},  /* 0: Black */
    {0xFF, 0x00, 0x00, 0xFF},  /* 1: Red */
    {0xFF, 0xB8, 0x97, 0xFF},  /* 2: Pink */
    {0xFF, 0xB8, 0xFF, 0xFF},  /* 3: Light pink */
    {0x00, 0xFF, 0xFF, 0xFF},  /* 4: Cyan */
    {0xFF, 0xB8, 0x00, 0xFF},  /* 5: Orange */
    {0x51, 0xFF, 0xFF, 0xFF},  /* 6: Light cyan */
    {0xFF, 0xFF, 0x00, 0xFF},  /* 7: Yellow */
    {0xFF, 0xB8, 0x97, 0xFF},  /* 8: Peach */
    {0xFF, 0xB8, 0x00, 0xFF},  /* 9: Orange */
    {0xFF, 0xFF, 0xFF, 0xFF},  /* A: White */
    {0x00, 0x00, 0xFF, 0xFF},  /* B: Blue */
    {0x00, 0xFF, 0x00, 0xFF},  /* C: Green */
    {0xFF, 0xFF, 0xB8, 0xFF},  /* D: Light yellow */
    {0xFF, 0x00, 0xFF, 0xFF},  /* E: Magenta */
    {0xFF, 0xB8, 0x51, 0xFF},  /* F: Light orange */
};

/***************************************************************************
 * Simple 8x8 Tile Graphics (Test Pattern)
 * 
 * For now, we'll generate test patterns for each tile
 * Later we'll load actual Pac-Man tile graphics
 ***************************************************************************/

static void generate_test_tile(u8* tile_data, u8 tile_index) {
    /* Create a test pattern based on tile index */
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            int pixel_idx = y * 8 + x;
            
            /* Create a checkerboard pattern with tile index influence */
            if ((x + y + tile_index) & 1) {
                tile_data[pixel_idx] = (tile_index & 0x0F);  /* Use low nibble as color */
            } else {
                tile_data[pixel_idx] = 0;  /* Black */
            }
        }
    }
}

/***************************************************************************
 * Initialization
 ***************************************************************************/

int video_init(video_state_t* state, u32* framebuffer, int width, int height) {
    printf("Initializing video system...\n");
    
    memset(state, 0, sizeof(video_state_t));
    
    state->framebuffer = framebuffer;
    state->fb_width = width;
    state->fb_height = height;
    state->frame_count = 0;
    
    /* Set default palette */
    video_set_default_palette(state);
    
    /* Allocate tile graphics (256 tiles × 64 bytes each) */
    state->tile_gfx = malloc(256 * 64);
    if (!state->tile_gfx) {
        printf("ERROR: Failed to allocate tile graphics\n");
        return -1;
    }
    
    /* Generate test tiles */
    printf("Generating test tile graphics...\n");
    for (int i = 0; i < 256; i++) {
        generate_test_tile(state->tile_gfx + (i * 64), i);
    }
    state->tile_count = 256;
    
    printf("Video system initialized\n");
    printf("  Framebuffer: %p (%dx%d)\n", framebuffer, width, height);
    printf("  Tiles: %d\n", state->tile_count);
    
    return 0;
}

void video_shutdown(video_state_t* state) {
    printf("Shutting down video system...\n");
    
    if (state->tile_gfx) {
        free(state->tile_gfx);
        state->tile_gfx = NULL;
    }
    
    memset(state, 0, sizeof(video_state_t));
}

/***************************************************************************
 * Palette
 ***************************************************************************/

void video_set_palette(video_state_t* state, const color_t* palette) {
    memcpy(state->palette, palette, sizeof(state->palette));
}

void video_set_default_palette(video_state_t* state) {
    video_set_palette(state, pacman_palette);
}

/***************************************************************************
 * Frame Management
 ***************************************************************************/

void video_begin_frame(video_state_t* state) {
    /* Nothing needed yet */
}

void video_end_frame(video_state_t* state) {
    state->frame_count++;
}

void video_clear(video_state_t* state, color_t color) {
    /* Convert color to 32-bit RGBA */
    u32 pixel = (color.r << 24) | (color.g << 16) | (color.b << 8) | color.a;
    
    /* Fill framebuffer */
    for (int i = 0; i < state->fb_width * state->fb_height; i++) {
        state->framebuffer[i] = pixel;
    }
}

/***************************************************************************
 * Tile Rendering
 ***************************************************************************/

static inline void put_pixel(u32* fb, int fb_width, int x, int y, color_t color) {
    if (x >= 0 && y >= 0 && x < fb_width && y < 480) {  /* Assuming max 480 height */
        u32 pixel = (color.r << 24) | (color.g << 16) | (color.b << 8) | color.a;
        fb[y * fb_width + x] = pixel;
    }
}

void video_render_tiles(video_state_t* state, 
                       const u8* vram, 
                       const u8* cram,
                       int flip_screen) {
    /* Render all tiles from VRAM */
    
    /* Calculate centering offset to center 224x288 in framebuffer */
    int x_offset = (state->fb_width - VIDEO_WIDTH) / 2;
    int y_offset = (state->fb_height - VIDEO_HEIGHT) / 2;
    
    for (int ty = 0; ty < VIDEO_TILES_Y; ty++) {
        for (int tx = 0; tx < VIDEO_TILES_X; tx++) {
            /* Pac-Man video RAM layout is column-major */
            int vram_idx = tx * VIDEO_TILES_Y + ty;
            
            /* Get tile index and color from VRAM/CRAM */
            u8 tile_idx = vram[vram_idx];
            u8 color_code = cram[vram_idx];
            
            /* Get tile graphics */
            u8* tile_gfx = state->tile_gfx + (tile_idx * 64);
            
            /* Render 8x8 tile */
            for (int py = 0; py < 8; py++) {
                for (int px = 0; px < 8; px++) {
                    /* Get pixel color index from tile */
                    u8 color_idx = tile_gfx[py * 8 + px];
                    
                    /* If pixel is transparent (0), skip it */
                    if (color_idx == 0) continue;
                    
                    /* Combine with color code (use color_code as palette offset) */
                    color_idx = (color_code & 0x0F);
                    
                    /* Get RGB color from palette */
                    color_t color = state->palette[color_idx];
                    
                    /* Calculate screen position */
                    int screen_x = x_offset + tx * 8 + px;
                    int screen_y = y_offset + ty * 8 + py;
                    
                    /* Put pixel */
                    put_pixel(state->framebuffer, state->fb_width, 
                             screen_x, screen_y, color);
                }
            }
        }
    }
}
