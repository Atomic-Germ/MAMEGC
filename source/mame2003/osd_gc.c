/***************************************************************************
 * GameCube OSD (Operating System Dependent) Layer - Implementation
 ***************************************************************************/

#include "osd_gc.h"
#include <gccore.h>
#include <fat.h>
#include <malloc.h>
#include <unistd.h>
#include <ogc/lwp_watchdog.h>

/***************************************************************************
 * Memory Management
 ***************************************************************************/

static size_t total_allocated = 0;
static size_t peak_allocated = 0;

void* osd_malloc(size_t size) {
    void* ptr = malloc(size);
    if (ptr) {
        total_allocated += size;
        if (total_allocated > peak_allocated) {
            peak_allocated = total_allocated;
        }
    }
    return ptr;
}

void* osd_calloc(size_t count, size_t size) {
    void* ptr = calloc(count, size);
    if (ptr) {
        total_allocated += (count * size);
        if (total_allocated > peak_allocated) {
            peak_allocated = total_allocated;
        }
    }
    return ptr;
}

void osd_free(void* ptr) {
    if (ptr) {
        free(ptr);
    }
}

size_t osd_get_memory_usage(void) {
    return total_allocated;
}

size_t osd_get_peak_memory_usage(void) {
    return peak_allocated;
}

/***************************************************************************
 * Timing
 ***************************************************************************/

static uint64_t start_ticks = 0;

UINT32 osd_ticks(void) {
    uint64_t now = gettime();
    uint64_t diff_ticks = diff_ticks(start_ticks, now);
    return (UINT32)ticks_to_millisecs(diff_ticks);
}

UINT32 osd_ticks_per_second(void) {
    return 1000; /* milliseconds */
}

void osd_sleep(int milliseconds) {
    if (milliseconds > 0) {
        usleep((useconds_t)(milliseconds * 1000));
    }
}

/***************************************************************************
 * Video (Stub for now)
 ***************************************************************************/

static void* video_buffer = NULL;
static int video_width = 0;
static int video_height = 0;

int osd_create_display(int width, int height, int depth) {
    video_width = width;
    video_height = height;
    
    /* Allocate framebuffer */
    size_t buffer_size = width * height * 4; /* RGBA32 */
    video_buffer = memalign(32, buffer_size);
    
    if (!video_buffer) {
        return -1;
    }
    
    memset(video_buffer, 0, buffer_size);
    return 0;
}

void osd_close_display(void) {
    if (video_buffer) {
        free(video_buffer);
        video_buffer = NULL;
    }
}

void osd_update_video(void) {
    /* Will implement GX rendering later */
}

/***************************************************************************
 * Input (Stub for now)
 ***************************************************************************/

static u16 pad_buttons = 0;

void osd_poll_inputs(void) {
    PAD_ScanPads();
    pad_buttons = PAD_ButtonsHeld(0);
}

int osd_key_pressed(int keycode) {
    return (pad_buttons & keycode) != 0;
}

/***************************************************************************
 * File I/O (Stub for now - will use libfat)
 ***************************************************************************/

osd_file osd_fopen(const char* path, const char* mode) {
    return (osd_file)fopen(path, mode);
}

int osd_fread(osd_file file, void* buffer, int length) {
    if (!file) return 0;
    return fread(buffer, 1, length, (FILE*)file);
}

int osd_fwrite(osd_file file, const void* buffer, int length) {
    if (!file) return 0;
    return fwrite(buffer, 1, length, (FILE*)file);
}

int osd_fseek(osd_file file, int offset, int whence) {
    if (!file) return -1;
    return fseek((FILE*)file, offset, whence);
}

int osd_ftell(osd_file file) {
    if (!file) return -1;
    return ftell((FILE*)file);
}

void osd_fclose(osd_file file) {
    if (file) {
        fclose((FILE*)file);
    }
}

/***************************************************************************
 * Initialization
 ***************************************************************************/

int osd_init(void) {
    /* Initialize timing */
    start_ticks = gettime();
    
    /* Initialize FAT filesystem for SD card */
    if (!fatInitDefault()) {
        printf("Warning: FAT initialization failed\n");
        /* Not fatal - ROMs can be embedded */
    }
    
    return 0;
}

void osd_exit(void) {
    osd_close_display();
    fatUnmount("sd:");
}
