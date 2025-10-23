/***************************************************************************
 * MAME2003 GameCube Port - Main Header
 ***************************************************************************/

#ifndef MAME2003_GC_H
#define MAME2003_GC_H

#include "mame2003/osd_gc.h"

/***************************************************************************
 * Version Information
 ***************************************************************************/

#define MAME2003_GC_VERSION_MAJOR 0
#define MAME2003_GC_VERSION_MINOR 2
#define MAME2003_GC_VERSION_PATCH 0
#define MAME2003_GC_VERSION_STRING "0.2.0"

/***************************************************************************
 * Configuration
 ***************************************************************************/

typedef struct {
    /* Video settings */
    int screen_width;
    int screen_height;
    int refresh_rate;
    
    /* Audio settings */
    int audio_enabled;
    int audio_sample_rate;
    
    /* Input settings */
    int controller_port;
    
    /* Emulation settings */
    int frameskip;
    int auto_frameskip;
    
    /* Debug settings */
    int show_fps;
    int debug_mode;
} mame2003_config_t;

/***************************************************************************
 * MAME2003 State
 ***************************************************************************/

typedef enum {
    MAME_STATE_INIT,
    MAME_STATE_RUNNING,
    MAME_STATE_PAUSED,
    MAME_STATE_EXIT
} mame_state_t;

typedef struct {
    mame_state_t state;
    mame2003_config_t config;
    
    /* Performance tracking */
    int current_fps;
    int frames_rendered;
    int frames_skipped;
    
    /* Memory usage */
    size_t memory_used;
    size_t memory_peak;
} mame2003_context_t;

/***************************************************************************
 * Function Declarations
 ***************************************************************************/

/* Initialization */
int  mame2003_init(mame2003_context_t* ctx);
void mame2003_shutdown(mame2003_context_t* ctx);

/* Game loading */
int  mame2003_load_game(mame2003_context_t* ctx, const char* game_name);
void mame2003_unload_game(mame2003_context_t* ctx);

/* Emulation control */
void mame2003_reset(mame2003_context_t* ctx);
void mame2003_pause(mame2003_context_t* ctx);
void mame2003_resume(mame2003_context_t* ctx);
int  mame2003_run_frame(mame2003_context_t* ctx);

/* Configuration */
void mame2003_set_config(mame2003_context_t* ctx, const mame2003_config_t* config);
void mame2003_get_config(const mame2003_context_t* ctx, mame2003_config_t* config);

/* State management */
int  mame2003_save_state(mame2003_context_t* ctx, const char* path);
int  mame2003_load_state(mame2003_context_t* ctx, const char* path);

/* Info */
const char* mame2003_get_version(void);
void mame2003_get_stats(const mame2003_context_t* ctx, char* buffer, size_t size);

#endif /* MAME2003_GC_H */
