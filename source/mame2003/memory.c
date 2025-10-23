/***************************************************************************
 * MAME2003 Memory System Implementation
 ***************************************************************************/

#include "memory.h"
#include "osd_gc.h"
#include <string.h>
#include <stdio.h>

/***************************************************************************
 * Memory System State
 ***************************************************************************/

#define MAX_MEMORY_REGIONS 16
#define MAX_MEMORY_SIZE (4 * 1024 * 1024)  /* 4MB max for emulated system */

static memory_region_t regions[MAX_MEMORY_REGIONS];
static int num_regions = 0;

/* Simple flat memory map for now (will expand for banking later) */
static UINT8* memory_map[256];  /* 256 pages of 256 bytes = 64KB address space */
static int memory_map_ro[256];  /* Read-only flags */

/***************************************************************************
 * Memory System Initialization
 ***************************************************************************/

int memory_init(void) {
    int i;
    
    /* Clear all regions */
    memset(regions, 0, sizeof(regions));
    num_regions = 0;
    
    /* Clear memory map */
    for (i = 0; i < 256; i++) {
        memory_map[i] = NULL;
        memory_map_ro[i] = 0;
    }
    
    printf("Memory system initialized\n");
    return 0;
}

void memory_shutdown(void) {
    int i;
    
    /* Free all allocated regions */
    for (i = 0; i < num_regions; i++) {
        if (regions[i].base) {
            osd_free(regions[i].base);
            regions[i].base = NULL;
        }
    }
    
    num_regions = 0;
    printf("Memory system shut down\n");
}

/***************************************************************************
 * Region Management
 ***************************************************************************/

int memory_region_alloc(int region, UINT32 size, const char* name) {
    UINT8* base;
    
    if (num_regions >= MAX_MEMORY_REGIONS) {
        printf("ERROR: Too many memory regions\n");
        return -1;
    }
    
    if (size > MAX_MEMORY_SIZE) {
        printf("ERROR: Region too large: %u bytes\n", size);
        return -1;
    }
    
    /* Allocate memory */
    base = (UINT8*)osd_malloc(size);
    if (!base) {
        printf("ERROR: Failed to allocate %u bytes for region %s\n", size, name);
        return -1;
    }
    
    /* Clear memory */
    memset(base, 0, size);
    
    /* Store region info */
    regions[num_regions].base = base;
    regions[num_regions].size = size;
    regions[num_regions].type = region;
    regions[num_regions].name = name;
    
    printf("Allocated region %s: %u bytes at %p\n", name, size, base);
    
    num_regions++;
    return 0;
}

void memory_region_free(int region) {
    int i;
    
    for (i = 0; i < num_regions; i++) {
        if (regions[i].type == region && regions[i].base) {
            printf("Freeing region %s\n", regions[i].name);
            osd_free(regions[i].base);
            regions[i].base = NULL;
        }
    }
}

UINT8* memory_region_get_base(int region) {
    int i;
    
    for (i = 0; i < num_regions; i++) {
        if (regions[i].type == region) {
            return regions[i].base;
        }
    }
    
    return NULL;
}

UINT32 memory_region_get_size(int region) {
    int i;
    
    for (i = 0; i < num_regions; i++) {
        if (regions[i].type == region) {
            return regions[i].size;
        }
    }
    
    return 0;
}

/***************************************************************************
 * Memory Mapping
 ***************************************************************************/

void memory_set_bankptr(int bank, UINT8* base) {
    if (bank < 0 || bank >= 256) {
        return;
    }
    
    memory_map[bank] = base;
    memory_map_ro[bank] = 1;  /* Assume ROM by default */
}

/***************************************************************************
 * Direct Memory Access
 ***************************************************************************/

UINT8 memory_read_byte(UINT32 address) {
    UINT8 page = (address >> 8) & 0xFF;
    UINT8 offset = address & 0xFF;
    
    if (memory_map[page]) {
        return memory_map[page][offset];
    }
    
    /* Unmapped memory returns 0xFF */
    return 0xFF;
}

void memory_write_byte(UINT32 address, UINT8 data) {
    UINT8 page = (address >> 8) & 0xFF;
    UINT8 offset = address & 0xFF;
    
    if (memory_map[page] && !memory_map_ro[page]) {
        memory_map[page][offset] = data;
    }
}

/***************************************************************************
 * ROM Loading
 ***************************************************************************/

int memory_load_rom(int region, const UINT8* data, UINT32 size) {
    UINT8* base = memory_region_get_base(region);
    UINT32 region_size = memory_region_get_size(region);
    
    if (!base) {
        printf("ERROR: Region not allocated\n");
        return -1;
    }
    
    if (size > region_size) {
        printf("ERROR: ROM too large for region (%u > %u)\n", size, region_size);
        return -1;
    }
    
    /* Copy ROM data */
    memcpy(base, data, size);
    
    printf("Loaded ROM: %u bytes\n", size);
    return 0;
}

/***************************************************************************
 * Memory Mapping Helper
 ***************************************************************************/

void memory_map_rom(UINT32 start_addr, UINT32 end_addr, UINT8* base) {
    UINT32 addr;
    UINT32 offset = 0;
    
    for (addr = start_addr; addr <= end_addr; addr += 256) {
        UINT8 page = (addr >> 8) & 0xFF;
        memory_map[page] = base + offset;
        memory_map_ro[page] = 1;  /* ROM is read-only */
        offset += 256;
    }
    
    printf("Mapped ROM: $%04X-$%04X (%u bytes)\n", start_addr, end_addr, end_addr - start_addr + 1);
}

void memory_map_ram(UINT32 start_addr, UINT32 end_addr, UINT8* base) {
    UINT32 addr;
    UINT32 offset = 0;
    
    for (addr = start_addr; addr <= end_addr; addr += 256) {
        UINT8 page = (addr >> 8) & 0xFF;
        memory_map[page] = base + offset;
        memory_map_ro[page] = 0;  /* RAM is read/write */
        offset += 256;
    }
    
    printf("Mapped RAM: $%04X-$%04X (%u bytes)\n", start_addr, end_addr, end_addr - start_addr + 1);
}

/***************************************************************************
 * Debugging
 ***************************************************************************/

void memory_dump(UINT32 address, UINT32 length) {
    UINT32 i;
    
    printf("\nMemory dump at $%04X (%u bytes):\n", address, length);
    
    for (i = 0; i < length; i++) {
        if (i % 16 == 0) {
            printf("%04X: ", address + i);
        }
        
        printf("%02X ", memory_read_byte(address + i));
        
        if (i % 16 == 15) {
            printf("\n");
        }
    }
    
    if (length % 16 != 0) {
        printf("\n");
    }
}

/* Export mapping functions */
void memory_install_read_handler(UINT32 start, UINT32 end, UINT8 (*handler)(UINT32)) {
    /* TODO: Implement handler-based reads */
    printf("TODO: Install read handler $%04X-$%04X\n", start, end);
}

void memory_install_write_handler(UINT32 start, UINT32 end, void (*handler)(UINT32, UINT8)) {
    /* TODO: Implement handler-based writes */
    printf("TODO: Install write handler $%04X-$%04X\n", start, end);
}
