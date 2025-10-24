#include "rom_loader.h"
#include "mame2003/memory.h"
#include "mame2003/osd_gc.h"
#include <stdio.h>
#include <string.h>

/* Global ROM loading status */
rom_load_status_t g_rom_status = {false, false, false, 0};

/* ROM memory regions - we'll allocate these statically for now */
static uint8_t program_rom_data[0x4000];    /* 16KB program ROM */
static uint8_t character_rom_data[0x1000];  /* 4KB character ROM */
static uint8_t color_prom_data[0x0120];     /* 288 bytes color PROMs */

bool rom_loader_init(void) {
    /* Clear ROM loading status */
    memset(&g_rom_status, 0, sizeof(g_rom_status));
    
    /* Clear ROM memory regions */
    memset(program_rom_data, 0, sizeof(program_rom_data));
    memset(character_rom_data, 0, sizeof(character_rom_data));
    memset(color_prom_data, 0, sizeof(color_prom_data));
    
    printf("ROM Loader: Initialized\n");
    printf("ROM Loader: Program ROM region: %p (16KB)\n", program_rom_data);
    printf("ROM Loader: Character ROM region: %p (4KB)\n", character_rom_data);
    printf("ROM Loader: Color PROM region: %p (288 bytes)\n", color_prom_data);
    
    return true;
}

bool load_rom_file(const char* filepath, uint8_t* dest, size_t size) {
    if (!filepath || !dest || size == 0) {
        printf("ROM Loader: Invalid parameters\n");
        return false;
    }
    
    printf("ROM Loader: Loading %s...\n", filepath);
    
    FILE* file = fopen(filepath, "rb");
    if (!file) {
        printf("ROM Loader: Cannot open %s\n", filepath);
        return false;
    }
    
    /* Get file size */
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    if (file_size != (long)size) {
        printf("ROM Loader: Size mismatch for %s (expected %zu, got %ld)\n", 
               filepath, size, file_size);
        fclose(file);
        return false;
    }
    
    /* Read file data */
    size_t bytes_read = fread(dest, 1, size, file);
    fclose(file);
    
    if (bytes_read != size) {
        printf("ROM Loader: Read error for %s (read %zu of %zu bytes)\n",
               filepath, bytes_read, size);
        return false;
    }
    
    printf("ROM Loader: Loaded %s (%zu bytes)\n", filepath, size);
    g_rom_status.total_bytes_loaded += size;
    return true;
}

bool load_pacman_roms(const rom_set_t* rom_set) {
    if (!rom_set || !rom_set->valid) {
        printf("ROM Loader: Invalid ROM set\n");
        return false;
    }
    
    printf("ROM Loader: Loading Pac-Man ROMs from %s\n", rom_set->directory);
    
    /* Get memory regions */
    uint8_t* prog_rom = get_program_rom_region();
    uint8_t* char_rom = get_character_rom_region();
    uint8_t* col_prom = get_color_prom_region();
    
    if (!prog_rom || !char_rom || !col_prom) {
        printf("ROM Loader: Memory regions not initialized\n");
        return false;
    }
    
    /* Load program ROMs (16KB total: 4 x 4KB files) */
    printf("Loading program ROMs...\n");
    for (int i = 0; i < 4; i++) {
        const rom_file_t* rom_file = &rom_set->files[i]; /* pacman.6e, 6f, 6h, 6j */
        uint8_t* dest = prog_rom + (i * 0x1000); /* 4KB chunks */
        
        if (!load_rom_file(rom_file->full_path, dest, 0x1000)) {
            printf("ROM Loader: Failed to load program ROM %d\n", i);
            return false;
        }
    }
    g_rom_status.program_roms_loaded = true;
    
    /* Load character ROM (4KB) */
    printf("Loading character ROM...\n");
    const rom_file_t* char_file = &rom_set->files[4]; /* pacman.5e */
    if (!load_rom_file(char_file->full_path, char_rom, 0x1000)) {
        printf("ROM Loader: Failed to load character ROM\n");
        return false;
    }
    g_rom_status.character_rom_loaded = true;
    
    /* Load color PROMs */
    printf("Loading color PROMs...\n");
    const rom_file_t* prom1 = &rom_set->files[5]; /* 82s123.7f (32 bytes) */
    const rom_file_t* prom2 = &rom_set->files[6]; /* 82s126.4a (256 bytes) */
    
    if (!load_rom_file(prom1->full_path, col_prom, 32)) {
        printf("ROM Loader: Failed to load color PROM 1\n");
        return false;
    }
    
    if (!load_rom_file(prom2->full_path, col_prom + 32, 256)) {
        printf("ROM Loader: Failed to load color PROM 2\n");
        return false;
    }
    g_rom_status.color_proms_loaded = true;
    
    printf("ROM Loader: All Pac-Man ROMs loaded successfully (%zu bytes total)\n",
           g_rom_status.total_bytes_loaded);
    
    /* Verify loading */
    verify_loaded_roms();
    
    return true;
}

void verify_loaded_roms(void) {
    printf("\n=== ROM Loading Verification ===\n");
    
    uint8_t* prog_rom = get_program_rom_region();
    uint8_t* char_rom = get_character_rom_region();
    uint8_t* col_prom = get_color_prom_region();
    
    if (g_rom_status.program_roms_loaded && prog_rom) {
        printf("Program ROM: LOADED\n");
        printf("  First bytes: %02X %02X %02X %02X\n", 
               prog_rom[0], prog_rom[1], prog_rom[2], prog_rom[3]);
        printf("  ROM 1 start: %02X %02X %02X %02X\n",
               prog_rom[0x0000], prog_rom[0x0001], prog_rom[0x0002], prog_rom[0x0003]);
        printf("  ROM 2 start: %02X %02X %02X %02X\n",
               prog_rom[0x1000], prog_rom[0x1001], prog_rom[0x1002], prog_rom[0x1003]);
    }
    
    if (g_rom_status.character_rom_loaded && char_rom) {
        printf("Character ROM: LOADED\n");
        printf("  First bytes: %02X %02X %02X %02X\n",
               char_rom[0], char_rom[1], char_rom[2], char_rom[3]);
    }
    
    if (g_rom_status.color_proms_loaded && col_prom) {
        printf("Color PROMs: LOADED\n");
        printf("  PROM1 first bytes: %02X %02X %02X %02X\n",
               col_prom[0], col_prom[1], col_prom[2], col_prom[3]);
        printf("  PROM2 first bytes: %02X %02X %02X %02X\n",
               col_prom[32], col_prom[33], col_prom[34], col_prom[35]);
    }
    
    printf("Total ROM data loaded: %zu bytes\n", g_rom_status.total_bytes_loaded);
    printf("================================\n\n");
}

/* Memory region getters */
uint8_t* get_program_rom_region(void) {
    return program_rom_data;
}

uint8_t* get_character_rom_region(void) {
    return character_rom_data;
}

uint8_t* get_color_prom_region(void) {
    return color_prom_data;
}