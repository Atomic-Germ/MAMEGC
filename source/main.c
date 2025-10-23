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
#include "z80_test.h"

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

int main(int argc, char **argv) {
    mame2003_context_t mame_ctx;
    int result;
    
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
    
    printf("\n\n");
    printf("========================================\n");
    printf("  MAME2003 GameCube Port - v0.2.0\n");
    printf("========================================\n");
    printf("\n");
    printf("Build: %s %s\n", __DATE__, __TIME__);
    printf("\n");
    printf("Status: Phase 2 - Core Integration\n");
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
        
        /* Allocate ROM region (16KB) */
        memory_region_alloc(REGION_CPU1, 16384, "CPU1 ROM");
        
        /* Allocate RAM region (16KB) */
        memory_region_alloc(REGION_USER1, 16384, "RAM");
        
        /* Load test program into ROM */
        printf("\nLoading Z80 test program...\n");
        memory_load_rom(REGION_CPU1, z80_test_program, Z80_TEST_PROGRAM_SIZE);
        
        /* Map ROM to 0x0000-0x3FFF */
        extern void memory_map_rom(UINT32, UINT32, UINT8*);
        memory_map_rom(0x0000, 0x3FFF, memory_region_get_base(REGION_CPU1));
        
        /* Map RAM to 0x4000-0x7FFF */
        extern void memory_map_ram(UINT32, UINT32, UINT8*);
        memory_map_ram(0x4000, 0x7FFF, memory_region_get_base(REGION_USER1));
        
        printf("\n");
        
        /* Initialize Z80 CPU */
        printf("Initializing Z80 CPU...\n");
        z80_init();
        z80_reset(NULL);
        
        /* Manually clear registers to ensure clean state */
        z80_set_reg(Z80_AF, 0x0000);
        z80_set_reg(Z80_BC, 0x0000);
        z80_set_reg(Z80_DE, 0x0000);
        z80_set_reg(Z80_HL, 0x0000);
        z80_set_reg(Z80_PC, 0x0000);
        z80_set_reg(Z80_SP, 0x0000);
        
        printf("Z80 CPU initialized!\n");
        printf("Z80 PC: %04X\n", z80_get_reg(Z80_PC));
        printf("Z80 SP: %04X\n", z80_get_reg(Z80_SP));
        
        /* Dump program memory */
        printf("\nMemory dump at $0000:\n");
        memory_dump(0x0000, 16);
        
        /* Execute Z80 test program */
        printf("\n=== Z80 Execution Test ===\n");
        
        /* Get initial register values */
        UINT16 af_initial = z80_get_reg(Z80_AF);
        UINT8 a_initial = (af_initial >> 8) & 0xFF;
        
        UINT16 bc_initial = z80_get_reg(Z80_BC);
        UINT8 b_initial = (bc_initial >> 8) & 0xFF;
        
        printf("Initial: A=%02X B=%02X PC=%04X\n", 
               a_initial, b_initial, z80_get_reg(Z80_PC));
        
        /* Execute the test program */
        printf("Executing test program...\n");
        int total_cycles = z80_execute(120);
        
        /* Get final register values */
        UINT16 af_final = z80_get_reg(Z80_AF);
        UINT8 a_final = (af_final >> 8) & 0xFF;
        UINT8 f_final = af_final & 0xFF;
        
        UINT16 bc_final = z80_get_reg(Z80_BC);
        UINT8 b_final = (bc_final >> 8) & 0xFF;
        
        printf("Final:   A=%02X B=%02X PC=%04X\n", 
               a_final, b_final, z80_get_reg(Z80_PC));
        printf("Flags:   F=%02X\n", f_final);
        printf("Cycles:  %d\n", total_cycles);
        
        /* Check if test passed */
        if (a_final == 0x42) {
            printf("\n*** Z80 TEST PASSED! ***\n");
            printf("A register correctly loaded with 0x42\n");
            printf("Loop executed, B decremented from 0x10 to 0x%02X\n", b_final);
        } else {
            printf("\n*** Test failed - A=%02X (expected 0x42)\n", a_final);
        }
        
        printf("\n");
        
        /* Display memory usage */
        char stats[256];
        mame2003_get_stats(&mame_ctx, stats, sizeof(stats));
        printf("Stats:\n%s\n", stats);
    } else {
        printf("ERROR: MAME2003 initialization failed!\n");
    }
    
    printf("\n");
    printf("Press START to exit.\n");
    printf("\n");
    
    while (1) {
        PAD_ScanPads();
        int buttonsDown = PAD_ButtonsDown(0);
        
        if (buttonsDown & PAD_BUTTON_START) {
            break;
        }
        
        VIDEO_WaitVSync();
    }
    
    /* Shutdown MAME2003 */
    if (result == 0) {
        printf("\nShutting down...\n");
        z80_exit();
        printf("Z80 CPU shut down\n");
        memory_shutdown();
        printf("Memory system shut down\n");
        mame2003_shutdown(&mame_ctx);
    }
    
    printf("Goodbye!\n");
    
    return 0;
}
