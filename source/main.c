#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gccore.h>
#include <ogc/video.h>
#include <ogc/video_types.h>
#include <ogc/consol.h>
#include <ogc/pad.h>
#include "mame2003_gc.h"

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
        printf("\nShutting down MAME2003...\n");
        mame2003_shutdown(&mame_ctx);
    }
    
    printf("Goodbye!\n");
    
    return 0;
}
