/***************************************************************************
 * CPU Interface Implementation
 ***************************************************************************/

#include "cpuintrf.h"
#include "memory.h"

/* Forward declare pacman state for memory access */
extern void* g_pacman_state;
extern UINT8 pacman_read_byte(void* state, UINT16 address);
extern void pacman_write_byte(void* state, UINT16 address, UINT8 data);

/* Stub implementations */

void cpu_setOPbase16(int cpu, unsigned val) {
    /* TODO: Implement PC change notification */
}

void cpu_set_irq_line(int cpu, int irqline, int state) {
    /* TODO: Implement IRQ handling */
}

void change_pc16(unsigned pc) {
    /* TODO: Implement PC change callback */
}

int cpu_getactivecpu(void) {
    return 0; /* TODO: Return actual active CPU */
}

/* Memory access - route through Pac-Man driver if available */
UINT8 cpu_readop(UINT32 address) {
    /* Mask to 16-bit address space - Z80 is only 16-bit */
    address &= 0xFFFF;
    
    /* Use Pac-Man driver if available */
    if (g_pacman_state) {
        return pacman_read_byte(g_pacman_state, (UINT16)address);
    }
    
    /* Fall back to direct memory access */
    return memory_read_byte(address);
}

UINT8 cpu_readop_arg(UINT32 address) {
    address &= 0xFFFF;  /* Mask to 16-bit */
    
    if (g_pacman_state) {
        return pacman_read_byte(g_pacman_state, (UINT16)address);
    }
    
    return memory_read_byte(address);
}

UINT8 cpu_readmem16(UINT32 address) {
    address &= 0xFFFF;  /* Mask to 16-bit */
    
    if (g_pacman_state) {
        return pacman_read_byte(g_pacman_state, (UINT16)address);
    }
    
    return memory_read_byte(address);
}

void cpu_writemem16(UINT32 address, UINT8 data) {
    address &= 0xFFFF;  /* Mask to 16-bit */
    
    if (g_pacman_state) {
        pacman_write_byte(g_pacman_state, (UINT16)address, data);
        return;
    }
    
    memory_write_byte(address, data);
}

UINT16 cpu_readport16(UINT16 port) {
    return 0; /* TODO: Implement I/O port reads */
}

void cpu_writeport16(UINT16 port, UINT16 data) {
    /* TODO: Implement I/O port writes */
}
