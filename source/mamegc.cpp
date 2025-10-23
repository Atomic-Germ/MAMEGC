#include <gccore.h>
#include <ogcsys.h>
#include <stdio.h>

static void* xfb = nullptr;
static GXRModeObj* rmode = nullptr;

static void init_video()
{
    VIDEO_Init();
    rmode = VIDEO_GetPreferredMode(NULL);
    xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
    console_init(xfb, 20, 20, rmode->fbWidth, rmode->xfbHeight, rmode->fbWidth * VI_DISPLAY_PIX_SZ);

    VIDEO_Configure(rmode);
    VIDEO_SetNextFramebuffer(xfb);
    VIDEO_SetBlack(FALSE);
    VIDEO_Flush();
    VIDEO_WaitVSync();
    if (rmode->viTVMode & VI_NON_INTERLACE)
        VIDEO_WaitVSync();
}

int main(int argc, char** argv)
{
    init_video();
    PAD_Init();

    printf("mameGC skeleton\n");
    printf("- Conventions modeled after snes9xGC\n");
    printf("- Press START to exit.\n\n");

    while (1)
    {
        PAD_ScanPads();
        u32 down = PAD_ButtonsDown(0);
        if (down & PAD_BUTTON_START)
            break;
        VIDEO_WaitVSync();
    }
    return 0;
}
