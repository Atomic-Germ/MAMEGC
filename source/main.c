#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

/* Global Pac-Man state (for CPU interface) */
void* g_pacman_state = NULL;
static pacman_state_t pacman;

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
    printf("MAME2003 GameCube Port - v0.3.0\n");
    printf("================================\n");
    printf("Build: %s %s\n", __DATE__, __TIME__);
    printf("Status: Phase 3 - Pac-Man Driver\n");
    printf("\n");
    
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
            
            /* Run for a few frames */
            printf("\nRunning Pac-Man for 3 frames...\n");
            for (int i = 0; i < 3; i++) {
                printf("Frame %d: ", i + 1);
                pacman_run_frame(&pacman);
                printf("PC=%04X ", z80_get_reg(Z80_PC));
                printf("SP=%04X ", z80_get_reg(Z80_SP));
                printf("A=%02X\n", (z80_get_reg(Z80_AF) >> 8) & 0xFF);
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
        PAD_ScanPads();
        int buttonsDown = PAD_ButtonsDown(0);
        
        if (buttonsDown & PAD_BUTTON_START) {
            break;
        }
        
        VIDEO_WaitVSync();
    }
    
    /* Shutdown MAME2003 */
    if (result == 0 && g_pacman_state) {
        printf("\nShutting down...\n");
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
