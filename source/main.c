#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <gccore.h>
#include <ogc/video.h>
#include <ogc/video_types.h>
#include <ogc/consol.h>
#include <ogc/pad.h>
#include "mame2003_gc.h"
#include "mame2003/cpu/z80/z80.h"
#include "mame2003/memory.h"
#include "drivers/pacman/pacman.h"
#include "drivers/pacman/pacman_rom.h"
#include "video/video.h"
#include "input.h"
#include "rom_scanner.h"
#include "rom_loader.h"

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

/* Global Pac-Man state (for CPU interface) */
void* g_pacman_state = NULL;
static pacman_state_t pacman;

/* Video system */
static video_state_t video;
static u32* video_framebuffer = NULL;

/* Copy rendered framebuffer to GX framebuffer */
static void copy_to_screen(void) {
    if (!video_framebuffer || !xfb) return;
    
    /* Convert RGBA8888 to YUV422 (YUY2) for GX framebuffer */
    u32* dst = (u32*)xfb;
    int width = rmode->fbWidth;
    int height = rmode->xfbHeight;
    
    /* YUV422 stores pairs of pixels: Y0 U Y1 V */
    for (int y = 0; y < height && y < 480; y++) {
        for (int x = 0; x < width && x < 640; x += 2) {
            /* Get two adjacent pixels */
            u32 rgba0 = video_framebuffer[y * 640 + x];
            u32 rgba1 = video_framebuffer[y * 640 + x + 1];
            
            /* Extract RGB for pixel 0 */
            u8 r0 = (rgba0 >> 24) & 0xFF;
            u8 g0 = (rgba0 >> 16) & 0xFF;
            u8 b0 = (rgba0 >> 8) & 0xFF;
            
            /* Extract RGB for pixel 1 */
            u8 r1 = (rgba1 >> 24) & 0xFF;
            u8 g1 = (rgba1 >> 16) & 0xFF;
            u8 b1 = (rgba1 >> 8) & 0xFF;
            
            /* RGB to YUV conversion (ITU-R BT.601) */
            /* Y = 0.299R + 0.587G + 0.114B */
            /* U = -0.147R - 0.289G + 0.436B + 128 */
            /* V = 0.615R - 0.515G - 0.100B + 128 */
            
            int y0 = (77 * r0 + 150 * g0 + 29 * b0) >> 8;
            int y1 = (77 * r1 + 150 * g1 + 29 * b1) >> 8;
            
            /* Average UV for both pixels */
            int r_avg = (r0 + r1) >> 1;
            int g_avg = (g0 + g1) >> 1;
            int b_avg = (b0 + b1) >> 1;
            
            int u = ((-38 * r_avg - 74 * g_avg + 112 * b_avg) >> 8) + 128;
            int v = ((112 * r_avg - 94 * g_avg - 18 * b_avg) >> 8) + 128;
            
            /* Clamp values */
            if (y0 < 0) y0 = 0; if (y0 > 255) y0 = 255;
            if (y1 < 0) y1 = 0; if (y1 > 255) y1 = 255;
            if (u < 0) u = 0; if (u > 255) u = 255;
            if (v < 0) v = 0; if (v > 255) v = 255;
            
            /* Pack as YUY2: Y0 U Y1 V */
            dst[(y * width + x) / 2] = (y0 << 24) | (u << 16) | (y1 << 8) | v;
        }
    }
    
    DCFlushRange(xfb, rmode->fbWidth * rmode->xfbHeight * 2);
}

