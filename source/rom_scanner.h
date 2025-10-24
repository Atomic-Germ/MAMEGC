#ifndef ROM_SCANNER_H
#define ROM_SCANNER_H

#include <stdbool.h>
#include <stddef.h>

#define MAX_ROM_SETS 32
#define MAX_ROM_FILES 16

typedef struct {
    char filename[64];
    size_t expected_size;
    bool found;
    char full_path[256];
} rom_file_t;

typedef struct {
    char name[64];           /* e.g., "pacman" */
    char directory[256];     /* e.g., "carda:/roms/pacman/" */
    bool valid;              /* All required files found */
    int file_count;
    rom_file_t files[MAX_ROM_FILES];
} rom_set_t;

/* ROM scanner functions */
bool rom_scanner_init(void);
int scan_for_rom_sets(rom_set_t* rom_sets, int max_sets);
bool validate_rom_set(rom_set_t* rom_set);
void print_rom_sets(rom_set_t* rom_sets, int count);

/* Pac-Man specific validation */
bool is_pacman_rom_set(const char* directory);
bool validate_pacman_files(rom_set_t* rom_set);

#endif /* ROM_SCANNER_H */