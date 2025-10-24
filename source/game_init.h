#ifndef GAME_INIT_H
#define GAME_INIT_H

#include <stdbool.h>
#include "rom_scanner.h"

/* Game initialization functions */
bool game_init_system(void);
bool init_pacman_game(const rom_set_t* rom_set);
bool start_game_execution(void);
void reset_game_system(void);

/* Game state */
typedef enum {
    GAME_STATE_UNINITIALIZED,
    GAME_STATE_ROMS_LOADED,
    GAME_STATE_INITIALIZED,
    GAME_STATE_RUNNING,
    GAME_STATE_ERROR
} game_state_t;

extern game_state_t g_game_state;

/* Hardware initialization */
bool init_pacman_hardware(void);
bool init_pacman_memory_map(void);
bool init_pacman_video(void);

#endif /* GAME_INIT_H */