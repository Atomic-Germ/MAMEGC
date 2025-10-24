#ifndef ROM_LOADER_H
#define ROM_LOADER_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "rom_scanner.h"

/* ROM loading functions */
bool rom_loader_init(void);
bool load_rom_file(const char* filepath, uint8_t* dest, size_t size);
bool load_pacman_roms(const rom_set_t* rom_set);
void verify_loaded_roms(void);

/* Memory region helpers */
uint8_t* get_program_rom_region(void);
uint8_t* get_character_rom_region(void);
uint8_t* get_color_prom_region(void);

/* ROM loading status */
typedef struct {
    bool program_roms_loaded;
    bool character_rom_loaded;
    bool color_proms_loaded;
    size_t total_bytes_loaded;
} rom_load_status_t;

extern rom_load_status_t g_rom_status;

#endif /* ROM_LOADER_H */