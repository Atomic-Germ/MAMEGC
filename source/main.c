#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gccore.h>
#include <ogc/video.h>
#include <ogc/video_types.h>
#include <ogc/consol.h>
#include <ogc/pad.h>

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

int main(int argc, char **argv) {
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
    printf("  MAME2003 GameCube Port - v0.1.0\n");
    printf("========================================\n");
    printf("\n");
    printf("Build: %s %s\n", __DATE__, __TIME__);
    printf("\n");
    printf("Minimal scaffold for porting MAME2003\n");
    printf("to the Nintendo GameCube.\n");
    printf("\n");
    printf("Status: Phase 1 Foundation Complete\n");
    printf("Next:   MAME2003 Core Integration\n");
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
    
    return 0;
}