int main(int argc, char **argv) {
    mame2003_context_t mame_ctx;
    int result = 0;
    
    /* Initialize video */
    VIDEO_Init();
    PAD_Init();
    
    rmode = VIDEO_GetPreferredMode(NULL);
    xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
    console_init(xfb, 20, 20, rmode->fbWidth, rmode->xfbHeight, rmode->fbWidth * VI_DISPLAY_PIX_SZ);
    
    VIDEO_Configure(rmode);
    VIDEO_SetNextFramebuffer(xfb);
    VIDEO_SetBlack(FALSE);
    VIDEO_Flush();
    VIDEO_WaitVSync();
    if (rmode->viTVMode & VI_NON_INTERLACE) VIDEO_WaitVSync();
    
    /* Print banner */
    printf("\n");
    printf("================================\n");
    printf("MAME2003 GameCube Port - v0.4.0\n");
    printf("================================\n");
    printf("Build: %s %s\n", __DATE__, __TIME__);
    printf("Status: Phase 4 - Input System\n");
    printf("\n");
    
    /* Initialize input system */
    printf("Initializing input system...\n");
    input_init();
    
    /* Initialize MAME2003 */
    printf("Initializing MAME2003...\n");
    result = mame2003_init(&mame_ctx);
    
    if (result == 0) {
        printf("MAME2003 initialized successfully!\n");
        printf("Version: %s\n", mame2003_get_version());
        printf("\n");
        
        /* Initialize memory system */
        printf("Initializing memory system...\n");
        memory_init();
        
        /* Initialize Pac-Man driver */
        printf("\nInitializing Pac-Man driver...\n");
        if (pacman_init(&pacman) != 0) {
            printf("ERROR: Failed to initialize Pac-Man\n");
            result = -1;
        } else {
            /* Set global state for CPU interface */
            g_pacman_state = &pacman;
            
            /* Load test ROM */
            printf("\nLoading Pac-Man test ROM...\n");
            memcpy(pacman.rom, pacman_test_rom, PACMAN_TEST_ROM_SIZE);
            printf("Loaded %u bytes\n", PACMAN_TEST_ROM_SIZE);
            
            /* Dump ROM */
            printf("\nROM dump:\n");
            for (int i = 0; i < 32; i++) {
                if (i % 16 == 0) printf("%04X: ", i);
                printf("%02X ", pacman.rom[i]);
                if (i % 16 == 15) printf("\n");
            }
            
            /* Initialize Z80 CPU */
            printf("\nInitializing Z80 CPU...\n");
            z80_init();
            z80_reset(NULL);
            
            /* Clear all registers */
            z80_set_reg(Z80_AF, 0x0000);
            z80_set_reg(Z80_BC, 0x0000);
            z80_set_reg(Z80_DE, 0x0000);
            z80_set_reg(Z80_HL, 0x0000);
            z80_set_reg(Z80_PC, 0x0000);
            z80_set_reg(Z80_SP, 0x0000);
            
            printf("Z80 initialized at PC=%04X\n", z80_get_reg(Z80_PC));
            
            /* Initialize video system */
            printf("\nInitializing video system...\n");
            
            /* Allocate framebuffer (640x480 for GameCube) */
            printf("Allocating video framebuffer...\n");
            printf("  Size: 640×480×4 = %u bytes\n", 640 * 480 * 4);
            
            video_framebuffer = (u32*)memalign(32, 640 * 480 * sizeof(u32));
            if (!video_framebuffer) {
                printf("ERROR: Failed to allocate video framebuffer\n");
                result = -1;
            } else {
                printf("  Allocated at: %p\n", video_framebuffer);
                printf("  Alignment: OK (%p & 31 = %d)\n", 
                       video_framebuffer, ((u32)video_framebuffer) & 31);
                if (video_init(&video, video_framebuffer, 640, 480) != 0) {
                    printf("ERROR: Failed to initialize video\n");
                    result = -1;
                } else {
                    printf("Video system initialized!\n");
                    
                    /* Clear screen to black */
                    color_t black = {0, 0, 0, 255};
                    video_clear(&video, black);
                    
                    printf("\nRunning Pac-Man for 3 frames...\n");
                    for (int i = 0; i < 3; i++) {
                        printf("Frame %d: ", i + 1);
                        pacman_run_frame(&pacman);
                        printf("PC=%04X ", z80_get_reg(Z80_PC));
                        printf("SP=%04X ", z80_get_reg(Z80_SP));
                        printf("A=%02X\n", (z80_get_reg(Z80_AF) >> 8) & 0xFF);
                    }
                    
                    printf("\nRendering tiles...\n");
                    printf("Framebuffer: %p\n", video_framebuffer);
                    printf("FB size: %dx%d = %d pixels\n", 640, 480, 640*480);
                    
                    /* First, draw a test pattern to verify framebuffer works */
                    printf("Drawing test rectangles...\n");
                    
                    /* Clear to black */
                    memset(video_framebuffer, 0, 640 * 480 * sizeof(u32));
                    
                    /* Draw some colored rectangles to verify rendering */
                    /* Red rectangle at top-left */
                    for (int y = 0; y < 50; y++) {
                        for (int x = 0; x < 100; x++) {
                            video_framebuffer[y * 640 + x] = 0xFF0000FF; /* Red */
                        }
                    }
                    
                    /* Green rectangle next to it */
                    for (int y = 0; y < 50; y++) {
                        for (int x = 100; x < 200; x++) {
                            video_framebuffer[y * 640 + x] = 0x00FF00FF; /* Green */
                        }
                    }
                    
                    /* Blue rectangle next to that */
                    for (int y = 0; y < 50; y++) {
                        for (int x = 200; x < 300; x++) {
                            video_framebuffer[y * 640 + x] = 0x0000FFFF; /* Blue */
                        }
                    }
                    
                    /* White rectangle */
                    for (int y = 0; y < 50; y++) {
                        for (int x = 300; x < 400; x++) {
                            video_framebuffer[y * 640 + x] = 0xFFFFFFFF; /* White */
                        }
                    }
                    
                    printf("Test rectangles drawn!\n");
                    
                    /* Now render actual tiles */
                    printf("\nRendering Pac-Man tiles...\n");
                    video_begin_frame(&video);
                    video_render_tiles(&video, pacman.video_ram, pacman.color_ram, 0);
                    video_end_frame(&video);
                    
                    printf("Rendered frame %u\n", video.frame_count);
                    printf("Tile area: 28x36 tiles = %d tiles total\n", 28*36);
                    printf("Display area: 224x288 pixels (centered at %d,%d)\n", 
                           (640-224)/2, (480-288)/2);
                    
                    /* Sample a few pixels to verify rendering */
                    printf("\nFramebuffer samples:\n");
                    for (int i = 0; i < 5; i++) {
                        u32 pixel = video_framebuffer[i];
                        printf("  Pixel %d: %08X\n", i, pixel);
                    }
                    
                    printf("\nCopying to screen...\n");
                    
                    /* Debug: Show what we're converting */
                    printf("Sample conversions:\n");
                    for (int i = 0; i < 3; i++) {
                        u32 rgba = video_framebuffer[i];
                        u8 r = (rgba >> 24) & 0xFF;
                        u8 g = (rgba >> 16) & 0xFF;
                        u8 b = (rgba >> 8) & 0xFF;
                        int y = (77 * r + 150 * g + 29 * b) >> 8;
                        int u = ((-38 * r - 74 * g + 112 * b) >> 8) + 128;
                        int v = ((112 * r - 94 * g - 18 * b) >> 8) + 128;
                        printf("  Pixel %d: RGB(%02X,%02X,%02X) -> YUV(%d,%d,%d)\n", 
                               i, r, g, b, y, u, v);
                    }
                    
                    copy_to_screen();
                    VIDEO_SetNextFramebuffer(xfb);
                    VIDEO_Flush();
                    VIDEO_WaitVSync();
                    printf("Display updated!\n");
                    
                    printf("\n*** Input Test Mode ***\n");
                    printf("Press buttons to test input\n");
                    printf("Press START twice to continue\n\n");
                    
                    /* Input test loop */
                    int start_count = 0;
                    while(start_count < 2) {
                        input_update();
                        
                        /* Check for any input */
                        int has_input = 0;
                        for (int i = 0; i < 2; i++) {
                            if (input_state[i].buttons != 0) {
                                has_input = 1;
                                break;
                            }
                        }
                        
                        if (has_input) {
                            printf("\033[2J\033[H"); /* Clear screen */
                            printf("=== Input State ===\n\n");
                            
                            for (int i = 0; i < 2; i++) {
                                if (input_state[i].buttons == 0 && 
                                    input_state[i].analog_x == 0 && 
                                    input_state[i].analog_y == 0)
                                    continue;
                                
                                printf("Player %d: ", i + 1);
                                
                                if (input_button_held(i, INPUT_UP)) printf("UP ");
                                if (input_button_held(i, INPUT_DOWN)) printf("DOWN ");
                                if (input_button_held(i, INPUT_LEFT)) printf("LEFT ");
                                if (input_button_held(i, INPUT_RIGHT)) printf("RIGHT ");
                                if (input_button_held(i, INPUT_BUTTON1)) printf("A ");
                                if (input_button_held(i, INPUT_BUTTON2)) printf("B ");
                                if (input_button_held(i, INPUT_BUTTON3)) printf("X ");
                                if (input_button_held(i, INPUT_BUTTON4)) printf("Y ");
                                if (input_button_held(i, INPUT_BUTTON5)) printf("L ");
                                if (input_button_held(i, INPUT_BUTTON6)) printf("R ");
                                if (input_button_held(i, INPUT_COIN)) printf("COIN ");
                                
                                int8_t x, y;
                                input_get_analog(i, &x, &y);
                                if (x != 0 || y != 0) {
                                    printf("Analog(%d,%d) ", x, y);
                                }
                                
                                if (input_button_held(i, INPUT_START)) {
                                    printf("START ");
                                }
                                
                                printf("\n");
                            }
                            
                            printf("\nPress START twice to continue\n");
                            printf("START count: %d/2\n", start_count);
                        }
                        
                        /* Count START presses */
                        if (input_button_pressed(0, INPUT_START)) {
                            start_count++;
                            printf("\nSTART pressed (%d/2)\n", start_count);
                        }
                        
                        VIDEO_WaitVSync();
                    }
                    
                    printf("\nInput test complete!\n");
                    
                    /* Test ROM scanning */
                    printf("\n=== ROM Scanner Test ===\n");
                    if (rom_scanner_init()) {
                        rom_set_t rom_sets[MAX_ROM_SETS];
                        int rom_count = scan_for_rom_sets(rom_sets, MAX_ROM_SETS);
                        
                        if (rom_count > 0) {
                            print_rom_sets(rom_sets, rom_count);
                            
                            /* Find Pac-Man ROM set for next phase */
                            for (int i = 0; i < rom_count; i++) {
                                if (rom_sets[i].valid && is_pacman_rom_set(rom_sets[i].directory)) {
                                    printf("Pac-Man ROM set ready at: %s\n", rom_sets[i].directory);
                                    break;
                                }
                            }
                        } else {
                            printf("No valid ROM sets found\n");
                            printf("Create directory: %s/roms/pacman/\n", 
                                   osd_sd_available() ? osd_get_sd_mount() : "[SD2GC not available]");
                            printf("Place Pac-Man ROM files in that directory\n");
                        }
                    } else {
                        printf("ROM scanner failed - no SD2GC available\n");
                    }
                    
                    /* Test ROM loading */
                    printf("\n=== ROM Loading Test ===\n");
                    if (rom_loader_init()) {
                        /* Check if we found any valid ROM sets */
                        if (rom_scanner_init()) {
                            rom_set_t rom_sets[MAX_ROM_SETS];
                            int rom_count = scan_for_rom_sets(rom_sets, MAX_ROM_SETS);
                            
                            if (rom_count > 0) {
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
                            } else {
                                printf("No ROM sets found - skipping ROM loading test\n");
                            }
                        } else {
                            printf("ROM scanner not available - skipping ROM loading test\n");
                        }
                    } else {
                        printf("ROM loader initialization failed\n");
                    }
                    
                    /* Check video RAM */
                    printf("\nVideo RAM dump (first 32 bytes):\n");
                    for (int i = 0; i < 32; i++) {
                        if (i % 16 == 0) printf("%04X: ", 0x4000 + i);
                        printf("%02X ", pacman.video_ram[i]);
                        if (i % 16 == 15) printf("\n");
                    }
                    
                    /* Check color RAM */
                    printf("\nColor RAM dump (first 16 bytes):\n");
                    for (int i = 0; i < 16; i++) {
                        if (i % 16 == 0) printf("%04X: ", 0x4400 + i);
                        printf("%02X ", pacman.color_ram[i]);
                    }
                    printf("\n");
                    
                    printf("\n*** Pac-Man Test Complete! ***\n");
                    printf("Video rendered to framebuffer\n");
                }
            }
        }
        
        /* Display memory usage */
        char stats[256];
        mame2003_get_stats(&mame_ctx, stats, sizeof(stats));
        printf("\nStats:\n%s\n", stats);
    } else {
        printf("ERROR: MAME2003 initialization failed!\n");
    }
    
    printf("\nPress START to exit.\n");
    
    /* Main loop */
    while(1) {
        input_update();
        
        if (input_button_pressed(0, INPUT_START)) {
            break;
        }
        
        VIDEO_WaitVSync();
    }
    
    /* Shutdown MAME2003 */
    if (result == 0 && g_pacman_state) {
        printf("\nShutting down...\n");
        video_shutdown(&video);
        printf("Video system shut down\n");
        if (video_framebuffer) {
            free(video_framebuffer);
            video_framebuffer = NULL;
        }
        z80_exit();
        printf("Z80 CPU shut down\n");
        pacman_shutdown(&pacman);
        printf("Pac-Man driver shut down\n");
        memory_shutdown();
        printf("Memory system shut down\n");
        mame2003_shutdown(&mame_ctx);
    }
    
    return 0;
}
