/***************************************************************************
 * MAME2003 Core Definitions
 ***************************************************************************/

#ifndef MAME2003_H
#define MAME2003_H

#include "osd_gc.h"

/* MAME2003 version */
#define MAME2003_VERSION "0.78"

/* Core configuration */
#define HAS_Z80 1

/* Logging */
#ifdef DEBUG
#define logerror printf
#else
#define logerror(...)
#endif

/* Libretro log callback (stub for GameCube) */
typedef void (*retro_log_printf_t)(int level, const char *fmt, ...);

/* Libretro stubs */
#define RETRO_ENVIRONMENT_GET_LOG_INTERFACE 0
#define RETRO_LOG_WARN 0

/* Libretro log callback structure */
struct retro_log_callback {
    retro_log_printf_t log;
};

/* Stub environment callback */
static inline int environ_cb(unsigned cmd, void *data) {
    return 0;
}

#endif /* MAME2003_H */
