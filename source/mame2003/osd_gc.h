/***************************************************************************
 * GameCube OSD (Operating System Dependent) Layer
 * 
 * This file provides GameCube-specific definitions and interfaces for MAME2003
 ***************************************************************************/

#ifndef OSD_GC_H
#define OSD_GC_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>

/***************************************************************************
 * Platform Definitions
 ***************************************************************************/

/* These may be defined by compiler flags */
#ifndef MAME_GC
#define MAME_GC
#endif

#ifndef LSB_FIRST
#define LSB_FIRST    /* GameCube/Wii are big-endian, but MAME expects little-endian ROMs */
#endif

#ifndef INLINE
#define INLINE static __inline__
#endif

/* Disable unused features for GameCube */
#define DISABLE_NETWORK
#define DISABLE_DRC
#define DISABLE_HISCORE

/***************************************************************************
 * Type Definitions
 ***************************************************************************/

/* MAME uses these types extensively */
#ifndef UINT8
typedef uint8_t   UINT8;
typedef uint16_t  UINT16;
typedef uint32_t  UINT32;
typedef uint64_t  UINT64;

typedef int8_t    INT8;
typedef int16_t   INT16;
typedef int32_t   INT32;
typedef int64_t   INT64;
#endif

/* Pointer-sized integers */
#ifdef __powerpc__
typedef UINT32 FPTR;
#else
typedef uintptr_t FPTR;
#endif

/***************************************************************************
 * File I/O Definitions
 ***************************************************************************/

typedef void* osd_file;

/***************************************************************************
 * Memory Management
 ***************************************************************************/

/* GameCube memory layout */
#define GC_MEM1_SIZE  (24 * 1024 * 1024)  /* 24MB main RAM */
#define GC_ARAM_SIZE  (16 * 1024 * 1024)  /* 16MB audio RAM */

/* Memory allocation wrappers */
void* osd_malloc(size_t size);
void* osd_calloc(size_t count, size_t size);
void  osd_free(void* ptr);

/***************************************************************************
 * Timing
 ***************************************************************************/

/* GameCube runs at 60Hz NTSC, 50Hz PAL */
#define GC_REFRESH_RATE 60.0

/* Get ticks (milliseconds) */
UINT32 osd_ticks(void);
UINT32 osd_ticks_per_second(void);

/***************************************************************************
 * Video
 ***************************************************************************/

/* GameCube display parameters */
#define GC_SCREEN_WIDTH   640
#define GC_SCREEN_HEIGHT  480

/* Video buffer structure */
typedef struct {
    void* framebuffer;
    int width;
    int height;
    int pitch;
} osd_bitmap;

/***************************************************************************
 * Input
 ***************************************************************************/

/* GameCube controller mapping */
#define GC_BUTTON_A      0x0001
#define GC_BUTTON_B      0x0002
#define GC_BUTTON_X      0x0004
#define GC_BUTTON_Y      0x0008
#define GC_BUTTON_START  0x0010
#define GC_BUTTON_Z      0x0020
#define GC_TRIGGER_L     0x0040
#define GC_TRIGGER_R     0x0080
#define GC_DPAD_UP       0x0100
#define GC_DPAD_DOWN     0x0200
#define GC_DPAD_LEFT     0x0400
#define GC_DPAD_RIGHT    0x0800

/***************************************************************************
 * Logging/Debug
 ***************************************************************************/

#ifdef DEBUG
#define logerror printf
#else
#define logerror(...)
#endif

/***************************************************************************
 * Math/Utility Macros
 ***************************************************************************/

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

#ifndef ABS
#define ABS(x) (((x)<0)?-(x):(x))
#endif

/* Bit manipulation */
#define BIT(x,n) (((x)>>(n))&1)

/***************************************************************************
 * Endianness Handling
 ***************************************************************************/

/* GameCube is big-endian, but most ROM data is little-endian */
/* These functions handle the conversion */

INLINE UINT16 BYTE_SWAP16(UINT16 x) {
    return (x >> 8) | (x << 8);
}

INLINE UINT32 BYTE_SWAP32(UINT32 x) {
    return ((x >> 24) & 0xff) |
           ((x >> 8) & 0xff00) |
           ((x << 8) & 0xff0000) |
           ((x << 24) & 0xff000000);
}

/* Read little-endian values (ROM data) */
#define READ_LE16(ptr) BYTE_SWAP16(*(UINT16*)(ptr))
#define READ_LE32(ptr) BYTE_SWAP32(*(UINT32*)(ptr))

/* Read big-endian values (native) */
#define READ_BE16(ptr) (*(UINT16*)(ptr))
#define READ_BE32(ptr) (*(UINT32*)(ptr))

/***************************************************************************
 * Function Declarations
 ***************************************************************************/

/* Initialization */
int  osd_init(void);
void osd_exit(void);

/* Video */
int  osd_create_display(int width, int height, int depth);
void osd_close_display(void);
void osd_update_video(void);

/* Input */
void osd_poll_inputs(void);
int  osd_key_pressed(int keycode);

/* File I/O */
osd_file osd_fopen(const char* path, const char* mode);
int  osd_fread(osd_file file, void* buffer, int length);
int  osd_fwrite(osd_file file, const void* buffer, int length);
int  osd_fseek(osd_file file, int offset, int whence);
int  osd_ftell(osd_file file);
void osd_fclose(osd_file file);

/* Timing */
void osd_sleep(int milliseconds);

#endif /* OSD_GC_H */
