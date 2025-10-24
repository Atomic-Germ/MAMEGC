# Phase 6B Implementation: ROM File Detection

## Goal
Scan SD card for MAME ROM files and directories, building on Phase 6A's working SD card mounting.

## Prerequisites
✅ Phase 6A complete: SD card mounting from carda:/cardb: working  
✅ File system access verified  
✅ Existing functionality still works  

## Implementation Tasks

### 1. Create rom_scanner.h

**New file: `source/rom_scanner.h`**
```c
#ifndef ROM_SCANNER_H
#define ROM_SCANNER_H

#include <stdbool.h>

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
```

### 2. Create rom_scanner.c

**New file: `source/rom_scanner.c`**
```c
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
        printf("ROM Scanner: No SD card available\n");
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
            if (statbuf.st_size == rom_set->files[i].expected_size) {
                rom_set->files[i].found = true;
                found_files++;
                printf("  Found: %s (%zu bytes)\n", 
                       rom_set->files[i].filename, statbuf.st_size);
            } else {
                printf("  Wrong size: %s (expected %zu, got %ld)\n",
                       rom_set->files[i].filename, 
                       rom_set->files[i].expected_size, 
                       statbuf.st_size);
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
```

### 3. Integrate with main.c

**Add to main.c includes:**
```c
#include "rom_scanner.h"
```

**Add ROM scanning to main() after input test:**
```c
/* Test ROM scanning */
if (osd_sd_available()) {
    printf("\n=== ROM Scanner Test ===\n");
    
    if (rom_scanner_init()) {
        rom_set_t rom_sets[MAX_ROM_SETS];
        int rom_count = scan_for_rom_sets(rom_sets, MAX_ROM_SETS);
        
        if (rom_count > 0) {
            print_rom_sets(rom_sets, rom_count);
            
            /* Find Pac-Man ROM set for next phase */
            for (int i = 0; i < rom_count; i++) {
                if (rom_sets[i].valid && is_pacman_rom_set(rom_sets[i].directory)) {
                    printf("Pac-Man ROM set ready at: %s\n", rom_sets[i].directory);
                    break;
                }
            }
        } else {
            printf("No valid ROM sets found\n");
            printf("Place ROM files in: %s/roms/pacman/\n", osd_get_sd_mount());
        }
    }
} else {
    printf("No SD card - ROM scanning skipped\n");
}
```

### 4. Update Makefile.gc

**Add new source to SOURCES line:**
```makefile
SOURCES := source source/mame2003 source/mame2003/cpu/z80 source/drivers/pacman source/video source/rom_scanner
```

Wait, that's not right. The rom_scanner.c is in the source directory, so just make sure it gets included:

**Verify SOURCES includes source directory (should already be there):**
```makefile
SOURCES := source source/mame2003 source/mame2003/cpu/z80 source/drivers/pacman source/video
```

The source directory should pick up rom_scanner.c automatically.

## Testing Steps

### Build Test:
```bash
./docker-build.sh build gc
```

### SD Card Setup:
1. Create directory structure on SD card:
   ```
   carda:/
   └── roms/
       └── pacman/
           ├── pacman.6e
           ├── pacman.6f  
           ├── pacman.6h
           ├── pacman.6j
           ├── pacman.5e
           ├── 82s123.7f
           └── 82s126.4a
   ```

### Hardware Test:
1. Copy ROM files to SD card in correct structure
2. Copy `mamegc-gc.dol` to SD card
3. Boot GameCube with Swiss
4. Run DOL file
5. Check console output for ROM detection

### Expected Output:
```
=== ROM Scanner Test ===
ROM Scanner: Initialized with mount point: carda:
Scanning for ROMs in: carda:/roms
Found ROM directory: pacman
  Found: pacman.6e (4096 bytes)
  Found: pacman.6f (4096 bytes)
  Found: pacman.6h (4096 bytes)
  Found: pacman.6j (4096 bytes)
  Found: pacman.5e (4096 bytes)
  Found: 82s123.7f (32 bytes)
  Found: 82s126.4a (256 bytes)
  -> Valid ROM set
Found 1 valid ROM sets

=== ROM Sets Found ===
1. pacman (VALID)
   Path: carda:/roms/pacman/
   Files: 7/7 found
======================

Pac-Man ROM set ready at: carda:/roms/pacman/
```

## Success Criteria for Phase 6B:

✅ Code compiles without errors  
✅ ROM scanner initializes successfully  
✅ ROM directories detected on SD card  
✅ Pac-Man ROM files validated (all 7 files found with correct sizes)  
✅ ROM set marked as VALID  
✅ Clear error messages when files missing  
✅ Existing functionality still works  

## Troubleshooting:

**If no ROM directories found:**
- Check SD card path structure: `carda:/roms/pacman/`
- Verify ROM files are in correct directory
- Check directory permissions

**If ROM files wrong size:**
- Verify you have correct Pac-Man ROM dump
- Check for corrupted downloads
- Some ROM files might be compressed

**If build fails:**
- Check all includes are correct
- Verify new files added to source directory
- Check for syntax errors

## Next Phase:
Once Phase 6B is complete and detecting ROM files correctly, proceed to Phase 6C (ROM Loading Infrastructure).