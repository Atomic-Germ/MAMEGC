/***************************************************************************
 * MAME2003 Memory System for GameCube
 * 
 * Handles ROM/RAM allocation and memory mapping for emulated systems
 ***************************************************************************/

#ifndef MEMORY_H
#define MEMORY_H

#include "osd_gc.h"

/***************************************************************************
 * Memory Region Types
 ***************************************************************************/

#define REGION_CPU1     0x01  /* Main CPU memory */
#define REGION_CPU2     0x02  /* Second CPU memory */
#define REGION_GFX1     0x10  /* Graphics region 1 */
#define REGION_GFX2     0x11  /* Graphics region 2 */
#define REGION_PROMS    0x20  /* Color PROMs */
#define REGION_SOUND1   0x30  /* Sound region 1 */
#define REGION_USER1    0x40  /* User region 1 */

/***************************************************************************
 * Memory Bank Structure
 ***************************************************************************/

typedef struct {
    UINT8* base;          /* Base pointer to memory */
    UINT32 size;          /* Size in bytes */
    UINT32 offset;        /* Offset into region */
    int read_only;        /* 1 = ROM, 0 = RAM */
} memory_bank_t;

/***************************************************************************
 * Memory Region Structure
 ***************************************************************************/

typedef struct {
    UINT8* base;          /* Base pointer */
    UINT32 size;          /* Size in bytes */
    UINT32 type;          /* Region type */
    const char* name;     /* Region name */
} memory_region_t;

/***************************************************************************
 * Memory System Functions
 ***************************************************************************/

/* Initialize memory system */
int memory_init(void);
void memory_shutdown(void);

/* Region management */
int memory_region_alloc(int region, UINT32 size, const char* name);
void memory_region_free(int region);
UINT8* memory_region_get_base(int region);
UINT32 memory_region_get_size(int region);

/* Set read/write handlers for address ranges */
void memory_set_bankptr(int bank, UINT8* base);
void memory_install_read_handler(UINT32 start, UINT32 end, UINT8 (*handler)(UINT32));
void memory_install_write_handler(UINT32 start, UINT32 end, void (*handler)(UINT32, UINT8));

/* Direct memory access (for simple systems) */
UINT8 memory_read_byte(UINT32 address);
void memory_write_byte(UINT32 address, UINT8 data);

/* ROM loading */
int memory_load_rom(int region, const UINT8* data, UINT32 size);

/* Memory mapping helpers */
void memory_map_rom(UINT32 start_addr, UINT32 end_addr, UINT8* base);
void memory_map_ram(UINT32 start_addr, UINT32 end_addr, UINT8* base);

/* Debugging */
void memory_dump(UINT32 address, UINT32 length);

#endif /* MEMORY_H */
