#include <gccore.h>
#include <ogcsys.h>
#include <stdio.h>
#include "fileop.h"

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

static void fs_demo()
{
    const char* base = fs_app_base_path();
    if (!base) {
        printf("No FAT device found.\n");
        return;
    }

    char p[512];
    // Ensure base subfolders
    fs_path_join(p, sizeof(p), base, "config"); fs_mkdir_p(p);
    fs_path_join(p, sizeof(p), base, "roms");   fs_mkdir_p(p);
    fs_path_join(p, sizeof(p), base, "saves");  fs_mkdir_p(p);

    // List ROMs folder (print up to 10 entries)
    fs_dir_entry ents[64];
    fs_path_join(p, sizeof(p), base, "roms");
    int n = fs_list_dir(p, ents, (int)(sizeof(ents)/sizeof(ents[0])));
    printf("ROMs in %s: %d\n", p, n);
    int max_show = n < 10 ? n : 10;
    for (int i = 0; i < max_show; ++i) {
        printf(" %c %s\n", ents[i].is_dir ? 'd' : '-', ents[i].name);
    }
}

int main(int argc, char** argv)
{
    init_video();
    PAD_Init();

    // Mount available FAT devices (carda:/, cardb:/, port2:/)
    fs_mount_all_fat();

    printf("mameGC skeleton\n");
    printf("- Conventions modeled after snes9xGC\n");
    const char* base = fs_app_base_path();
    printf("- Base path: %s\n", base ? base : "<none>");
    fs_demo();
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
