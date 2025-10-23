/***************************************************************************
 * CPU Interface Implementation
 ***************************************************************************/

#include "cpuintrf.h"
#include "memory.h"

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

/* Memory access - now using memory system */
UINT8 cpu_readop(UINT32 address) {
    /* Mask to 16-bit address space - Z80 is only 16-bit */
    address &= 0xFFFF;
    return memory_read_byte(address);
}

UINT8 cpu_readop_arg(UINT32 address) {
    address &= 0xFFFF;  /* Mask to 16-bit */
    return memory_read_byte(address);
}

UINT8 cpu_readmem16(UINT32 address) {
    address &= 0xFFFF;  /* Mask to 16-bit */
    return memory_read_byte(address);
}

void cpu_writemem16(UINT32 address, UINT8 data) {
    address &= 0xFFFF;  /* Mask to 16-bit */
    memory_write_byte(address, data);
}

UINT16 cpu_readport16(UINT16 port) {
    return 0; /* TODO: Implement I/O port reads */
}

void cpu_writeport16(UINT16 port, UINT16 data) {
    /* TODO: Implement I/O port writes */
}
