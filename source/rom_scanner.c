#include "rom_scanner.h"
#include "mame2003/osd_gc.h"
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

/* Pac-Man ROM file requirements */
static const rom_file_t pacman_required_files[] = {
    {"pacman.6e", 4096, false, ""},  /* Program ROM 1 */
    {"pacman.6f", 4096, false, ""},  /* Program ROM 2 */
    {"pacman.6h", 4096, false, ""},  /* Program ROM 3 */
    {"pacman.6j", 4096, false, ""},  /* Program ROM 4 */
    {"pacman.5e", 4096, false, ""},  /* Character ROM */
    {"82s123.7f", 32, false, ""},    /* Color PROM 1 */
    {"82s126.4a", 256, false, ""},   /* Color PROM 2 */
};
static const int pacman_file_count = sizeof(pacman_required_files) / sizeof(rom_file_t);

bool rom_scanner_init(void) {
    if (!osd_sd_available()) {
        printf("ROM Scanner: No SD2GC available\n");
        return false;
    }
    
    printf("ROM Scanner: Initialized with mount point: %s\n", osd_get_sd_mount());
    return true;
}

int scan_for_rom_sets(rom_set_t* rom_sets, int max_sets) {
    if (!osd_sd_available()) {
        return 0;
    }
    
    char roms_path[64];
    snprintf(roms_path, sizeof(roms_path), "%s/roms", osd_get_sd_mount());
    
    printf("Scanning for ROMs in: %s\n", roms_path);
    
    DIR* roms_dir = opendir(roms_path);
    if (!roms_dir) {
        printf("ROM Scanner: Could not open %s\n", roms_path);
        printf("Create directory: %s\n", roms_path);
        return 0;
    }
    
    int rom_count = 0;
    struct dirent* entry;
    
    while ((entry = readdir(roms_dir)) != NULL && rom_count < max_sets) {
        /* Skip hidden files and current/parent directories */
        if (entry->d_name[0] == '.') continue;
        
        char full_path[256];
        snprintf(full_path, sizeof(full_path), "%s/%s", roms_path, entry->d_name);
        
        struct stat statbuf;
        if (stat(full_path, &statbuf) == 0 && S_ISDIR(statbuf.st_mode)) {
            /* Found a directory - check if it's a ROM set */
            rom_set_t* rom_set = &rom_sets[rom_count];
            
            strncpy(rom_set->name, entry->d_name, sizeof(rom_set->name) - 1);
            rom_set->name[sizeof(rom_set->name) - 1] = '\0';
            
            snprintf(rom_set->directory, sizeof(rom_set->directory), "%s/", full_path);
            rom_set->valid = false;
            rom_set->file_count = 0;
            
            printf("Found ROM directory: %s\n", rom_set->name);
            
            /* Validate the ROM set */
            if (validate_rom_set(rom_set)) {
                printf("  -> Valid ROM set\n");
                rom_count++;
            } else {
                printf("  -> Invalid ROM set (missing files)\n");
            }
        }
    }
    
    closedir(roms_dir);
    printf("Found %d valid ROM sets\n", rom_count);
    return rom_count;
}

bool validate_rom_set(rom_set_t* rom_set) {
    /* Currently only supports Pac-Man */
    if (is_pacman_rom_set(rom_set->directory)) {
        return validate_pacman_files(rom_set);
    }
    
    printf("Unknown ROM set type: %s\n", rom_set->name);
    return false;
}

bool is_pacman_rom_set(const char* directory) {
    /* Check if directory name contains "pacman" (case insensitive) */
    char dir_lower[256];
    strncpy(dir_lower, directory, sizeof(dir_lower) - 1);
    dir_lower[sizeof(dir_lower) - 1] = '\0';
    
    /* Convert to lowercase */
    for (int i = 0; dir_lower[i]; i++) {
        if (dir_lower[i] >= 'A' && dir_lower[i] <= 'Z') {
            dir_lower[i] += 32;
        }
    }
    
    return strstr(dir_lower, "pacman") != NULL;
}

bool validate_pacman_files(rom_set_t* rom_set) {
    /* Copy required files list */
    rom_set->file_count = pacman_file_count;
    for (int i = 0; i < pacman_file_count; i++) {
        rom_set->files[i] = pacman_required_files[i];
        
        /* Build full path */
        snprintf(rom_set->files[i].full_path, sizeof(rom_set->files[i].full_path),
                 "%s%s", rom_set->directory, rom_set->files[i].filename);
    }
    
    /* Check each required file */
    int found_files = 0;
    for (int i = 0; i < rom_set->file_count; i++) {
        struct stat statbuf;
        if (stat(rom_set->files[i].full_path, &statbuf) == 0) {
            if (statbuf.st_size == (off_t)rom_set->files[i].expected_size) {
                rom_set->files[i].found = true;
                found_files++;
                printf("  Found: %s (%zu bytes)\n", 
                       rom_set->files[i].filename, rom_set->files[i].expected_size);
            } else {
                printf("  Wrong size: %s (expected %zu, got %ld)\n",
                       rom_set->files[i].filename, 
                       rom_set->files[i].expected_size, 
                       (long)statbuf.st_size);
            }
        } else {
            printf("  Missing: %s\n", rom_set->files[i].filename);
        }
    }
    
    rom_set->valid = (found_files == rom_set->file_count);
    return rom_set->valid;
}

void print_rom_sets(rom_set_t* rom_sets, int count) {
    printf("\n=== ROM Sets Found ===\n");
    for (int i = 0; i < count; i++) {
        printf("%d. %s (%s)\n", i + 1, rom_sets[i].name, 
               rom_sets[i].valid ? "VALID" : "INVALID");
        printf("   Path: %s\n", rom_sets[i].directory);
        if (rom_sets[i].valid) {
            printf("   Files: %d/%d found\n", rom_sets[i].file_count, rom_sets[i].file_count);
        }
    }
    printf("======================\n\n");
}